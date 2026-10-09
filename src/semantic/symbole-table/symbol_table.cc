#include <format>

#include "symbol_table.hh"

void SymbolTable::enter_scope()
{
    scopes.push({});
}

void SymbolTable::exit_scope()
{
    scopes.pop();
}

void SymbolTable::declare(const Symbol &symbol)
{
    auto &scope = scopes.top();
    if (scope.count(symbol.name))
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Redeclaration of \"{}\"", symbol.name));
    scope[symbol.name] = symbol;
}

Symbol *SymbolTable::lookup(std::string_view name)
{
    auto temp = scopes;
    while (!temp.empty())
    {
        auto &scope = temp.top();
        if (scope.count(name))
            return &scope[name];
        temp.pop();
    }

    return nullptr;
}