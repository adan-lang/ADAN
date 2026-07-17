## Creating and Using Functions

Declaring a function in ADAN looks like the following:

```adan
func example(<Params>): <Type>? {

}
```

- `<Type>?` refers to an *optional* type. If no type is provided, one will be inferred from the function's result.
  - If no type is provided, `: <Type>?` can be fully omitted.

After a function is defined, it is callable using the `<Name>(<Params>);` syntax:

```adan
func example(): Int ... # Let's say this returns `1`.

example(); # <-- A function's returned value is given when calling a function.

var x = example(); # The variable we just defined now has the value `1`.
```

The compiler will throw an error if functions are used incorrectly. For example, comparing the return value of a function against a value of a different type is a compile-time error, not a runtime one.

```adan
func myName(): String {
    return "Lily";
}

var name = myName(); # Type: String, Value: "Lily"

if name == 18 { ... } # <-- Compile-time error: type mismatch between `name` (String) and `18` (Int).
```

## Parameters

**Parameters** (also called **arguments** when you're *calling* the function) are local variables defined in a function.

It's easier to show than explain, so here's a visual example:

```adan
##
Non-recognized variable keywords in params:
- var
- global

Recognized:
- readonly
##

func isOfAge(age: Number) { # <-- NOTICE: We do not use the `var` keyword here when creating a parameter, though we can use `readonly`.
    return age == 18;
}
```

We can have a technically infinite number of parameters, but that would take forever to write out by hand.

- The solution to this is the `...` keyword. `...` is an array given the `Any[]` type.
- You can iterate over provided arguments by treating `...` like a regular array.

```adan
func example(...) {
    for param in ... {

    }
}
```

> `param` in this case is the *value* of the parameter.

## Single-Lined Functions

Similarly to other languages, ADAN functions can be compacted into one line of code. When compacted, the typical `{ }` syntax can be omitted:

```adan
# Regular functions:
func normal() {
    return;
}

# Mini!
func mini() return;

# Stuff like this is ALSO possible!
var age = 18;

func is18() if age == 18 return true; else false; # <-- Stacking code with `;` as a separator is supported!
```

## Anonymous Functions (Lambdas)

Anonymous functions are functions that don't have names, containing a single expression that runs once.

ADAN builds off this concept using the `lambda` keyword, with syntax similar to regular functions:

```adan
lambda(<Params>) {

}
```