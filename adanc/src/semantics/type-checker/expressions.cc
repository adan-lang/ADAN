#include <format>

#include "type-checker.hh"

void TypeChecker::visit(BinaryExpr &node)
{
    TypePtr lhs = infer(*node.lhs);
    TypePtr rhs = infer(*node.rhs);

    switch (node.op)
    {
    case BinaryOp::Plus:
    case BinaryOp::Minus:
    case BinaryOp::Multiply:
    case BinaryOp::Divide:
    case BinaryOp::Modulo:
    case BinaryOp::GreaterThan:
    case BinaryOp::GreaterEqual:
    case BinaryOp::LessThan:
    case BinaryOp::LessEqual:
    case BinaryOp::Equal:
    case BinaryOp::EqualEqual:
    case BinaryOp::PlusAssign:
    case BinaryOp::MinusAssign:
    case BinaryOp::MultiplyAssign:
    case BinaryOp::DivideAssign:
    case BinaryOp::ModuloAssign:
        return;
    }
}

void TypeChecker::visit(Identifier &node)
{
    Symbol *symbol = symbol_table.lookup(node.name);
    if (!symbol)
        throw std::runtime_error(std::format("Unknown identifier: {}", node.name));

    last_inferred_type = symbol->type;
}