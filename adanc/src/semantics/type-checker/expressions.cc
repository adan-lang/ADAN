#include <format>
#include <vector>
#include <print>

#include "type-checker.hh"
#include "operator-table.hh"

/**
 *
 * A binary expression (BinaryExpr) is just an expression that
 *  contains a binary operator (i.e., "+").
 *
 * "local x = 1 + 2" <-- "1 + 2" is the binary expression.
 *
 * Goal:
 * - Infer both the left and right-side operands. ✅
 * - Ensure both sides are compatible with one another. ❌
 */
void TypeChecker::visit(BinaryExpr &node)
{
    TypePtr lhs = infer(*node.lhs);
    TypePtr rhs = infer(*node.rhs);

    // for (std::size_t i{}; i < operator_table.get_binary_rules(node.op).size(); ++i)
    for (const auto &rule : operator_table.get_binary_rules(node.op))
    {
        std::println("{}", rule);
    }
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