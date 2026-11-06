#include <format>
#include <stdexcept>

#include "ast.hh"
#include "type-checker.hh"
#include "operator-table.hh"
#include "ast-helpers.hh"
#include "type-helpers.hh"

void TypeChecker::visit(CallExpr &node)
{
    Symbol *callee = symbol_table.lookup(node.callee);
    if (!callee)
        throw std::runtime_error(std::format("Attempted to call an unknown function: {}", node.callee));

    // @todo Stuff
}

/**
 *
 * A binary expression (BinaryExpr) is just an expression that
 *  contains a binary operator (i.e., "+").
 *
 * "local x = 1 + 2" <-- "1 + 2" is the binary expression.
 */
void TypeChecker::visit(BinaryExpr &node)
{
    TypePtr lhs = infer(*node.lhs);
    TypePtr rhs = infer(*node.rhs);

    bool success{};
    for (const BinaryOperator &rule : operator_table.get_binary_rules(node.op))
    {
        if ((*lhs == *rule.lhs) && (*rhs == *rule.rhs))
        {
            last_inferred_type = rule.result;
            success = true;
            break;
        }
    }

    if (!success)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Type mismatch on the binary operator: {}, with the types: {} (left) and {} (right)",
                                 op_to_string(node.op),
                                 type_to_string(lhs), type_to_string(rhs)));
}

void TypeChecker::visit(Identifier &node)
{
    Symbol *symbol = symbol_table.lookup(node.name);
    if (!symbol)
        throw std::runtime_error(std::format("Unknown identifier: {}", node.name));

    last_inferred_type = symbol->type;
}

void TypeChecker::visit(UnaryExpr &node)
{
    TypePtr operand = infer(*node.operand);

    bool success{};
    for (const UnaryOperator &rule : operator_table.get_unary_rules(node.op))
    {
        if ((*operand == *rule.operand))
        {
            last_inferred_type = rule.result;
            success = true;
            break;
        }
    }

    if (!success)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Type mismatch on the unary operator: {}, with the type: {}",
                                 op_to_string(node.op),
                                 type_to_string(operand)));
}