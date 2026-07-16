#pragma once

#include <string>
#include <unordered_map>
#include <string_view>

#include "sym-info.hh"

class SymbolTable
{
private:
    std::unordered_map<std::string, SymbolInfo> symbols;

public:
    /**
     * 
     * add new information into the current symbol table.
     */
    bool insert(
        std::string_view name,
        SymbolInfo info
    );

    /**
     * 
     * searches the current scope in the current symbol table.
     */
    SymbolInfo* lookup(
        std::string_view name
    );

    /**
     * 
     * verifies if the specified string is in the current symbol table
     *  at all.
     */
    bool contains(
        std::string_view name
    ) const;

    SymbolTable();

    ~SymbolTable();
};
