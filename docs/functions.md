## Creating and Using Functions

Declaring a function in ADAN will be like the following:

```adan
func example(<Params>): <Type>? {

}
```

- `<Type>?` Refers to an *optional* type, if no type is provided, one will be inferred by the function's result.
- - If no type is provided, `: <Type>?` can be fully omitted.

After a function is defined, it is callable using the `<Name>(<Params>);` syntax:

```adan
func example(): Integer ... # Let's say this returns `1`.

example(); # <-- A function's returned value is given when calling a function.

var x = example(); # The variable we just defined now has the value `1`.
```

The compiler will throw an error if functions are used incorrectly, however. An example of that would be if you're trying to compare the return value of a function with a value in an if statement and the function's return value is a completely different type than the one you are trying to match with.

Here's an example:

```adan
func myName(): String {
    return "Lily";
}

var name = myName(); # Type: String, Value: "Lily"

if name == 18 { ... } # <-- This would throw a compile-time error because of the type mismatch between `name` and `18`. (String vs. Integer)
```

## Parameters

**Parameters** (otherwise called *arguments** when you're *calling* the function), are local variables defined in a function.

It's quite hard to put into words, so here is a visual example:

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

We can have a technically infinite amount of parameters, but that would take FOREVER to do by hand.

- A solution to this is using the `...` keyword. "`...`" is an array given the `Any[]` type.
- We can iterate over provided arguments by treating "`...`" like a regular array.

Here's an example:

```adan
func example(...) {
    for param in ... {
        
    }
}
```

> `param` in this case is the *value* of the parameter.

## Single-lined Functions

Similarly to other languages, ADAN functions are able to be compacted into 1 line of code. When compacted, the typical `{ }` syntax can be omitted, it looks like this:

```adan
# Regular functions:
func normal() {
    return;
}

# Mini!
func mini() return;

# Stuff like this is ALSO possible!
var age = 18;

func is18() if age == 18 return true; else false; # <-- Stacking code upon eachother whilst splitting them with a `;` is supported!
```

## Anonymous Functions (Lambdas)

Anonymous functions are types of functions that don't have names, containing one expression that are ran a single time.

ADAN builds off this concept using the `lambda` keyword, with similar syntax for functions:

```adan
lambda(<Params>) {

}
```