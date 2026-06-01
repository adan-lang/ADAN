## Various Kinds of Loops

There are various kinds of loops supported in the ADAN programming language.

All of generally server the same purpose, *to iterate over a loop*, but the way they accomplish this task, and the results they provide are different.

### For-in Loop

A for-in loop is a kind of loop that can optionally save the `key` *and* the `value` from a loop, or one of the two (interchangeable).

```adan
# Syntax

## <...>? = <...> (local variable) OR _ (no variable)

for <key>?, <value>? in <loop> {
    ...
}
```