#include <stdexcept>
#include <string>
#include <format>
#include <vector>

#include "ast-helpers.hh"
#include "type-checker.hh"
#include "type-helpers.hh"

namespace
{
    std::string describe_expression(const Expr &expr)
    {
        if (dynamic_cast<const IntLiteral *>(&expr))
            return "IntLiteral";
        if (dynamic_cast<const FloatLiteral *>(&expr))
            return "FloatLiteral";
        if (dynamic_cast<const BoolLiteral *>(&expr))
            return "BoolLiteral";
        if (dynamic_cast<const StringLiteral *>(&expr))
            return "StringLiteral";
        if (dynamic_cast<const InterpolatedString *>(&expr))
            return "InterpolatedString";
        if (const auto *identifier = dynamic_cast<const Identifier *>(&expr))
            return "Identifier '" + identifier->name + "'";
        if (dynamic_cast<const ArrayLiteral *>(&expr))
            return "ArrayLiteral";
        if (dynamic_cast<const ArrayIndex *>(&expr))
            return "ArrayIndex";
        if (const auto *call = dynamic_cast<const CallExpr *>(&expr))
            return "CallExpr '" + call->callee + "'";
        if (dynamic_cast<const BinaryExpr *>(&expr))
            return "BinaryExpr";
        if (dynamic_cast<const UnaryExpr *>(&expr))
            return "UnaryExpr";

        return "unknown expression";
    }
}

TypePtr TypeChecker::infer(Expr &expr)
{
    last_inferred_type = nullptr;
    expr.accept(*this);

    if (!last_inferred_type)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Couldn't infer a type for {}. Is the corresponding TypeChecker::visit override implemented?",
                                             describe_expression(expr)));

    return last_inferred_type;
}

void TypeChecker::check(Stmt &stmt)
{
    stmt.accept(*this);
}

void TypeChecker::check(Block &block)
{
    for (const auto &stmt : block.stmts)
        check(*stmt);
}

void TypeChecker::finalize()
{
    resolve_pending_operators();
}

void TypeChecker::resolve_pending_operators()
{
    std::vector<bool> binary_resolved(pending_binary_operators.size());
    std::vector<bool> unary_resolved(pending_unary_operators.size());

    bool made_progress{};
    do
    {
        made_progress = false;

        for (std::size_t i{}; i < pending_binary_operators.size(); ++i)
        {
            if (binary_resolved[i])
                continue;

            const PendingBinaryOperator &pending = pending_binary_operators[i];
            std::vector<BinaryOperator> matches;
            for (const BinaryOperator &rule : operator_table.get_binary_rules(pending.op))
            {
                if (type_can_match(pending.lhs, rule.lhs) &&
                    type_can_match(pending.rhs, rule.rhs) &&
                    type_can_match(pending.result, rule.result))
                    matches.emplace_back(rule);
            }

            if (matches.empty())
                throw std::runtime_error(std::format("Type mismatch on the binary operator: {}, with the types: {} (left) and {} (right)",
                                         op_to_string(pending.op),
                                         type_to_string(pending.lhs), type_to_string(pending.rhs)));
            if (matches.size() != 1)
                continue;

            const BinaryOperator &rule = matches.front();
            unify(pending.lhs, rule.lhs);
            unify(pending.rhs, rule.rhs);
            unify(pending.result, rule.result);

            if (!pending.target_widenable && !(*pending.lhs == *rule.result))
                throw std::runtime_error("This compound assignment cannot widen its target type");

            binary_resolved[i] = true;
            made_progress = true;
        }

        for (std::size_t i{}; i < pending_unary_operators.size(); ++i)
        {
            if (unary_resolved[i])
                continue;

            const PendingUnaryOperator &pending = pending_unary_operators[i];
            std::vector<UnaryOperator> matches;
            for (const UnaryOperator &rule : operator_table.get_unary_rules(pending.op))
            {
                if (type_can_match(pending.operand, rule.operand) &&
                    type_can_match(pending.result, rule.result))
                    matches.emplace_back(rule);
            }

            if (matches.empty())
                throw std::runtime_error(std::format("Type mismatch on the unary operator: {}, with the type: {}",
                                         op_to_string(pending.op), type_to_string(pending.operand)));
            if (matches.size() != 1)
                continue;

            const UnaryOperator &rule = matches.front();
            unify(pending.operand, rule.operand);
            unify(pending.result, rule.result);
            unary_resolved[i] = true;
            made_progress = true;
        }
    } while (made_progress);

    for (std::size_t i{}; i < binary_resolved.size(); ++i)
        if (!binary_resolved[i])
        {
            const PendingBinaryOperator &pending = pending_binary_operators[i];
            throw std::runtime_error(std::format("Ambiguous binary operator: {}, with the types: {} (left) and {} (right)",
                                     op_to_string(pending.op),
                                     type_to_string(pending.lhs), type_to_string(pending.rhs)));
        }

    for (std::size_t i{}; i < unary_resolved.size(); ++i)
        if (!unary_resolved[i])
        {
            const PendingUnaryOperator &pending = pending_unary_operators[i];
            throw std::runtime_error(std::format("Ambiguous unary operator: {}, with the type: {}",
                                     op_to_string(pending.op), type_to_string(pending.operand)));
        }
}