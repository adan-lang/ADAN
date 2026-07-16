#include <string_view>

#include "sym-table.hh"

bool SymbolTable::insert(
    std::string_view name,
    SymbolInfo info
) {
    auto result = symbols.emplace(std::string(name), std::move(info));

    return result.second;
}

SymbolInfo* SymbolTable::lookup(
    std::string_view name
) {
    auto item = symbols.find(std::string(name));

    if (item == symbols.end())
    {
        return nullptr;
    }

    return &item->second;
}

bool SymbolTable::contains(
    std::string_view name
) const {
    return symbols.find(std::string(name)) != symbols.end();
}

SymbolTable::SymbolTable() { }

SymbolTable::~SymbolTable() { }