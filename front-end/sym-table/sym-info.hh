#pragma once

#include <string>
#include <vector>

#include "type.hh"

enum class StorageClass {
    LOCAL,
    GLOBAL,
    PARAM,
};

enum class SymbolKind
{
    VARIABLE,
    FUNCTION,
    PARAMETER,
};

struct SymbolInfo // think of these as like addons or flags per-say
{
    std::string name;
    SymbolKind kind;
    Type* type;

    bool is_readonly;
    bool is_global;
    bool is_exported;
    bool is_variadic;

    StorageClass storage;
    int offset;                         // stack offset / frame slot / etc.
    
    int decl_line;
    int decl_col;
    bool is_defined;                    // declared vs. actually assigned/implemented

    SymbolInfo();
};