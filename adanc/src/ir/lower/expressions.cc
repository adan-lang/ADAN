#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/Casting.h>

#include <format>
#include <iterator>
#include <vector>

#include "ast-helpers.hh"
#include "ast.hh"
#include "lower.hh"

llvm::Value *IRLowerer::lower_array_element_address(llvm::AllocaInst *array, llvm::Value *index)
{
    auto *array_type = llvm::dyn_cast<llvm::ArrayType>(array->getAllocatedType());
    if (array_type == nullptr)
        diagnostics.error("Indexed storage is not an LLVM array");
    if (!index->getType()->isIntegerTy(64))
        diagnostics.error("Array index did not lower to an i64 value");

    llvm::Value *zero = builder.getInt64(0);
    llvm::Value *non_negative = builder.CreateICmpSGE(index, zero);
    llvm::Value *below_length = builder.CreateICmpSLT(index, builder.getInt64(array_type->getNumElements()));
    llvm::Value *in_bounds = builder.CreateAnd(non_negative, below_length);

    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *failure_block = llvm::BasicBlock::Create(context, "array.bounds.fail", function);
    llvm::BasicBlock *success_block = llvm::BasicBlock::Create(context, "array.bounds.ok", function);
    builder.CreateCondBr(in_bounds, success_block, failure_block);

    builder.SetInsertPoint(failure_block);
    llvm::Function *trap = llvm::Intrinsic::getOrInsertDeclaration(_module.get(), llvm::Intrinsic::trap);
    builder.CreateCall(trap);
    builder.CreateUnreachable();

    builder.SetInsertPoint(success_block);
    llvm::Value *indices[] = {zero, index};
    return builder.CreateGEP(array_type, array, indices);
}

void IRLowerer::visit(ArrayIndex &node)
{
    llvm::AllocaInst *array_storage{};
    if (auto ident = dynamic_cast<Identifier *>(node.array.get()); ident != nullptr)
    {
        auto array = local_values.find(ident->name);
        if (array == std::end(local_values))
            diagnostics.error(std::format("No lowered storage exists for indexed array: \"{}\"", ident->name));

        array_storage = array->second;
    }
    else
    {
        llvm::Value *array_value = lower_expr(*node.array);
        auto *array_type = llvm::dyn_cast<llvm::ArrayType>(array_value->getType());
        if (array_type == nullptr)
            diagnostics.error("Array indexing requires an array value");

        llvm::BasicBlock &entry_block = builder.GetInsertBlock()->getParent()->getEntryBlock();
        llvm::IRBuilder<> entry_builder(context);
        entry_builder.SetInsertPoint(&entry_block, entry_block.begin());
        array_storage = entry_builder.CreateAlloca(array_type);
        builder.CreateStore(array_value, array_storage);
    }

    auto *array_type = llvm::dyn_cast<llvm::ArrayType>(array_storage->getAllocatedType());
    if (array_type == nullptr)
        diagnostics.error("Indexed storage is not an LLVM array");

    llvm::Value *index = lower_expr(*node.index);
    llvm::Value *element = lower_array_element_address(array_storage, index);
    result = builder.CreateLoad(array_type->getElementType(), element);
}

void IRLowerer::visit(CallExpr &node)
{
    llvm::Function *function = _module->getFunction(node.callee);
    if (function == nullptr)
        diagnostics.error(std::format("No LLVM declaration exists for function: \"{}\"", node.callee));

    if (node.args.size() != function->arg_size())
        diagnostics.error(std::format("Call argument count does not match the function declaration for {}", node.callee));

    std::vector<llvm::Value *> args;
    args.reserve(node.args.size());
    for (std::size_t i{}; i < node.args.size(); ++i)
    {
        llvm::Value *lowered = lower_expr(*node.args[i]);
        if (lowered->getType()->isVoidTy())
            diagnostics.error("A void function call cannot be used as an argument");
        if (lowered->getType() != function->getFunctionType()->getParamType(static_cast<unsigned>(i)))
            diagnostics.error(std::format("Call argument type does not match the function declaration for {}", node.callee));

        args.emplace_back(lowered);
    }

    result = builder.CreateCall(function, args);
}

void IRLowerer::visit(Identifier &node)
{
    const auto value = local_values.find(node.name);
    if (value == local_values.end())
        diagnostics.error("No lowered value exists for identifier");

    auto instance = llvm::dyn_cast<llvm::AllocaInst>(value->second);
    if (instance == nullptr)
        diagnostics.error(std::format("Identifier: \"{}\" does not refer to local storage", node.name));

    llvm::LoadInst *loaded = builder.CreateLoad(instance->getAllocatedType(), value->second);
    result = loaded;
}

