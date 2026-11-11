#include "built-ins.hh"

#include "symbol-table.hh"
#include "types.hh"

void declare_builtins(SymbolTable &table)
{
    {
        table.declare(Symbol{"print", make_function({make_string()}, make_void())});
    }
}