## The `todo` Keyword
The `todo` keyword is a placeholder for code that hasn't been written yet. It borrows its behavior *directly* from Gleam's `todo`, combined with the idea of Python's `pass` keyword.

Unlike `pass` in the loops content (which skips to the *next iteration* of a loop), `todo` isn't loop-specific. The keyword can be dropped anywhere an expression or statement body is expected, most commonly inside function bodies you haven't implemented yet and don't want to fuss over compiler issues.

```adan
# Syntax

func sample_func() {
    todo;
}
```

Practically, it works as a stub to keep code clean an consise without worrying about compiler-time issues.

- **During compile/parse time**: `todo` is treated as syntactically valid, so your program compiles *and* runs, even though the logic doesn't quite exist just yet.
- **At runtime**: if execution actually *reaches* a `todo`, the program will immediately panic and halt, providing an error telling you unimplemented code was reached.