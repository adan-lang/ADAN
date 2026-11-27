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

llvm::Value *IRLowerer::lower_array_element_address(llvm::Value *array, llvm::Type *element_type, llvm::Value *index)
{
    if (const auto *descriptor_type = llvm::dyn_cast<llvm::StructType>(array->getType()); descriptor_type == nullptr || descriptor_type->getNumElements() != 2 || !descriptor_type->getElementType(0)->isPointerTy() || !descriptor_type->getElementType(1)->isIntegerTy(64))
        diagnostics.error("Array indexing requires a {ptr, i64} descriptor");
    if (!index->getType()->isIntegerTy(64))
        diagnostics.error("Array index did not lower to an i64 value");

    llvm::Value *data = builder.CreateExtractValue(array, {0});
    llvm::Value *length = builder.CreateExtractValue(array, {1});
    llvm::Value *one = builder.getInt64(1);
    llvm::Value *at_least_one = builder.CreateICmpSGE(index, one);
    llvm::Value *at_most_length = builder.CreateICmpSLE(index, length);
    llvm::Value *in_bounds = builder.CreateAnd(at_least_one, at_most_length);

    llvm::Function *function = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *failure_block = llvm::BasicBlock::Create(context, "array.bounds.fail", function);
    llvm::BasicBlock *success_block = llvm::BasicBlock::Create(context, "array.bounds.ok", function);
    builder.CreateCondBr(in_bounds, success_block, failure_block);

    builder.SetInsertPoint(failure_block);
    llvm::Function *trap = llvm::Intrinsic::getOrInsertDeclaration(_module.get(), llvm::Intrinsic::trap);
    builder.CreateCall(trap);
    builder.CreateUnreachable();

    builder.SetInsertPoint(success_block);
    llvm::Value *zero_based_index = builder.CreateSub(index, one);
    return builder.CreateGEP(element_type, data, zero_based_index);
}

void IRLowerer::visit(ArrayIndex &node)
{
    llvm::Value *array_value = lower_expr(*node.array);
    const auto element_type = array_element_types.find(array_value);
    if (element_type == array_element_types.end())
        diagnostics.error("Array element type is unavailable during IR lowering");

    llvm::Value *index = lower_expr(*node.index);
    llvm::Value *element = lower_array_element_address(array_value, element_type->second, index);
    result = builder.CreateLoad(element_type->second, element);
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
    if (const auto return_element_type = fn_array_return_types.find(node.callee); return_element_type != fn_array_return_types.end())
        array_element_types.emplace(result, return_element_type->second);
}

void IRLowerer::visit(Identifier &node)
{
    const auto value = local_values.find(node.name);
    if (value == local_values.end())
        diagnostics.error("No lowered value exists for identifier");

    const auto instance = llvm::dyn_cast<llvm::AllocaInst>(value->second);
    if (instance == nullptr)
        diagnostics.error(std::format("Identifier: \"{}\" does not refer to local storage", node.name));

    llvm::LoadInst *loaded = builder.CreateLoad(instance->getAllocatedType(), value->second);
    result = loaded;
    if (const auto element_type = array_element_types.find(value->second); element_type != array_element_types.end())
        array_element_types.emplace(result, element_type->second);
}

//
// @todo Power: decide what integer powers do for negative exponents and overflow.
//       Then keep int^int as Int and any power with a Float operand as Float, as registered.
//       Lower integer powers with integer operations and floating powers with a pow operation.
//       Add checks for integer, float, mixed, and edge-case exponents.
//
// @todo Numeric compound assignment: use the matching arithmetic operation for each operator.
//       Read the current value, apply the same Int/Float conversions as normal arithmetic, then store it.
//       Support identifiers and indexed elements; evaluate the target and index only once.
//       Return the value that was stored, and test each operator with Int and Float values.
//
void IRLowerer::visit(BinaryExpr &node)
{
    llvm::Value *rhs = lower_expr(*node.rhs);
    if (node.op == BinaryOp::Equal)
    {
        if (const auto lhs = dynamic_cast<Identifier *>(node.lhs.get()); lhs != nullptr)
        {
            const auto value = local_values.find(lhs->name);
            if (value == std::end(local_values))
                diagnostics.error(std::format("No lowered storage exists for assignment target: \"{}\"", lhs->name));

            llvm::AllocaInst *stored = value->second;
            builder.CreateStore(coerce_numeric_value(rhs, stored->getAllocatedType()), stored);
        }
        else if (const auto array_index = dynamic_cast<ArrayIndex *>(node.lhs.get()); array_index != nullptr)
        {
            llvm::Value *array_value = lower_expr(*array_index->array);
            const auto element_type = array_element_types.find(array_value);
            if (element_type == array_element_types.end())
                diagnostics.error("Array element type is unavailable during IR lowering");

            llvm::Value *index = lower_expr(*array_index->index);
            llvm::Value *element = lower_array_element_address(array_value, element_type->second, index);
            builder.CreateStore(rhs, element);
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
    case BinaryOp::Equal: // Impossible case
        break;
    }

    if (instruction == nullptr)
        diagnostics.error(std::format("No IR lowering exists for the binary operator: {}", op_to_string(node.op)));

    result = instruction;
}

void IRLowerer::visit(UnaryExpr &node)
{
    llvm::Value *operand = lower_expr(*node.operand);

    const bool is_float = operand->getType()->isDoubleTy();

    llvm::Value *instruction = nullptr;
    switch (node.op)
    {
    case UnaryOp::Minus:
        if (!operand->getType()->isIntegerTy() && !operand->getType()->isDoubleTy())
            diagnostics.error("Attempted to flip the sign of a non-number value.");
        if (is_float)
            instruction = builder.CreateFNeg(operand);
        else
            instruction = builder.CreateNeg(operand);
        break;
    case UnaryOp::Not:
        if (!operand->getType()->isIntegerTy(1))
            diagnostics.error("Attempted to inverse the truthiness of a non-boolean value.");
        instruction = builder.CreateNot(operand);
        break;
    case UnaryOp::Hashtag:
        // Support arrays. The intent is to get the length of an array (array literal or not).
        break;
    case UnaryOp::Increment:
        // Support both integers and doubles, adding 1 or 1.0 depending on the type.
        break;
    case UnaryOp::Decrement:
        // Support both integers and doubles, subtracting 1 or 1.0 depending on the type.
        break;
    }

    if (instruction == nullptr)
        diagnostics.error(std::format("No IR lowering exists for the unary operator: {}", op_to_string(node.op)));

    result = instruction;
}