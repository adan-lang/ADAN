#include <print>

#include "declaration-collector.hh"

void DeclarationCollector::visit(LocalDecl &stmt)
{
    symbol_table.declare(Symbol{
        stmt.name,
        make_var(),
    });
}

void DeclarationCollector::visit(FuncDecl &stmt)
{

}