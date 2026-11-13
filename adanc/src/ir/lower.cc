#include "lower.hh"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

#include <expected>
#include <string>
#include <string_view>
#include <vector>

#include "ast.hh"

ASTLowerer::ASTLowerer(llvm::LLVMContext &context, std::string_view module_name, Diagnostics &diagnostics) : context(context), builder(context), _module(std::make_unique<llvm::Module>(module_name, context)), diagnostics(diagnostics)
{
    auto *print_type = llvm::FunctionType::get(builder.getVoidTy(), {builder.getPtrTy()}, false);
    llvm::Function::Create(print_type, llvm::Function::ExternalLinkage, "print", *_module);
}

llvm::Type *ASTLowerer::lower_type(const TypePtr &type, bool allow_void)
{
    const TypePtr resolved = prune(type);
    switch (resolved->kind)
    {
    case TypeKind::Int:
        return builder.getInt64Ty();
    case TypeKind::Float:
        return builder.getDoubleTy();
    case TypeKind::Bool:
        return builder.getInt1Ty();
    case TypeKind::String:
        return builder.getPtrTy();
    case TypeKind::Void:
        if (allow_void)
            return builder.getVoidTy();
        break;
    default:
        break;
    }

    diagnostics.error("IR lowering does not support this type yet");
}

void ASTLowerer::declare_function(std::string_view name, const TypePtr &function_type)
{
    const TypePtr resolved_function_type = prune(function_type);
    if (resolved_function_type->kind != TypeKind::Function)
        diagnostics.error("Cannot declare an LLVM function from a non-function symbol");

    if (_module->getFunction(name) != nullptr)
        diagnostics.error("An LLVM function with this name is already declared");

    llvm::Type *return_type = lower_type(resolved_function_type->result, true);
    std::vector<llvm::Type *> parameter_types;
    parameter_types.reserve(resolved_function_type->params.size());
    for (const TypePtr &parameter : resolved_function_type->params)
        parameter_types.push_back(lower_type(parameter));

    auto *type = llvm::FunctionType::get(return_type, parameter_types, false);
    llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, *_module);
}

std::expected<llvm::Function *, std::string> ASTLowerer::lower_function(const FuncDecl &declaration)
{
    llvm::Function *function = _module->getFunction(declaration.name);
    if (function == nullptr)
        diagnostics.error("Function was not declared before its body was lowered");

    if (function->arg_size() != declaration.params.size())
        diagnostics.error("Function parameter count does not match its LLVM declaration");

    local_values.clear();
    std::size_t parameter_index{};
    for (llvm::Argument &argument : function->args())
    {
        const std::string &parameter_name = declaration.params[parameter_index++];
        argument.setName(parameter_name);
        local_values.emplace(parameter_name, &argument);
    }

    builder.SetInsertPoint(llvm::BasicBlock::Create(context, "entry", function));

    for (const auto &stmt : declaration.body.stmts)
        lower_stmt(*stmt);

    std::string error;
    llvm::raw_string_ostream stream(error);
    if (llvm::verifyFunction(*function, &stream))
    {
        function->eraseFromParent();
        return std::unexpected(std::move(error));
    }

    return function;
}

llvm::Value *ASTLowerer::lower_expr(Expr &expr)
{
    result = nullptr;
    expr.accept(*this);
    if (result == nullptr)
        diagnostics.error("No IR lowering is implemented for this expression yet");
    return result;
}

void ASTLowerer::lower_stmt(Stmt &stmt)
{
    stmt.accept(*this);
}