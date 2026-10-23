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
        std::vector<TypePtr> params;
        for (std::size_t i = 0; i < stmt.params.size(); ++i)
        {
            params.emplace_back(string_to_type(stmt.params[i]));
        }

        symbol_table.declare(Symbol{
            stmt.name,
            make_function(params, make_var()),
        });
    }
    else
    {
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Redeclaration of function: \"{}\"", stmt.name));
    }
}