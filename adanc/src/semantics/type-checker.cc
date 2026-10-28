#include <stdexcept>

#include "type-checker.hh"

TypePtr TypeChecker::infer(Expr &expr)
{
    last_inferred_type = nullptr;
    expr.accept(*this);

    if (!last_inferred_type)
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error("Couldn't infer this type! Is its TypeChecker::visit override implemented?");

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