#include <stdexcept>
#include <string>
#include <format>

#include "type-checker.hh"

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