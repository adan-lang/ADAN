## Basic Variable Rules

You can declare a variable in ADAN using the `var` keyword. Variables are both *mutable by default* and *infer* (assume) *the type* at runtime.

- Optionally, a type can be provided if it would not be explicitly known at compile time (**i.e.** sending HTTP requests and saving the response).

Variables can be made read-only (immutable) using the `readonly` keyword beforehand.

```adan
var a = 1;

var b: <Type>? = 1;

a = 2;

readonly var c: <Type>? = 1;
```

- `<Type>?` refers to an *optional* type. If no type is provided, one will be inferred from the variable's value. An optional type also means the variable is allowed to hold `None`.
  - If no type is provided, `: <Type>?` can be fully omitted.

Unlike *most modern languages* (**i.e.** JavaScript, Python, etc.), variables are capable of starting with integers.

```adan
var 18thVariable = 18;
```

---

## Global Variables

And finally, a nice feature ADAN introduces are *global variables*.

- Using the `global` keyword before `var`/`readonly` allows that variable to be used anywhere after it's defined.

```adan
readonly var age = 18; # Scope 0 (Highest)

func sampleScope() {
    global readonly var name = "Lily"; # Scope 1 (Deeper)
}

# `name` CAN be accessed from any scope higher or lower than where it's defined.
```

> [!NOTE]
> When assigning a global variable a new value, it must also be given the `global` tag, this includes compound assignment operators like `+=` and `-=`.

```adan
global var count = 0;

global count += 1; # count is now 1
```

---

## Conclusion

In conclusion, here are all of the various ways variables may be defined:

```adan
var a = 1;

var b: <Type>? = 1;

a = 2; # Assigning an existing variable a new value.
       # Cannot declare variables this way.

readonly var c: <Type>? = 1;

global var d: <Type>? = 1;

global readonly var e: <Type>? = 1;

global d = 2;

global d += 1; # d is now 3
```