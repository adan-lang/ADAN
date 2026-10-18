#pragma once

#include "symbol-table.hh"

class ScopeManager : public SymbolTable
{
public:
    void enter_scope();
    void exit_scope();

    bool in_global_scope() const;

    void print_scope();
};