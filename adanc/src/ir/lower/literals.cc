#include <llvm/ADT/ArrayRef.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <cstddef>

#include "ast.hh"
#include "lower.hh"

void IRLowerer::visit(IntLiteral &node)
{
    result = llvm::ConstantInt::getSigned(builder.getInt64Ty(), node.value);
}

void IRLowerer::visit(FloatLiteral &node)
{
    result = llvm::ConstantFP::get(builder.getDoubleTy(), node.value);
}

void IRLowerer::visit(BoolLiteral &node)
{
    result = llvm::ConstantInt::get(builder.getInt1Ty(), node.value);
}

void IRLowerer::visit(StringLiteral &node)
{
    result = builder.CreateGlobalString(node.value);
}

void IRLowerer::visit(ArrayLiteral &node)
{
    std::vector<llvm::Value *> vec;
    for (auto &element : node.elements)
        vec.emplace_back(lower_expr(*element));

    if (node.elements.empty())
        diagnostics.error("Cannot lower an empty array literal because its element type cannot be inferred");

    llvm::Type *element_type = vec.at(0)->getType();
    for (const auto &element : vec)
        if (element->getType() != element_type)
            diagnostics.error("Array literal elements must have the same lowered LLVM type");

    llvm::IRBuilder<> temp_builder(context);

    llvm::BasicBlock &entry_block = builder.GetInsertBlock()->getParent()->getEntryBlock();
    temp_builder.SetInsertPoint(&entry_block, entry_block.begin());

    llvm::AllocaInst *array_storage = temp_builder.CreateAlloca(element_type, temp_builder.getInt64(vec.size()));
    llvm::Type *descriptor = llvm::StructType::get(context, {builder.getPtrTy(), builder.getInt64Ty()});
    llvm::Value *value = llvm::UndefValue::get(descriptor);

    for (std::size_t i{}; i < vec.size(); ++i)
    {
        llvm::Value *constructed = builder.CreateGEP(element_type, array_storage, temp_builder.getInt64(i));
        builder.CreateStore(vec[i], constructed);
    }

    value = builder.CreateInsertValue(value, array_storage, {0});
    value = builder.CreateInsertValue(value, builder.getInt64(vec.size()), {1});

    result = value;
    array_element_types.emplace(result, element_type);
}