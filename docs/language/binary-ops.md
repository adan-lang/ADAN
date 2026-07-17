## Binary Operators

Binary operators are kinds of operators that expect *both* a left and a right hand side value.

An example of a binary operator would be `+`. In addition, you need a value to add, and a value to add on to.

```
1 + 1
```

Binary operators are also called Binops for short, which is the name I'll be using from now on. As well as LHS and RHS representing the *left* and *right* hand side operands.

### Arithmetic / Math Binops

- `+`: Addition, LHS and RHS integers added together.
- `-`: Subtraction, the *opposite* of addition, you take away instead of combine.
- `*`: Multiplication, you're adding the LHS value n (RHS) times.
- `/`: Division, you're *subtracting* the LHS value n (RHS) times.
- `%`: Modulo (or Remainder), the leftovers after division. If a number doesn't divide evenly, you'll be leftover with non-zero value.
- `//`: Floor Division, regular devision except you're rounding the outcome *down* to the nearest whole number.

### Comparator Binops

- `==`: Equality, checking the *value* equality between the LHS and RHS value. To be equal, they must share the same *type* and *value*.
- `!=`: Inequality, the opposite of equality. Checks if the LHS and RHS values are *not* equal. Returns false if the types *or* values are not the same.
- `<=`: Less Than or Equals, returns truthiness depending on if the LHS value is smaller than or equivalent to the RHS value.
- `>=`: Greater Than or Equals, returns truthiness depending on if the LHS value is *greater* than or equivalent to the RHS value.
- `<`: Less Than, equivalent to `<=`, with the exception of returning false is the values are equivalent.
- `>`: Greater Than, the opposite of `>`, returning truthiness depending on if the value is only *greater* than the RHS.
- `and`: Returns truthiness when both of the LHS and RHS values share equivalent values *and* types.
- `or`: Returns truthiness when either value itself returns true.

### Compound Binops

- `=`: Equals, different from equality validation, assigns the LHS value to *become* equal to the RHS value, as long as they share equivalent types.
- `+=`: Addition + Equals, directly equivalent to the expression: `LHS = LHS + RHS`.
- `-=`: Subtraction + Equals, directly equivalent to the expression: `LHS = LHS - RHS`.
- `*=`: Multiplication + Equals, directly equivalent to the expression: `LHS = LHS * RHS`.
- `/=`: Division + Equals, directly equivalent to the expression: `LHS = LHS / RHS`.
- `%=`: Modulo (or Remainder) + Equals, directly equivalent to the expression: `LHS = LHS % RHS`.
- `//=`: Floor Division + Equals, directly equivalent to the expression: `LHS = LHS // RHS`.

### Null Handling Binops

- `?:`: Elvis, used like `LHS ?: RHS`. If LHS is null, return the RHS value instead, otherwise use LHS.