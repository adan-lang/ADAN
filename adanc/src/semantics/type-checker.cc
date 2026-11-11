#include "type-checker.hh"

#include <format>
#include <string>
#include <vector>

#include "ast-helpers.hh"
#include "type-helpers.hh"

// made using AI
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

    return "Unknown expression";
}

TypePtr TypeChecker::infer(Expr &expr)
{
    last_inferred_type = nullptr;
    expr.accept(*this);

    if (!last_inferred_type)
        diagnostics.error(std::format("Couldn't infer a type for {}. Is the corresponding TypeChecker::visit override implemented?", describe_expression(expr)));

    return last_inferred_type;
}

void TypeChecker::check(Stmt &stmt)
{
    stmt.accept(*this);
}

void TypeChecker::check(Block &block)
{
    bool unreachable{};
    for (const auto &stmt : block.stmts)
    {
        if (unreachable)
            diagnostics.warning("Unreachable statement after return");

        check(*stmt);
        if (dynamic_cast<ReturnStmt *>(stmt.get()))
            unreachable = true;
    }

    symbol_table.warn_unused_declarations();
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
                if (type_can_match(pending.lhs, rule.lhs) && type_can_match(pending.rhs, rule.rhs) && type_can_match(pending.result, rule.result))
                    matches.emplace_back(rule);
            }

            if (matches.empty())
                diagnostics.error(std::format("Type mismatch on the binary operator: {}, with the types: {} (left) and {} (right)", op_to_string(pending.op), type_to_string(pending.lhs), type_to_string(pending.rhs)));
            if (matches.size() != 1)
                continue;

            const BinaryOperator &rule = matches.front();

            unify(pending.lhs, rule.lhs, diagnostics);
            unify(pending.rhs, rule.rhs, diagnostics);
            unify(pending.result, rule.result, diagnostics);

            if (!pending.target_widenable && !(*pending.lhs == *rule.result))
                diagnostics.error("This compound assignment cannot widen its target type");

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
                if (type_can_match(pending.operand, rule.operand) && type_can_match(pending.result, rule.result))
                    matches.emplace_back(rule);
            }

            if (matches.empty())
                diagnostics.error(std::format("Type mismatch on the unary operator: {}, with the type: {}", op_to_string(pending.op), type_to_string(pending.operand)));
            if (matches.size() != 1)
                continue;

            const UnaryOperator &rule = matches.front();

            unify(pending.operand, rule.operand, diagnostics);
            unify(pending.result, rule.result, diagnostics);

            unary_resolved[i] = true;
            made_progress = true;
        }
    }
    while (made_progress);

    for (std::size_t i{}; i < binary_resolved.size(); ++i)
        if (!binary_resolved[i])
        {
            const PendingBinaryOperator &pending = pending_binary_operators[i];
            diagnostics.error(std::format("Ambiguous binary operator: {}, with the types: {} (left) and {} (right)", op_to_string(pending.op), type_to_string(pending.lhs), type_to_string(pending.rhs)));
        }

    for (std::size_t i{}; i < unary_resolved.size(); ++i)
        if (!unary_resolved[i])
        {
            const PendingUnaryOperator &pending = pending_unary_operators[i];
            diagnostics.error(std::format("Ambiguous unary operator: {}, with the type: {}", op_to_string(pending.op), type_to_string(pending.operand)));
        }
}