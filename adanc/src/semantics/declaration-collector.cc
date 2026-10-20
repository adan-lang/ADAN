#include <vector>
#include <format>
#include <print>

#include "declaration-collector.hh"
#include "type-helpers.hh"

void DeclarationCollector::visit(LocalDecl &stmt)
{
    symbol_table.declare(Symbol{
        stmt.name,
        make_var(),
    });
}

void DeclarationCollector::visit(FuncDecl &stmt)
{
    // function a { ... }
    // function b -> () { ... }
    // function c -> (d, e) { ... }

    if (!symbol_table.lookup(stmt.name))
    {

    }
    else
    {
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Redeclaration of function: \"{}\"", stmt.name));
    }
}