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
protected:
    std::stack<std::unordered_map<std::string_view, Symbol>> scopes;

public:
    void declare(const Symbol &symbol);

    Symbol *lookup(std::string_view name);
};