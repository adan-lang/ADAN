#pragma once

#include "ast-visitor.hh"
#include "scope-manager.hh"
#include "types.hh"

class TypeChecker : public ASTVisitor
{
    ScopeManager &symbol_table;
    TypePtr last_inferred_type;

public:
    explicit TypeChecker(ScopeManager &symbols) : symbol_table(symbols) {}

    TypePtr infer(Expr &expr);

    void check(Stmt &stmt);
    void check(Block &block);

    void visit(IntLiteral &node) override;
    void visit(LocalDecl &node) override;
    void visit(FuncDecl &node) override;
};