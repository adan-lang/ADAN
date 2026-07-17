## Unary Operators

Unary operators work on a single value.

- `!`: logical not, flips a `Bool` value between `true` and `false`.
- `-`: negation, turns a number into its negative form.
- `!!`: not-none assertion, forces a value to be non-`None` and errors if it is `None`.

```adan
var a = true;
var b = !a; # b is false

var c = 5;
var d = -c; # d is -5

var e: Int? = 10;
var f = e!!; # f is 10, an Int (no longer optional)

var g: Int? = None;
var h = g!!; # errors, g is None
```