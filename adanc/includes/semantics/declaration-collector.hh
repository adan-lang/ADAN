#pragma once

#include "scope-manager.hh"
#include "ast-visitor.hh"

class DeclarationCollector : public ASTVisitor
{
    ScopeManager &symbol_table;

public:
    explicit DeclarationCollector(ScopeManager &symbol_table) : symbol_table(symbol_table) {}

    void visit(LocalDecl &stmt) override;
    void visit(FuncDecl &stmt) override;
};