#pragma once

#include <memory>
#include <string>
#include <vector>
#include <string_view>

#include "sym-info.hh"
#include "sym-table.hh"

/**
 * 
 * owns an entire stack of scopes for a single file. includes a
 *  flat table for `global` declared symbols, since they're visible
 *  from any scope regardless of nesting.
 */
class SymbolTableManager
{
private:
    std::vector<std::unique_ptr<SymbolTable>> scope_stack;
    SymbolTable globals;
public:
    void enter_scope();

    void exit_scope();

    /**
     * 
     * inserts into the current (innermost) scope.
     */
    bool insert(
        std::string_view name,
        SymbolInfo info
    );

    /**
     * 
     * inserts into the flat global table (`global var` / `global readonly var`).
     */
    bool insert_global(
        std::string_view name,
        SymbolInfo info
    );

    /**
     * 
     * searches the current scope, then walks UP through enclosing scopes,
     *  if it cannot be found, falls back to the global table.
     */
    SymbolInfo* lookup(
        std::string_view name
    );

    /**
     * 
     * checks ONLY the innermost scope (e.g. for redeclaration issues).
     */
    bool in_current_scope(
        std::string_view name
    ) const;

    SymbolTableManager();

    ~SymbolTableManager();
};
