#include <print>
#include <format>

#include "scope-manager.hh"

void ScopeManager::enter_scope()
{
    scopes.push({});
}

void ScopeManager::exit_scope()
{
    if (!in_global_scope())
        scopes.pop();
    else
        throw std::runtime_error(std::format("Cannot go below the global scope"));
}

bool ScopeManager::in_global_scope() const
{
    return scopes.size() == 0;
}

void ScopeManager::print_scope()
{
    std::println("Currently in scope {}", scopes.size());
}