void IRLowerer::visit(BinaryExpr &node)
{
    llvm::Value *rhs = lower_expr(*node.rhs);
    if (node.op == BinaryOp::Equal)
    {
        if (auto lhs = dynamic_cast<Identifier *>(node.lhs.get()); lhs != nullptr)
        {
            auto value = local_values.find(lhs->name);
            if (value == std::end(local_values))
                diagnostics.error(std::format("No lowered storage exists for assignment target: \"{}\"", lhs->name));

            llvm::AllocaInst *stored = value->second;
            builder.CreateStore(rhs, stored);
        }
        else if (auto lhs = dynamic_cast<ArrayIndex *>(node.lhs.get()); lhs != nullptr)
        {
            if (auto ident = dynamic_cast<Identifier *>(lhs->array.get()); ident != nullptr)
            {
                auto array = local_values.find(ident->name);
                if (array == std::end(local_values))
                    diagnostics.error(std::format("No lowered storage exists for assignment target: \"{}\"", ident->name));

                llvm::Value *index = lower_expr(*lhs->index);
                llvm::Value *element = lower_array_element_address(array->second, index);
                builder.CreateStore(rhs, element);
            }
        }

        result = rhs;
        return;
    }

    llvm::Value *lhs = lower_expr(*node.lhs);
    const bool is_float = lhs->getType()->isDoubleTy() || rhs->getType()->isDoubleTy();
    if (is_float)
    {
        if (lhs->getType()->isIntegerTy())
            lhs = builder.CreateSIToFP(lhs, llvm::Type::getDoubleTy(context));
        if (rhs->getType()->isIntegerTy())
            rhs = builder.CreateSIToFP(rhs, llvm::Type::getDoubleTy(context));
    }

    llvm::Value *instruction = nullptr;
    switch (node.op)
    {
    case BinaryOp::Plus:
        // @todo Eventually add support for string concatenation (str + str).
        if (is_float)
            instruction = builder.CreateFAdd(lhs, rhs);
        else if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
            instruction = builder.CreateAdd(lhs, rhs);
        break;
    case BinaryOp::Minus:
        if (is_float)
            instruction = builder.CreateFSub(lhs, rhs);
        else if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
            instruction = builder.CreateSub(lhs, rhs);
        break;
    case BinaryOp::Multiply:
        if (is_float)
            instruction = builder.CreateFMul(lhs, rhs);
        else if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
            instruction = builder.CreateMul(lhs, rhs);
        break;
    case BinaryOp::Divide:
        if (is_float)
            instruction = builder.CreateFDiv(lhs, rhs);
        else if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
            instruction = builder.CreateSDiv(lhs, rhs);
        break;
    case BinaryOp::Power:
        break;
    case BinaryOp::Modulo:
        if (is_float)
            instruction = builder.CreateFRem(lhs, rhs);
        else if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
            instruction = builder.CreateSRem(lhs, rhs);
        break;
    case BinaryOp::GreaterThan:
        if (is_float)
            instruction = builder.CreateFCmpUGT(lhs, rhs);
        else if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
            instruction = builder.CreateICmpSGT(lhs, rhs);
        break;
    case BinaryOp::GreaterEqual:
        if (is_float)
            instruction = builder.CreateFCmpUGE(lhs, rhs);
        else if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
            instruction = builder.CreateICmpSGE(lhs, rhs);
        break;
    case BinaryOp::LessThan:
        if (is_float)
            instruction = builder.CreateFCmpULT(lhs, rhs);
        else if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
            instruction = builder.CreateICmpSLT(lhs, rhs);
        break;
    case BinaryOp::LessEqual:
        if (is_float)
            instruction = builder.CreateFCmpULE(lhs, rhs);
        else if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
            instruction = builder.CreateICmpSLE(lhs, rhs);
        break;
    case BinaryOp::EqualEqual:
        // @todo Eventually add support for string concatenation (str + str).
        // @todo Eventually add support for booleans.
        // @todo Eventually add support for arrays.
        if (is_float)
            instruction = builder.CreateFCmpUEQ(lhs, rhs);
        else if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
            instruction = builder.CreateICmpEQ(lhs, rhs);
        break;
    case BinaryOp::NotEqualEqual:
        if (is_float)
            instruction = builder.CreateFCmpUNE(lhs, rhs);
        else if (lhs->getType()->isIntegerTy() && rhs->getType()->isIntegerTy())
            instruction = builder.CreateICmpNE(lhs, rhs);
        break;
    case BinaryOp::PlusAssign:
        break;
    case BinaryOp::MinusAssign:
        break;
    case BinaryOp::MultiplyAssign:
        break;
    case BinaryOp::DivideAssign:
        break;
    case BinaryOp::PowerAssign:
        break;
    case BinaryOp::ModuloAssign:
        break;
    }

    if (instruction == nullptr)
        diagnostics.error(std::format("No IR lowering exists for binary operator: {}", op_to_string(node.op)));

    result = instruction;
}