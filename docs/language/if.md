## Creating an If Statement

When constructing an if statement, they can be written with or without parenthesis, like the following suggests:

```adan
if (x) {

}

if x {

}
```

Expressions in ADAN follow the [order of operations](https://en.wikipedia.org/wiki/Order_of_operations).

- For an if statement to pass, the result of the expression must be **truthy**, meaning anything other than `None`. `Bool` values are the exception, and evaluate as themselves. See [Binary Operators](./binary-ops.md#logical-binops) for the full rule.

```adan
var a = 1;
var b = 2;

# This'll always be true, `a` is 1, which isn't None.
if a {

    # This'll always be false.
    if (a == b) {

    }

    # This'll now be true.
    a = 2;
    if a == b {

    }
}
```