## Various Kinds of Loops

There are various kinds of loops supported in the ADAN programming language.

All of which generally serve the same purpose, *to iterate over a loop*, but the way they accomplish this task, and the results they provide are different.

### For-in Loop

A for-in loop is a kind of loop that can optionally save the `key` *and* the `value` from a loop, or one of the two (interchangeable).

```adan
# Syntax

## <...>? = <...> (local variable) OR _ (no variable)

for <Key>?, <Value>? in <Array> {
    ...
}
```

### Entry-Controlled For Loop

An entry-controlled for loop is a type of loop that allows you to change the starting position in the array.

For example, we can do: `for var i = 0; ...; ... { ... }`, where `0` is the position we start at in the array.

> If we set it to `3` for example, our loop would *start* in the *fourth* index of the array.

```adan
# Syntax

for <Init>?; <Condition>?; <Iterate --/++>? {
    ...
}
```

If every field is left empty (for ; ; ; { ... }), the loop would run indefinitely.

### While Loops

While loops by design, repeat indefinitely while the condition provided equates to true. Once false, the loop will break, never repeating again.

```adan
# Syntax

while <Condition> {
    ...
}
```

### Exit-Controlled Loop

Unlike entry-controlled for loops, exit-controlled loops are styled similarly to while loops, where they check the condition first, *then*, and only if true, they run the code, exit-controlled loops run the code *first*, ***and then*** check the condition to validate if it should continue.

```adan
# Syntax

repeat {
    ...
} until <Condition>;
```