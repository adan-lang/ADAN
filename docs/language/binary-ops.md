## Binary Operators

Binary operators are kinds of operators that expect *both* a left and a right hand side value.

An example of a binary operator would be `+`. In addition, you need a value to add, and a value to add on to.

```adan
1 + 1
```

Binary operators are also called Binops for short, which is the name I'll be using from now on. As well as LHS and RHS representing the *left* and *right* hand side operands.

### Arithmetic / Math Binops

- `+`: Addition, LHS and RHS values added together. Works on any `Number` type (`Int` or `Float`).
- `-`: Subtraction, the *opposite* of addition, you take away instead of combine.
- `*`: Multiplication, you're adding the LHS value n (RHS) times.
- `/`: Division, you're subtracting the RHS value from the LHS repeatedly, and the number of times you can do that (plus any leftover) is your result.
  - If the LHS divides evenly by the RHS, the result is an `Int`.
  - Otherwise, the result is a `Float`.

    ```adan
    var a = 10 / 2; # a is Int, value 5
    var b = 10 / 3; # b is Float, value 3.3333...
    ```
- `%`: Modulo (or Remainder), the leftovers after division. If a number doesn't divide evenly, you'll be leftover with a non-zero value. The result takes the *sign of the LHS*.

    ```adan
    var a = 7 % 3;   # a is 1
    var b = -7 % 3;  # b is -1, sign follows the LHS
    ```
- `//`: Floor Division, regular division except you're rounding the outcome *down* to the nearest whole number.

    ```adan
    var a = 7 // 2; # a is 3
    ```

### Comparator Binops

- `==`: Equality, checking the *value* equality between the LHS and RHS value. To be equal, they must share the same *type* and *value*.
- `!=`: Inequality, the opposite of equality. Checks if the LHS and RHS values are *not* equal. Returns false if the types *and* values are the same.
- `<=`: Less Than or Equals, returns truthiness depending on if the LHS value is smaller than or equivalent to the RHS value.
- `>=`: Greater Than or Equals, returns truthiness depending on if the LHS value is *greater* than or equivalent to the RHS value.
- `<`: Less Than, equivalent to `<=`, with the exception of returning false if the values are equivalent.
- `>`: Greater Than, the mirror of `<`, returning truthiness depending on if the LHS value is only *greater* than the RHS.

### Logical Binops

- `and`: Returns `true` only if both the LHS and RHS are truthy.
- `or`: Returns `true` if either the LHS or RHS is truthy.

A value is considered **truthy** if it is anything other than `None`. `Bool` values are the one exception to this, and simply evaluate as themselves, `true` stays `true`, and `false` stays `false`.

```adan
var a = 1;

if a and true {
    # Runs, since `a` isn't None and the RHS is true.
}

if false and a {
    # Doesn't run, `false` evaluates as itself, not as truthy.
}
```

### Compound Binops

- `=`: Equals, different from equality validation, assigns the LHS value to *become* equal to the RHS value, as long as they share equivalent types. Used to reassign an already-declared variable, not to declare a new one.
- `+=`: Addition + Equals, directly equivalent to the expression: `LHS = LHS + RHS`.
- `-=`: Subtraction + Equals, directly equivalent to the expression: `LHS = LHS - RHS`.
- `*=`: Multiplication + Equals, directly equivalent to the expression: `LHS = LHS * RHS`.
- `/=`: Division + Equals, directly equivalent to the expression: `LHS = LHS / RHS`.
- `%=`: Modulo (or Remainder) + Equals, directly equivalent to the expression: `LHS = LHS % RHS`.
- `//=`: Floor Division + Equals, directly equivalent to the expression: `LHS = LHS // RHS`.

These also work on `global` variables directly, without needing to re-tag every operation:

```adan
global var d = 1;

global d += 1; # d is now 2
```

### None Handling Binops

- `?:`: Elvis, used like `LHS ?: RHS`. If LHS is `None`, the RHS is evaluated and returned instead. If LHS isn't `None`, the RHS is **never evaluated**, and LHS is used as-is.

```adan
func expensive(): Int {
    # Some costly computation, only run if actually needed.
    return 42;
}

var a: Int? = None;
var b = a ?: expensive(); # expensive() runs, b is 42

var c: Int? = 5;
var d = c ?: expensive(); # expensive() never runs, d is 5
```

### Operator Precedence

When an expression mixes multiple binops without parenthesis, ADAN resolves them in the following order, from highest to lowest priority, evaluating left-to-right within each tier:

1. Arithmetic (`*`, `/`, `%`, `//`, `+`, `-`)
2. Comparators (`==`, `!=`, `<=`, `>=`, `<`, `>`)
3. Logical (`and`, `or`)

```adan
1 + 2 * 3 == 7 and true # Reads as: ((1 + (2 * 3)) == 7) and true
```

Use parenthesis freely to make evaluation order explicit, especially in longer expressions.