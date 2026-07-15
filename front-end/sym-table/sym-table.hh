#pragma once

#include <string>
#include <unordered_map>

#include "sym-info.hh"

class SymbolTable
{
private:
    std::unordered_map<std::string, SymbolInfo> symbols;

public:
    bool insert(
        const std::string& name,
        SymbolInfo info
    );

    SymbolInfo* lookup(
        const std::string& name
    );

    bool contains(
        const std::string& name
    ) const;

    SymbolTable();

    ~SymbolTable();
};
