#pragma once

#include <unordered_map>
#include <stack>
#include <stdexcept>
#include <string>
#include <print>

#include "types.hh"

// @note Possibly in the future consider `basic::Maybe` if adding support for anonymous functions
struct Symbol
{
    std::string name;
    TypePtr type;

    std::string to_string() const
    {
        return std::format("Symbol(Name: {}, Type: {})", name, type);
    }
};

template <typename T, typename Container = std::deque<T>>
struct _stack : std::stack<T, Container>
{
    using std::stack<T, Container>::c;
};

class SymbolTable
{
protected:
    _stack<std::unordered_map<std::string_view, Symbol>> scopes;

public:
    void declare(const Symbol &symbol);

    Symbol *lookup(std::string_view name);

    inline void print_scopes()
    {
        for (auto item = scopes.c.rbegin(); item != scopes.c.rend(); ++item)
        {
            const auto& curr = *item;
            for (const auto& [name, symbol] : curr)
            {
                std::println("Key: {}, Symbol: {}", name, symbol.to_string());
            }
        }
    }
};