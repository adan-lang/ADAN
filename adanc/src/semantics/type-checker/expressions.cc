#include <format>

#include "type-checker.hh"

void TypeChecker::visit(BinaryExpr &node)
{
    TypePtr lhs = infer(*node.lhs);
    TypePtr rhs = infer(*node.rhs);
}

void TypeChecker::visit(Identifier &node)
{
    Symbol *symbol = symbol_table.lookup(node.name);
    if (!symbol)
        throw std::runtime_error(std::format("Unknown identifier: {}", node.name));

    last_inferred_type = symbol->type;
}