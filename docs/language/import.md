## Importing Files and Libraries

ADAN has two separate systems for bringing external code into your program, each with a distinct purpose.

## `include`: File System

The `include` keyword takes a file path, reads it, parses it, and injects its contents directly into the current compilation.

```adan
include "src/utils/parser.adn";
include "../lib/math.adn";
```

> [!NOTE]
> If a file moves, any `include` pointing to it will break. Be mindful of this when structuring projects.

## `using module`: Module System

The `using module` keyword resolves a module by *name* rather than by path, searching configured module paths to find and load it.

```adan
using module std.io;
using module math;
```

- Modules are identified by a logical name, not a file path.
- Files can move freely without breaking any `using module` references.

## Accessing Module Members

Once a module is brought in with `using module`, its exported members are reached through the module's *last* path segment, using `.`:

```adan
using module std.io;

io.log("Hello, world!"); # `std.io` is accessed as just `io`
```

```adan
using module math;

math.sqrt(16); # a module with no parent path keeps its full name
```

Only what the module explicitly marked with `export` is reachable this way. See [Exporting from Modules](./export.md) for how a module controls what it exposes.