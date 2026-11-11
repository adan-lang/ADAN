#pragma once

#include <format>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "diagnostics.hh"
#include "type-helpers.hh"
#include "types.hh"

enum class SymbolKind {
    Other,
    Local,
    Parameter,
};

struct Symbol {
    std::string name;
    TypePtr type;
    SymbolKind kind{SymbolKind::Other};
    bool used{};

    std::string to_string() const
    {
        return std::format("Symbol(Name: {}, Type: {})", name, type_to_string(type));
    }
};

using Scope = std::unordered_map<std::string, Symbol>;

class SymbolTable {
  protected:
    std::vector<Scope> scopes;
    Diagnostics &diagnostics;
    bool warn_shadowing{true};

  public:
    explicit SymbolTable(Diagnostics &diagnostics) : diagnostics{diagnostics}
    {
    }

    void declare(const Symbol &symbol);

    Symbol *lookup(std::string_view name);

    void set_shadow_warnings_enabled(bool enabled)
    {
        warn_shadowing = enabled;
    }

    void warn_unused_declarations() const;
};