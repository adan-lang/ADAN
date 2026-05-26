You can declare a variable in ADAN using the `var` keyword. Variables are both *mutable by default* and *infer* (assume) *the type* at runtime.

- Optionally, a type can be provided if it would not be explicitly known at compile time (**i.e.** sending HTTP requests and saving the response)

Variables can be read only (immutable) using the `readonly` keyword beforehand.

```adan
var a = 1;

var b: <type>? = 1;

a = 2;

readonly var c: <type>? = 1;
```

- `<type>?` Refers to an *optional* type, if no type is provided, one will be inferred by the function's result.
- - If no type is provided, `: <type>?` can be fully omitted.

Unlike *most modern languages* (**i.e.** JavaScript, Python, etc.), variables are capable of starting with integers.

```
var 18thVariable = 18;
```

---

And finally, a nice feature ADAN introduces are *global variables*.

- Using the `global` keyword before `var`/`readonly` allows that variable to be used anywhere after it's defined!

```adan
readonly var age = 18; # Scope 0 (Highest)

func sampleScope() {
    global readonly var name = "Lily"; # Scope 1 (Deeper)
}

# `name` CAN be accessed from a scope higher than where it's defined.
```

> [!NOTE]
> When assigning a global variable a new value, it must also be given the `global` tag.

---

In conclusion, here are all of the various ways variables may be defined:

```adan
var a = 1;

var b: <type>? = 1;

a = 2; # Assigning an existing variable a new value.
       # Cannot declare variables this way.

readonly var c: <type>? = 1;

global var d: <type>? = 1;

global readonly var e: <type>? = 1;

global d = 2;
```