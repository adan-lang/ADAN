#include <format>

#include "symbol-table.hh"

void SymbolTable::declare(const Symbol &symbol)
{
    auto &scope = scopes.top();
    if (lookup(symbol.name))
    {
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Redeclaration of \"{}\"", symbol.name));
    }

    scope[symbol.name] = symbol;
}

Symbol *SymbolTable::lookup(std::string_view name)
{
    auto temp_scopes = scopes;
    while (!temp_scopes.empty())
    {
        // @note `scope` is an unordered map since `temp_scores` is a stack of them
        // The `.count` method on a hashmap returns the amt. of elements on a key, so we're checking if it's not 0
        auto &scope = temp_scopes.top();
        if (scope.count(name))
            return &scope[name];
        temp_scopes.pop();
    }
    
    return nullptr;
}