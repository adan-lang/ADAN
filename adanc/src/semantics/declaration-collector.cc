#include <print>

#include "declaration-collector.hh"

void DeclarationCollector::visit(LocalDecl &stmt)
{
    std::println("Testing");
}

void DeclarationCollector::visit(FuncDecl &stmt)
{

}