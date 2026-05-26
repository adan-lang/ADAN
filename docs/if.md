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

// This will always evaluate to true.
if a {
    // This will always evaluate to false.
    if (a == b) {
        
    }

    // NOW, this will always evaluate to true.
    a = 2
    if a == b {

    }
}
```