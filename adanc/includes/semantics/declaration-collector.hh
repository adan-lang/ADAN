#pragma once

#include "symbol-table.hh"
#include "ast-visitor.hh"

class DeclarationCollector : public ASTVisitor
{
    SymbolTable &symbol_table;

public:
    explicit DeclarationCollector(SymbolTable &symbol_table) : symbol_table(symbol_table) {}

    void visit(LocalDecl &stmt) override;
    void visit(FuncDecl &stmt) override;
};