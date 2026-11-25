#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Type.h>
#include <llvm/Support/Casting.h>

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

void IRLowerer::visit(NumericForStmt &node)
{
    llvm::Value *value = lower_expr(*node.start);
    llvm::Type *ident_type = value->getType();
    if (ident_type == nullptr)
        diagnostics.error("Local initializer did not produce an LLVM type");

    llvm::IRBuilder<> temp_builder(context);

    llvm::BasicBlock &entry_block = builder.GetInsertBlock()->getParent()->getEntryBlock();
    temp_builder.SetInsertPoint(&entry_block, entry_block.begin());

    llvm::AllocaInst *storage = temp_builder.CreateAlloca(ident_type);
    builder.CreateStore(value, storage);

    llvm::Value *stop = lower_expr(*node.stop);
    llvm::Value *step = lower_expr(*node.step);

    auto to_find = local_values.find(node.name);
    llvm::AllocaInst *found = nullptr;
    if (to_find != local_values.end())
        found = to_find->second;
    bool was_found = to_find != local_values.end();

    local_values.insert_or_assign(node.name, storage);

    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *condition_block = llvm::BasicBlock::Create(context, "for.cond", function);
    llvm::BasicBlock *body_block = llvm::BasicBlock::Create(context, "for.body", function);
    llvm::BasicBlock *increment_block = llvm::BasicBlock::Create(context, "for.step", function);
    llvm::BasicBlock *exit_block = llvm::BasicBlock::Create(context, "for.exit", function);

    builder.CreateBr(condition_block);

    builder.SetInsertPoint(condition_block);
    llvm::Value *current = builder.CreateLoad(ident_type, storage);
    llvm::Value *positive_step = builder.CreateICmpSGT(step, builder.getInt64(0));
    llvm::Value *negative_step = builder.CreateICmpSLT(step, builder.getInt64(0));
    llvm::Value *within_positive_range = builder.CreateICmpSLE(current, stop);
    llvm::Value *within_negative_range = builder.CreateICmpSGE(current, stop);
    llvm::Value *continue_positive = builder.CreateAnd(positive_step, within_positive_range);
    llvm::Value *continue_negative = builder.CreateAnd(negative_step, within_negative_range);
    llvm::Value *continue_loop = builder.CreateOr(continue_positive, continue_negative);
    builder.CreateCondBr(continue_loop, body_block, exit_block);

    builder.SetInsertPoint(body_block);

    for (const auto &stmt : node.body.stmts)
    {
        if (builder.GetInsertBlock()->getTerminatorOrNull() != nullptr)
            break;
        lower_stmt(*stmt);
    }

    if (builder.GetInsertBlock()->getTerminatorOrNull() == nullptr)
        builder.CreateBr(increment_block);

    builder.SetInsertPoint(increment_block);
    current = builder.CreateLoad(ident_type, storage);
    llvm::Value *next = builder.CreateAdd(current, step);
    builder.CreateStore(next, storage);
    builder.CreateBr(condition_block);

    builder.SetInsertPoint(exit_block);
    if (was_found)
        local_values.insert_or_assign(node.name, found);
    else
        local_values.erase(node.name);
}

