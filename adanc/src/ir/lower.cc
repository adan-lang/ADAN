#include "lower.hh"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>

#include <expected>
#include <format>
#include <string>
#include <string_view>
#include <vector>

#include "ast.hh"
#include "type-helpers.hh"
#include "types.hh"

IRLowerer::IRLowerer(llvm::LLVMContext &context, std::string_view module_name, Diagnostics &diagnostics) : context(context), builder(context), _module(std::make_unique<llvm::Module>(module_name, context)), diagnostics(diagnostics)
{
    auto *print_type = llvm::FunctionType::get(builder.getVoidTy(), {builder.getPtrTy()}, false);
    llvm::Function::Create(print_type, llvm::Function::ExternalLinkage, "print", *_module);
}

llvm::Type *IRLowerer::lower_type(const TypePtr &type, bool allow_void)
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
    case TypeKind::Array:
        return llvm::StructType::get(context, {builder.getPtrTy(), builder.getInt64Ty()});
    default:
        break;
    }

    diagnostics.error(std::format("IR lowering does not support the {} type yet", type_to_string(resolved)));
}

llvm::Value *IRLowerer::coerce_numeric_value(llvm::Value *value, llvm::Type *destination_type)
{
    if (destination_type->isDoubleTy() && value->getType()->isIntegerTy())
        return builder.CreateSIToFP(value, destination_type);
    return value;
}

void IRLowerer::declare_function(std::string_view name, const TypePtr &function_type)
{
    const TypePtr resolved_function_type = prune(function_type);
    if (resolved_function_type->kind != TypeKind::Function)
        diagnostics.error("Cannot declare an LLVM function from a non-function symbol");

    if (_module->getFunction(name) != nullptr)
        diagnostics.error("An LLVM function with this name is already declared");

    llvm::Type *return_type = lower_type(resolved_function_type->result, true);
    std::vector<llvm::Type *> parameter_types;
    std::vector<llvm::Type *> array_parameter_element_types;
    parameter_types.reserve(resolved_function_type->params.size());
    for (const TypePtr &parameter : resolved_function_type->params)
    {
        parameter_types.push_back(lower_type(parameter));
        const TypePtr resolved_parameter = prune(parameter);
        array_parameter_element_types.push_back(resolved_parameter->kind == TypeKind::Array ? lower_type(resolved_parameter->element) : nullptr);
    }

    if (prune(resolved_function_type->result)->kind == TypeKind::Array)
        fn_array_return_types.emplace(std::string{name}, lower_type(prune(resolved_function_type->result)->element));
    fn_array_arg_types.emplace(std::string{name}, std::move(array_parameter_element_types));

    auto *type = llvm::FunctionType::get(return_type, parameter_types, false);
    llvm::Function::Create(type, llvm::Function::ExternalLinkage, name, *_module);
}

std::expected<llvm::Function *, std::string> IRLowerer::lower_function(const FuncDecl &declaration)
{
    llvm::Function *function = _module->getFunction(declaration.name);
    if (function == nullptr)
        diagnostics.error("Function was not declared before its body was lowered");

    if (function->arg_size() != declaration.params.size())
        diagnostics.error("Function parameter count does not match its LLVM declaration");

    local_values.clear();
    llvm::BasicBlock *entry_block = llvm::BasicBlock::Create(context, "entry", function);
    builder.SetInsertPoint(entry_block);

    llvm::IRBuilder<> entry_builder(context);
    entry_builder.SetInsertPoint(entry_block, entry_block->begin());

    std::size_t parameter_index{};
    const auto array_parameter_types = fn_array_arg_types.find(declaration.name);
    for (llvm::Argument &argument : function->args())
    {
        const std::string &parameter_name = declaration.params[parameter_index++];
        argument.setName(parameter_name);

        llvm::AllocaInst *storage = entry_builder.CreateAlloca(argument.getType(), nullptr, parameter_name);
        builder.CreateStore(&argument, storage);
        local_values.emplace(parameter_name, storage);
        if (array_parameter_types != fn_array_arg_types.end() && array_parameter_types->second[parameter_index - 1] != nullptr)
            array_element_types.emplace(storage, array_parameter_types->second[parameter_index - 1]);
    }

    for (const auto &stmt : declaration.body.stmts)
    {
        if (builder.GetInsertBlock()->getTerminatorOrNull() != nullptr)
            break;
        lower_stmt(*stmt);
    }

    std::string error;
    llvm::raw_string_ostream stream(error);
    if (llvm::verifyFunction(*function, &stream))
    {
        function->eraseFromParent();
        return std::unexpected(std::move(error));
    }

    return function;
}

void IRLowerer::lower_program(Block &program)
{
    local_values.clear();
    llvm::FunctionType *function_type = llvm::FunctionType::get(llvm::Type::getInt32Ty(context), false);
    llvm::Function *function = llvm::Function::Create(function_type, llvm::Function::ExternalLinkage, "main", *_module);

    builder.SetInsertPoint(llvm::BasicBlock::Create(context, "main", function));

    for (auto &stmt : program.stmts)
        if (dynamic_cast<FuncDecl *>(stmt.get()) == nullptr)
            lower_stmt(*stmt);

    if (builder.GetInsertBlock()->getTerminatorOrNull() == nullptr)
        builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(context), 0));

    std::string error;
    llvm::raw_string_ostream stream(error);
    if (llvm::verifyModule(*_module, &stream))
        diagnostics.error(std::format("LLVM module verification failed:\n{}", stream.str()));
}

llvm::Value *IRLowerer::lower_expr(Expr &expr)
{
    result = nullptr;
    expr.accept(*this);
    if (result == nullptr)
        diagnostics.error("No IR lowering is implemented for this expression yet");
    return result;
}

void IRLowerer::lower_stmt(Stmt &stmt)
{
    stmt.accept(*this);
}