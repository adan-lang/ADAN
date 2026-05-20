You can declare a variable in ADAN using the `var` keyword. Variables are both *mutable by default* and *infer* (assume) *the type* at runtime.

- Optionally, a type can be provided if it would not be explicitly known at compile time (**i.e.** sending HTTP requests and saving the response)

Variables can be read only (immutable) using the `readonly` keyword beforehand.

```adan
var a = 1;

var b: <type>? = 1;

a = 2;

readonly var c = 1;
```

- `<type>?` Refers to an *optional* type, if no type is provided, one will be inferred by the function's result.
- - If no type is provided, `: <type>?` can be fully omitted.

Unlike *most modern languages* (**i.e.** JavaScript, Python, etc.), variables are capable of starting with integers.

```
var 18thVariable = 18;
```