void IRLowerer::visit(ForInStmt &node)
{
    llvm::Value *array = lower_expr(*node.array);
    const auto element_type = array_element_types.find(array);
    if (element_type == array_element_types.end())
        diagnostics.error("Array element type is unavailable during IR lowering");

    auto *descriptor_type = llvm::dyn_cast<llvm::StructType>(array->getType());
    if (descriptor_type == nullptr || descriptor_type->getNumElements() != 2 || !descriptor_type->getElementType(0)->isPointerTy() || !descriptor_type->getElementType(1)->isIntegerTy(64))
        diagnostics.error("For-in iteration requires a {ptr, i64} array descriptor");
    llvm::Value *length = builder.CreateExtractValue(array, {1});

    llvm::IRBuilder<> temp_builder(context);

    llvm::BasicBlock &entry_block = builder.GetInsertBlock()->getParent()->getEntryBlock();
    temp_builder.SetInsertPoint(&entry_block, entry_block.begin());

    llvm::AllocaInst *index_storage = temp_builder.CreateAlloca(builder.getInt64Ty());
    llvm::AllocaInst *element_storage = temp_builder.CreateAlloca(element_type->second);
    builder.CreateStore(builder.getInt64(0), index_storage);

    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *condition_block = llvm::BasicBlock::Create(context, "for-in.cond", function);
    llvm::BasicBlock *body_block = llvm::BasicBlock::Create(context, "for-in.body", function);
    llvm::BasicBlock *increment_block = llvm::BasicBlock::Create(context, "for-in.step", function);
    llvm::BasicBlock *exit_block = llvm::BasicBlock::Create(context, "for-in.exit", function);

    builder.CreateBr(condition_block);

    builder.SetInsertPoint(condition_block);
    llvm::Value *current_index = builder.CreateLoad(builder.getInt64Ty(), index_storage);
    llvm::Value *has_next = builder.CreateICmpSLT(current_index, length);
    builder.CreateCondBr(has_next, body_block, exit_block);

    auto to_find = local_values.find(node.key);
    llvm::AllocaInst *found = nullptr;
    if (to_find != local_values.end())
        found = to_find->second;
    const bool was_found = to_find != local_values.end();
    local_values.insert_or_assign(node.key, element_storage);

    builder.SetInsertPoint(body_block);
    current_index = builder.CreateLoad(builder.getInt64Ty(), index_storage);
    llvm::Value *one_based_index = builder.CreateAdd(current_index, builder.getInt64(1));
    llvm::Value *element_address = lower_array_element_address(array, element_type->second, one_based_index);
    llvm::Value *element_value = builder.CreateLoad(element_type->second, element_address);
    builder.CreateStore(element_value, element_storage);

    for (const auto &stmt : node.body.stmts)
    {
        if (builder.GetInsertBlock()->getTerminatorOrNull() != nullptr)
            break;
        lower_stmt(*stmt);
    }

    if (builder.GetInsertBlock()->getTerminatorOrNull() == nullptr)
        builder.CreateBr(increment_block);

    builder.SetInsertPoint(increment_block);
    current_index = builder.CreateLoad(builder.getInt64Ty(), index_storage);
    llvm::Value *next_index = builder.CreateAdd(current_index, builder.getInt64(1));
    builder.CreateStore(next_index, index_storage);
    builder.CreateBr(condition_block);

    builder.SetInsertPoint(exit_block);
    if (was_found)
        local_values.insert_or_assign(node.key, found);
    else
        local_values.erase(node.key);
}

void IRLowerer::visit(WhileStmt &node)
{
    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *condition_block = llvm::BasicBlock::Create(context, "while.condition", function);
    builder.CreateBr(condition_block);
    builder.SetInsertPoint(condition_block);

    llvm::Value *condition = lower_expr(*node.condition);
    if (!condition->getType()->isIntegerTy(1))
        diagnostics.error("While statement's condition did not lower to an LLVM i1 (boolean) value.");

    llvm::BasicBlock *body_block = llvm::BasicBlock::Create(context, "while.then", function);
    llvm::BasicBlock *exit_block = llvm::BasicBlock::Create(context, "while.cont", function);

    builder.CreateCondBr(condition, body_block, exit_block);
    builder.SetInsertPoint(body_block);

    for (const auto &stmt : node.body.stmts)
    {
        if (builder.GetInsertBlock()->getTerminatorOrNull() != nullptr)
            break;
        lower_stmt(*stmt);
    }

    if (builder.GetInsertBlock()->getTerminatorOrNull() == nullptr)
        builder.CreateBr(condition_block);

    builder.SetInsertPoint(exit_block);
}

void IRLowerer::visit(LocalDecl &node)
{
    if (!node.init)
        diagnostics.error("Local declarations without an initializer are not supported in IR lowering");

    llvm::Value *value = lower_expr(*node.init);
    llvm::Type *ident_type = node.inferred_type ? lower_type(node.inferred_type) : value->getType();
    if (ident_type == nullptr)
        diagnostics.error("Local initializer did not produce an LLVM type");
    value = coerce_numeric_value(value, ident_type);

    llvm::IRBuilder<> temp_builder(context);

    llvm::BasicBlock &entry_block = builder.GetInsertBlock()->getParent()->getEntryBlock();
    temp_builder.SetInsertPoint(&entry_block, entry_block.begin());

    llvm::AllocaInst *storage = temp_builder.CreateAlloca(ident_type);
    builder.CreateStore(value, storage);

    local_values.insert({node.name, storage});
    const auto element_type = array_element_types.find(value);
    if (element_type != array_element_types.end())
        array_element_types.emplace(storage, element_type->second);
}