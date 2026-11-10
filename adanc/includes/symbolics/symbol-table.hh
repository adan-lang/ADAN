#pragma once

#include <format>
#include <print>
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

  std::string to_string() const {
    return std::format("Symbol(Name: {}, Type: {})", name,
                       type_to_string(type));
  }
};

// @todo Fix this garbage shit and make it use a tree instead of a flat stack of
// Symbols sometime
using Scope = std::unordered_map<std::string, Symbol>;

class SymbolTable {
protected:
  std::vector<Scope> scopes;
  Diagnostics &diagnostics;
  bool warn_shadowing{true};

public:
  explicit SymbolTable(Diagnostics &diagnostics) : diagnostics{diagnostics} {}

  void declare(const Symbol &symbol);

  Symbol *lookup(std::string_view name);

  void set_shadow_warnings_enabled(bool enabled) { warn_shadowing = enabled; }

  void warn_unused_declarations() const;

  // @todo Remember to remove this after everything is finished
  inline void print_symbols() {
    for (auto item = scopes.rbegin(); item != scopes.rend(); ++item) {
      const auto &curr = *item;
      for (const auto &[name, symbol] : curr) {
        std::println("Key: {}, Symbol: {}", name, symbol.to_string());
      }
    }
  }
};