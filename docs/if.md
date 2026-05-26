When constructing an if statement, they can be written with or without parenthesis, like the following suggests:

```adan
if (x) {

}

if x {
    
}
```

Expressions in ADAN follow the [order of operations](https://en.wikipedia.org/wiki/Order_of_operations).

- For an if statement to go through, the result of the expression should equate to `true`.

```adan
var a = 1;
var b = 2;

# This'll always be true.
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