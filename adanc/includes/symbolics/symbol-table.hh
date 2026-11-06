#pragma once

#include <format>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>

#include "types.hh"
#include "type-helpers.hh"

struct Symbol
{
    std::string name;
    TypePtr type;

    std::string to_string() const
    {
        return std::format("Symbol(Name: {}, Type: {})", name, type_to_string(type));
    }
};

// @todo Fix this garbage shit and make it use a tree instead of a flat stack of Symbols sometime
using Scope = std::unordered_map<std::string, Symbol>;

class SymbolTable
{
protected:
    std::vector<Scope> scopes;

public:
    void declare(const Symbol &symbol);

    Symbol *lookup(std::string_view name);
};