#pragma once

#include <unordered_map>
#include <stack>
#include <stdexcept>

#include "symbol.hh"

class SymbolTable
{
    std::stack<std::unordered_map<std::string_view, Symbol>> scopes;

public:
    SymbolTable()
    {
        enter_scope();
    }

    void enter_scope();
    void exit_scope();

    void declare(const Symbol &symbol);

    Symbol *lookup(std::string_view name);
};