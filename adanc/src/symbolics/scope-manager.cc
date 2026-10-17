#include <print>
#include <format>

#include "scope-manager.hh"

void ScopeManager::enter_scope()
{
    scopes.push({});
}

void ScopeManager::exit_scope()
{
    if (scopes.size() > 0)
        scopes.pop();
    else
        throw std::runtime_error(std::format("Cannot go below scope 0"));
}

void ScopeManager::print_scope()
{
    std::println("Currently in scope {}", scopes.size());
}