#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <stdexcept>

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
    for (auto &element : vec)
        if (element->getType() != element_type)
            diagnostics.error("Array literal elements must have the same lowered LLVM type");

    llvm::ArrayType *array_type = llvm::ArrayType::get(element_type, node.elements.size());
    llvm::Value *inserted = llvm::UndefValue::get(array_type);
    for (std::size_t i{}; i < vec.size(); ++i)
    {
        auto unsigned_vec = llvm::SmallVector<unsigned, 1>();
        unsigned_vec.emplace_back(static_cast<unsigned>(i));
        inserted = builder.CreateInsertValue(inserted, vec[i], unsigned_vec);
    }

    result = inserted;
}