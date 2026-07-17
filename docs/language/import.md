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