<div align="center">
   <h1>ADAN</h1>
   <img alt="Version" src="https://img.shields.io/badge/version-1.0.a-blue">
   <img alt="Latest Release" src="https://img.shields.io/github/v/release/transicle/adan">
   <img alt="Docs" src="https://img.shields.io/badge/docs-docs.adan.sh-blue">
   <br><br>
   <p>
      Beginner friendly language with inferred typing and no memory managing.
   </p>
</div>

## What is ADAN?

ADAN is a general purpose programming language built around the idea that low level work should not require low level complexity. It gives you the readability and simplicity of a high level language while still being capable enough for jobs that typically demand something closer to the hardware.

## Features

- **Inferred typing** -- types are resolved at compile time without requiring you to write them everywhere
- **Readable syntax** -- borrows the clarity of Python with the structure of a curly-brace language
- **Module system** -- explicit exports keep your code organized and your namespaces clean
- **Multiple loop forms** -- for-in, entry-controlled, while, and repeat-until loops out of the box
- **Null handling** -- built-in null coalescing and not-null assertion operators
- **Global scope modifier** -- promote a variable to global scope from anywhere using the `global` keyword
- **Lambdas** -- anonymous single-expression functions for quick inline logic
- **Variadic functions** -- accept any number of arguments with the `...` parameter

## Quick Look

At its simplest, ADAN stays out of your way.

```adan
var name = "world";
var message = "Hello, " + name;
```

Functions are straightforward, and types are only written when you need them.

```adan
func add(a, b) return a + b;

func greet(name: String): String {
    return "Hello, " + name;
}
```

As things get more involved, the language scales with you.

```adan
using module std.io;
using module std.fs;

export func readLines(path: String): String[] {
    var file = fs.open(path);
    var lines: String[] = [];

    for _, line in file.lines() {
        if line != "" {
            lines += line;
        }
    }

    return lines;
}
```

And at the more complex end, you have the full picture: modules, exports, typed signatures, loops, and null handling, all still readable.

```adan
using module std.io;
using module std.net;

readonly var MAX_RETRIES = 3;

func fetchWithRetry(url: String, retries: Int): String {
    var response: String = None;
    var attempts = 0;

    repeat {
        response = net.get(url);
        attempts += 1;
    } until response != None or attempts >= retries;

    return response ?: "no response";
}

export func main() {
    var result = fetchWithRetry("https://adan.sh/api/version", MAX_RETRIES);
    io.print(result);
}
```

## Status

ADAN is actively in development. Things will change, break, and improve. If you want to follow along with development in real time, streams are live on [YouTube](https://www.youtube.com/@transicle/streams) ***!!***

> **Note:** 0 AI is being used for the code, only used for small research.