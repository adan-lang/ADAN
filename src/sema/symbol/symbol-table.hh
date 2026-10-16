#pragma once

#include <unordered_map>
#include <stack>
#include <stdexcept>
#include <string>

#include "types.hh"

// @note Possibly in the future consider `basic::Maybe` if adding support for anonymous functions
struct Symbol
{
    std::string name;
    TypePtr type;
};

class SymbolTable
{
    std::stack<std::unordered_map<std::string_view, Symbol>> scopes;

public:
    void enter_scope();
    void exit_scope();

    void declare(const Symbol &symbol);

    Symbol *lookup(std::string_view name);
};