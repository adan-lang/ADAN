#include <print>
#include <format>

#include "scope-manager.hh"

void ScopeManager::enter_scope()
{
    scopes.emplace_back();
}

void ScopeManager::exit_scope()
{
    if (scopes.empty())
        throw std::runtime_error("No active scope");
    if (in_global_scope())
        throw std::runtime_error("Cannot exit the global scope");

    scopes.pop_back();
}

bool ScopeManager::in_global_scope() const
{
    return scopes.size() == 1;
}

void ScopeManager::print_scope()
{
    std::println("Currently in scope {}", scopes.size());
}