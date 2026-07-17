## Using the Symbol Table

The symbol table tracks every declared name, variables, functions, and parameters, along with its type and metadata, throughout compilation. It's what later stages use to answer questions like "does this name exist here?" and "what type is it?".

### `SymbolInfo`

Each entry in the symbol table is a `SymbolInfo`:

```cpp
struct SymbolInfo
{
    std::string name;
    SymbolKind kind;        // VARIABLE, FUNCTION, or PARAMETER
    Type* type;

    bool is_readonly;
    bool is_global;
    bool is_exported;
    bool is_variadic;

    StorageClass storage;    // LOCAL, GLOBAL, or PARAM
    int offset;              // stack offset / frame slot / etc.

    int decl_line;
    int decl_col;
    bool is_defined;         // declared vs. actually assigned/implemented
};
```

### Scopes and the Global Table

`SymbolTableManager` owns two things: a stack of nested `SymbolTable`s (one per lexical scope) and a single flat `SymbolTable` for `global`-declared symbols. Scopes model regular block/function nesting, the global table exists separately because a `global var` needs to be reachable from *any* scope, not just outward through its enclosing ones.

A `SymbolTableManager` always starts with one scope already on the stack, so `insert`/`lookup` have somewhere to write to even before your first explicit `enter_scope()` call.

```cpp
#include "sym.hh"

SymbolTableManager symbols;

symbols.enter_scope(); // e.g. entering a function body
// ... insert params and locals ...
symbols.exit_scope();  // leaving the function body
```

### Inserting Symbols

Use `insert()` for anything scoped normally (locals, parameters):

```cpp
SymbolInfo info;
info.name = "age";
info.kind = SymbolKind::VARIABLE;
info.type = /* an Int Type* */;
info.decl_line = 4;
info.decl_col = 9;

bool ok = symbols.insert("age", info);
```

Use `insert_global()` specifically for `global var` / `global readonly var` declarations, since these live in the flat global table instead of the current scope:

```cpp
bool ok = symbols.insert_global("count", info);
```

Both return `false` if the name already exists in the target table, this is how you detect illegal redeclaration.

### Looking Up Symbols

`lookup()` walks from the innermost scope outward, and only falls back to the global table if nothing is found in any enclosing scope:

```cpp
SymbolInfo* found = symbols.lookup("age");

if (found == nullptr)
{
    // name doesn't exist anywhere visible from here
}
```

If you specifically need to know whether a name is already declared *in the current scope only* (for example, to reject a redeclaration before even attempting `insert()`), use `in_current_scope()` instead, it deliberately does not walk outward or check the global table:

```cpp
if (symbols.in_current_scope("age"))
{
    // error: "age" is already declared in this exact scope
}
```

### Putting It Together

A minimal walkthrough resembling how a function body might be processed:

```cpp
SymbolTableManager symbols;

// Top-level: `readonly var age = 18;`
SymbolInfo age_info;
age_info.name = "age";
age_info.kind = SymbolKind::VARIABLE;
age_info.is_readonly = true;
symbols.insert("age", age_info);

// Entering `func sampleScope() { ... }`
symbols.enter_scope();

    // `global readonly var name = "Lily";`
    SymbolInfo name_info;
    name_info.name = "name";
    name_info.kind = SymbolKind::VARIABLE;
    name_info.is_readonly = true;
    name_info.is_global = true;
    symbols.insert_global("name", name_info);

symbols.exit_scope();

// `name` is reachable from anywhere, `age` is only reachable
//  from scopes at or below where it was declared.
SymbolInfo* found_name = symbols.lookup("name"); // found via the global table
SymbolInfo* found_age  = symbols.lookup("age");  // found via the outer scope
```

### Types

Every `SymbolInfo::type` points to a `Type`, defined in `type.hh`. `Type` supports structural equality via `Type::equals()`, arrays via `element_type`, and function signatures via `param_types`/`return_type`. `Type::make_string()` produces a human-readable form, useful for diagnostics:

```cpp
Type* int_type = new Type();
int_type->kind = TypeKind::INT;

std::cout << int_type->make_string(); // "int"
```

A `Type` with `kind == TypeKind::ANY` compares equal to any other type via `equals()`, this is used sparingly, mainly for contexts like `...` variadic parameters where the element type is intentionally unconstrained.