#pragma once

#include "symbol-table.hh"

class ScopeManager : public SymbolTable
{
public:
    void enter_scope();
    void exit_scope();

    void print_scope();
};