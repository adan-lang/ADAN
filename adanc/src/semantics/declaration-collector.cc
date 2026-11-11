#include "declaration-collector.hh"

#include <cstddef>
#include <vector>

void DeclarationCollector::visit(LocalDecl &stmt)
{
    symbol_table.declare(Symbol{
        stmt.name,
        make_var(),
        SymbolKind::Local,
    });
}

void DeclarationCollector::visit(FuncDecl &stmt)
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

    for (std::size_t i{}; i < params.size(); ++i)
    {
        symbol_table.declare(Symbol{
            stmt.params[i],
            params[i],
            SymbolKind::Parameter,
        });
    }

    for (const auto &body_stmt : stmt.body.stmts)
    {
        body_stmt->accept(*this);
    }

    symbol_table.exit_scope();
}

void DeclarationCollector::visit(IfStmt &stmt)
{
    symbol_table.enter_scope();
    for (const auto &body_stmt : stmt.body.stmts)
        body_stmt->accept(*this);
    symbol_table.exit_scope();
}

void DeclarationCollector::visit(NumericForStmt &stmt)
{
    symbol_table.enter_scope();
    symbol_table.declare(Symbol{stmt.name, make_int(), SymbolKind::Local});
    for (const auto &body_stmt : stmt.body.stmts)
        body_stmt->accept(*this);
    symbol_table.exit_scope();
}

void DeclarationCollector::visit(ForInStmt &stmt)
{
    symbol_table.enter_scope();
    symbol_table.declare(Symbol{stmt.var, make_var(), SymbolKind::Local});
    for (const auto &body_stmt : stmt.body.stmts)
        body_stmt->accept(*this);
    symbol_table.exit_scope();
}

void DeclarationCollector::visit(WhileStmt &stmt)
{
    symbol_table.enter_scope();
    for (const auto &body_stmt : stmt.body.stmts)
        body_stmt->accept(*this);
    symbol_table.exit_scope();
}