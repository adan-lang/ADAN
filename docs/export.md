## Exporting from Modules

By default, nothing in an ADAN file is visible outside of it. The `export` keyword marks a declaration as part of a module's public interface, making it available to anyone who uses that module.

---

## Exporting Declarations

The `export` keyword can be placed before any top-level declaration.

```adan
export func add(a: Number, b: Number): Number {
    return a + b;
}

export var PI = 3.14159;

export readonly var VERSION = "1.0.0";
```

Anything *without* `export` remains private to the file, and cannot be accessed from outside, even if the module is loaded.

---

## Why This Exists

Without explicit exports, every symbol in every file would be globally visible, which causes naming collisions and makes it impossible to reason about what a module actually provides.

`export` forces you to be intentional about your module's public surface. Everything else is an internal implementation detail.

---

## How It Connects to `using module`

When another file does:

```adan
using module math;
```

It only gains access to what `math` explicitly exported. Private declarations stay hidden.

```adan
# math.adn

export func sqrt(n: Number): Number { ... }  # visible to importers

func internal_helper(): Number { ... }       # not visible
```