#include <format>

#include "type-checker.hh"
#include "operator-table.hh"

void TypeChecker::visit(BinaryExpr &node)
{
    TypePtr lhs = infer(*node.lhs);
    TypePtr rhs = infer(*node.rhs);

    // @todo do when i wake up
}

void TypeChecker::visit(UnaryExpr &node)
{
    TypePtr operand = infer(*node.operand);

    // @todo do later
}

void TypeChecker::visit(Identifier &node)
{
    Symbol *symbol = symbol_table.lookup(node.name);
    if (!symbol)
        throw std::runtime_error(std::format("Unknown identifier: {}", node.name));

    last_inferred_type = symbol->type;
}