#include <cstddef>
#include <vector>
#include <format>
#include <stdexcept>

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
    if (!symbol_table.lookup(stmt.name))
    {
        std::vector<TypePtr> params;
        for (std::size_t i{}; i < stmt.params.size(); ++i)
        {
            params.emplace_back(make_var());
        }

        symbol_table.declare(Symbol{
            stmt.name,
            make_function(params, make_var()),
        });

        symbol_table.enter_scope();

        // std::println("{}", params.size());
        for (std::size_t i{}; i < params.size(); ++i)
        {
            // std::println("{}, {}", stmt.params[i], type_to_string(params[i]));
            symbol_table.declare(Symbol{
                stmt.params[i],
                params[i],
            });
        }

        for (const auto &body_stmt : stmt.body.stmts)
        {
            body_stmt->accept(*this);
        }

        symbol_table.exit_scope();
    }
    else
    {
        // @todo @important Design a diagnostics system for debugging during semantic analysis
        throw std::runtime_error(std::format("Redeclaration of function: \"{}\"", stmt.name));
    }
}