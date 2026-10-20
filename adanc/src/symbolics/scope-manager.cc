#include <print>
#include <format>

#include "scope-manager.hh"

void ScopeManager::enter_scope()
{
    scopes.emplace_back();
}

void ScopeManager::exit_scope()
{
    if (!in_global_scope())
        scopes.pop_back();
    else
        throw std::runtime_error(std::format("Cannot go below the global scope"));
}

bool ScopeManager::in_global_scope() const
{
    return scopes.empty();
}

void ScopeManager::print_scope()
{
    std::println("Currently in scope {}", scopes.size());
}