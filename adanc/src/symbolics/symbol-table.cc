#include <string>

#include "symbol-table.hh"

void SymbolTable::declare(const Symbol &symbol)
{
    auto &scope = scopes.back();
    // if (lookup(symbol.name))
    // {
    //     // @todo @important Design a diagnostics system for debugging during semantic analysis
    //     throw std::runtime_error(std::format("Redeclaration of \"{}\"", symbol.name));
    // }

    scope[symbol.name] = symbol;
}

Symbol *SymbolTable::lookup(std::string_view name)
{
    const std::string _name{name};
    for (auto scope = scopes.rbegin(); scope != scopes.rend(); ++scope)
    {
        auto symbol = scope->find(_name);
        if (symbol != scope->end())
            return &symbol->second;
    }

    return nullptr;
}