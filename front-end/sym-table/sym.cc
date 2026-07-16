#include <string>
#include <vector>
#include <string_view>

#include "sym.hh"

void SymbolTableManager::enter_scope()
{

}

void SymbolTableManager::exit_scope()
{

}

bool SymbolTableManager::insert(
    std::string_view name,
    SymbolInfo info
) {

}

bool SymbolTableManager::insert_global(
    std::string_view name,
    SymbolInfo info
) {

}

SymbolInfo* SymbolTableManager::lookup(
    std::string_view name
) {

}

bool SymbolTableManager::in_current_scope(
    std::string_view name
) const {

}

SymbolTableManager::SymbolTableManager()
{

}

SymbolTableManager::~SymbolTableManager()
{
    
}