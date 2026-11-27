#include "symbol-table.hh"

#include <format>
#include <ranges>
#include <string>

void SymbolTable::declare(const Symbol &symbol)
{
    auto &scope = scopes.back();
    if (scope.contains(symbol.name))
        diagnostics.error(std::format("Redeclaration of \"{}\"", symbol.name));

    if (warn_shadowing)
    {
        for (auto outer_scope = scopes.rbegin() + 1; outer_scope != scopes.rend(); ++outer_scope)
        {
            if (outer_scope->contains(symbol.name))
            {
                diagnostics.warning(std::format("Declaration of \"{}\" shadows an outer declaration", symbol.name));
                break;
            }
        }
    }

    scope.emplace(symbol.name, symbol);
}

void SymbolTable::warn_unused_declarations() const
{
    for (const auto &[name, symbol] : scopes.back())
    {
        if (symbol.kind == SymbolKind::Other || symbol.used || name == "_")
            continue;

        const std::string_view description = symbol.kind == SymbolKind::Parameter ? "parameter" : "local";
        diagnostics.warning(std::format("Unused {}: \"{}\"", description, name));
    }
}

Symbol *SymbolTable::lookup(std::string_view name)
{
    const std::string _name{name};
    for (auto & scope : std::views::reverse(scopes))
    {
        auto symbol = scope.find(_name);
        if (symbol != scope.end())
            return &symbol->second;
    }

    return nullptr;
}