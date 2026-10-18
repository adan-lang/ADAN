#pragma once

#include "symbol-table.hh"
#include "ast-visitor.hh"

class DeclarationCollector : public ASTVisitor
{
    SymbolTable* symbol_table;

public:
    void visit(LocalDecl &node) override;
    void visit(FuncDecl &node) override;
};