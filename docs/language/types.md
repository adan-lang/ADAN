## Different Kinds of Types

There are various kinds of built-in types in ADAN, which consist of:

### Character-based Types

- String
- Char

### Number-based Types

- Number (Handles *both* Ints and Floating-point values automatically)
- Int
- Float

### Array Types

To create an array in ADAN, use the basic syntax: `<Type>[]`. This creates a simple 1-dimensional array.

This process can be stacked to create higher dimensions of arrays, such as `<Type>[][]`, or `<Type>[][][]`, and so on.

```adan
var a: Int[] = [1, 2, 3];
var b: Int[][] = [[1, 2], [3, 4]];
```

### Other Types

- Bool
- None
- Void
- Auto