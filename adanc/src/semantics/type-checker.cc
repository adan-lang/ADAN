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

// Visitors

void TypeChecker::visit(IntLiteral &)
{
    last_inferred_type = make_int();
}

void TypeChecker::visit(FloatLiteral &)
{
    last_inferred_type = make_float();
}

void TypeChecker::visit(BoolLiteral &)
{
    last_inferred_type = make_bool();
}

void TypeChecker::visit(StringLiteral &)
{
    last_inferred_type = make_string();
}

void TypeChecker::visit(LocalDecl &node)
{
    
}

void TypeChecker::visit(FuncDecl &node)
{
}