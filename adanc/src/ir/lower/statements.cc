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

/**
 *
 * Breakdown of this function:
 *  - Create a new `condition`, which is an `llvm::Value *`.
 *
 *  - Create `function`, which is the LLVM function that owns the block.
 *
 *  - Create a `then` block, the parent of which is `function`.
 *  - - The block is this portion: `if <cond> { ... (Block) }`
 *
 *  - Create a `continuation` block, which is what is after the if statement, not the else clause.
 *
 *  - Update the "Insert Point", which is updating what block we're in to be the `then` block.
 *
 *  - Iterate over each statement in the if statement's statement list.
 *
 *  - Check if there is no terminator (like `return`), if not, create a branch and set its destination to the `continuation` block.
 *
 *  - Update the "Insert Point" to be the `continuation` block.
 */
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