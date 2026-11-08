#include <format>
#include <print>
#include <stdexcept>

#include "ast.hh"
#include "type-checker.hh"
#include "operator-table.hh"
#include "ast-helpers.hh"
#include "type-helpers.hh"
#include "types.hh"

void TypeChecker::visit(ArrayIndex &node)
{
    TypePtr array = infer(*node.array);
    TypePtr array_generic = make_array(make_var());
    unify(array, array_generic);

    TypePtr index = infer(*node.index);
    unify(index, make_int());

    last_inferred_type = array_generic->element;
}

void TypeChecker::visit(CallExpr &node)
{
    Symbol *callee = symbol_table.lookup(node.callee);
    if (!callee)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Attempted to call an unknown function: {}", node.callee));

    TypePtr callee_t = prune(callee->type);
    if (callee_t->kind != TypeKind::Function)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Attempted to call something that isn't a function"));

    if (node.args.size() != callee_t->params.size())
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Function: \"{}\" expects {} argument(s), but got {}",
                                 node.callee, callee_t->params.size(), node.args.size()));

    for (std::size_t i{}; i < callee_t->params.size(); ++i)
    {
        unify(infer(*node.args[i]), callee_t->params[i]);
    }

    last_inferred_type = callee_t->result;
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

    // std::println("{}", type_to_string(rhs));

    bool success{};
    if (node.op == BinaryOp::Equal)
    {
        if ((dynamic_cast<Identifier*>(node.lhs.get()) != nullptr) ||
            (dynamic_cast<ArrayIndex*>(node.lhs.get()) != nullptr))
        {
            unify(lhs, rhs);
            last_inferred_type = lhs;
            return;
        }

        throw std::runtime_error("The left side of an assignment must be a variable or array index");
    }

    for (const BinaryOperator &rule : operator_table.get_binary_rules(node.op))
    {
        if ((*lhs == *rule.lhs) && (*rhs == *rule.rhs))
        {
            last_inferred_type = rule.result;
            success = true;
            break;
        }
    }

    if (prune(rhs)->kind == TypeKind::Var)
    {
        std::println("Right-hand side is an unresolved variable");
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
    bool success{};
    
    TypePtr operand = infer(*node.operand);
    if (node.op == UnaryOp::Hashtag)
    {
        unify(operand, make_array(make_var()));
        last_inferred_type = make_int();
        return;
    }

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