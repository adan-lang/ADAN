## Various Kinds of Loops

There are various kinds of loops supported in the ADAN programming language.

All of them generally serve the same purpose, *to iterate*, but the way they accomplish this task, and the results they provide, are different.

### For-in Loop

A for-in loop is a kind of loop that can optionally save the `key` *and* the `value` from a loop, or just one of the two (interchangeable).

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

> If we set it to `3` for example, our loop would *start* at the *fourth* index of the array.

```adan
# Syntax

for <Init>?; <Condition>?; <Iterate --/++>? {
    ...
}
```

If every field is left empty (`for ; ; ; { ... }`), the loop runs indefinitely.

### While Loops

While loops, by design, repeat indefinitely while the condition provided is truthy. The condition is checked *before* each iteration, once it's falsy, the loop breaks and never repeats again.

```adan
# Syntax

while <Condition> {
    ...
}
```

### Exit-Controlled Loop

An exit-controlled loop is the opposite of a while loop in terms of timing. Instead of checking the condition *before* running the body, it runs the body *first*, and only checks the condition *afterward* to decide whether to continue. This guarantees the body runs at least once.

```adan
# Syntax

repeat {
    ...
} until <Condition>;
```

## Loop-Specific Keywords

Inside loops, sometimes you can't programmatically write what you want without exiting early or skipping to the next iteration.

- To *stop* a loop early, use the `break` keyword. This stops the loop entirely, disregarding any future code after it.
- To *skip* to the next iteration without stopping the loop, use the `pass` keyword. This disregards any remaining code in the current iteration, but keeps the loop running for all future iterations.