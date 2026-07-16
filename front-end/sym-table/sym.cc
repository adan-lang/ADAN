#include <string>
#include <vector>
#include <string_view>
#include <memory>

#include "sym.hh"

void SymbolTableManager::enter_scope()
{
    scope_stack.push_back(std::make_unique<SymbolTable>());
}

void SymbolTableManager::exit_scope()
{
    if (!scope_stack.empty())
    {
        scope_stack.pop_back();
    }
}

bool SymbolTableManager::insert(
    std::string_view name,
    SymbolInfo info
) {
    if (scope_stack.empty()) // when we have no scope (what??) :(
    {
        return false;
    }

    scope_stack.back()->insert(name, std::move(info));
}

bool SymbolTableManager::insert_global(
    std::string_view name,
    SymbolInfo info
) {
    return globals.insert(name, std::move(info));
}

SymbolInfo* SymbolTableManager::lookup(
    std::string_view name
) {
    // ok so... the plan here is to walk from the innermost scope
    //  to the outermost scope :3
    
    /**
     * .rbegin() = Returns a read/write reverse iterator that points to the
     *              last element in the vector. Iteration is done in reverse
     *              element order.
     */
    for (auto item = scope_stack.rbegin(); item != scope_stack.rend(); ++item)
    {
        SymbolInfo* found = (*item)->lookup(name);

        if (found != nullptr)
        {
            return found;
        }
    }

    return globals.lookup(name);
    
    // love from emily <3 (love you lily :3)
}

bool SymbolTableManager::in_current_scope(
    std::string_view name
) const {
    if (scope_stack.empty())
    {
        return false;
    }

    /**
     * .back() = Returns a read-only (constant) reference to the data at the
     *            last element of the vector.
     */
    return scope_stack.back()->contains(name);
}

SymbolTableManager::SymbolTableManager()
{
    // we're starting with one scope so `insert`/`lookup` *always* have
    //  somewhere to write to beforte the first explicit `enter_scope()` is called.
    scope_stack.push_back(std::make_unique<SymbolTable>());
}

SymbolTableManager::~SymbolTableManager() { }