#pragma once

#include "symbol-table.hh"

class ScopeManager : public SymbolTable {
  public:
    explicit ScopeManager(Diagnostics &diagnostics) : SymbolTable{diagnostics} {}

    void enter_scope();
    void exit_scope();

    bool in_global_scope() const;

    void print_scope();
};