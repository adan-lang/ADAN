#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/IRBuilder.h>

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