#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>

#include "ast.hh"
#include "lower.hh"

void IRLowerer::visit(ReturnStmt &node)
{
    llvm::Value *value = lower_expr(*node.value);
    if (value->getType()->isVoidTy())
        builder.CreateRetVoid();
    else
        builder.CreateRet(value);
}

void IRLowerer::visit(ExprStmt &node)
{
    lower_expr(*node.expr);
}

void IRLowerer::visit(IfStmt &node)
{
    if (const auto *literal = dynamic_cast<const BoolLiteral *>(node.condition.get()))
    {
        if (!literal->value)
            return;

        for (const auto &stmt : node.body.stmts)
        {
            if (builder.GetInsertBlock()->getTerminatorOrNull() != nullptr)
                break;
            lower_stmt(*stmt);
        }
        return;
    }

    llvm::Value *condition = lower_expr(*node.condition);
    if (!condition->getType()->isIntegerTy(1))
        diagnostics.error("If statement's condition did not lower to an LLVM i1 (boolean) value.");

    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *then_block = llvm::BasicBlock::Create(context, "if.then", function);
    llvm::BasicBlock *continuation_block = llvm::BasicBlock::Create(context, "if.cont", function);

    builder.CreateCondBr(condition, then_block, continuation_block);
    builder.SetInsertPoint(then_block);

    for (const auto &stmt : node.body.stmts)
    {
        if (builder.GetInsertBlock()->getTerminatorOrNull() != nullptr)
            break;
        lower_stmt(*stmt);
    }

    if (builder.GetInsertBlock()->getTerminatorOrNull() == nullptr)
        builder.CreateBr(continuation_block);

    builder.SetInsertPoint(continuation_block);
}

void IRLowerer::visit(LocalDecl &node)
{
    if (!node.init)
        diagnostics.error("Local declarations without an initializer are not supported in IR lowering");

    llvm::Value *value = lower_expr(*node.init);
    llvm::Type *ident_type = value->getType();
    if (ident_type == nullptr)
        diagnostics.error("Local initializer did not produce an LLVM type");

    llvm::IRBuilder<> temp_builder(context);
    
    llvm::BasicBlock &entry_block = builder.GetInsertBlock()->getParent()->getEntryBlock();
    temp_builder.SetInsertPoint(&entry_block, entry_block.begin());

    llvm::AllocaInst *storage = temp_builder.CreateAlloca(ident_type);
    builder.CreateStore(value, storage);

    local_values.insert({node.name, storage});
}