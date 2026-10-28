#include "type-checker.hh"

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