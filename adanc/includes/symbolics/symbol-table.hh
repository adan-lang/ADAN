#pragma once

#include <format>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>
#include <print>

#include "types.hh"
#include "type-helpers.hh"

// @note Possibly in the future consider `basic::Maybe` if adding support for anonymous functions
struct Symbol
{
    std::string name;
    TypePtr type;

    std::string to_string() const
    {
        return std::format("Symbol(Name: {}, Type: {})", name, type_to_string(type));
    }
};

using Scope = std::unordered_map<std::string, Symbol>;

class SymbolTable
{
protected:
    std::vector<Scope> scopes;

public:
    void declare(const Symbol &symbol);

    Symbol *lookup(std::string_view name);

    // @todo Remember to remove this after everything is finished
    inline void print_symbols()
    {
        for (auto item = scopes.rbegin(); item != scopes.rend(); ++item)
        {
            const auto &curr = *item;
            for (const auto &[name, symbol] : curr)
            {
                std::println("Key: {}, Symbol: {}", name, symbol.to_string());
            }
        }
    }
};