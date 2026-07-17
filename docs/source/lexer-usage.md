## Using the Lexer

The lexer is the first stage of the ADAN compiler pipeline. It takes raw source text and converts it into a flat stream of `Token`s for the parser to consume.

### The `Token` Structure

Every token produced by the lexer carries the following fields:

```cpp
struct Token
{
    int pos;             // absolute character offset into the source, right after the token
    int line;             // 1-indexed line the token starts on
    int col;              // 1-indexed column the token starts on
    std::string lexeme;   // the exact source text that produced this token
    TokenType type;        // what kind of token this is
};
```

`TokenType` covers several categories, defined in `tokens.hh`:

- **Special**: `TOKEN_ERROR`, `TOKEN_EOF`, `TOKEN_IDENTIFIER`, `TOKEN_WHITESPACE`
- **Keywords**: `var`, `global`, `readonly`, `if`, `func`, `for`, `while`, `repeat`, `until`, `break`, `pass`, `in`, `include`, `using`, `export`, `todo`, `and`, `or`, `else`, `lambda`
- **Types**: `String`, `Char`, `Number`, `Int`, `Float`, `Bool`, `None`, `Void`, `Auto`
- **Symbols**: punctuation like `,` `:` `(` `)` `{` `}` `[` `]` `;` `'` `.` `?`, plus comments
- **Literals**: `TOKEN_INT_LITERAL`, `TOKEN_FLOAT_LITERAL`, `TOKEN_STRING_LITERAL`
- **Binary Operators**: `+` `-` `*` `**` `/` `%` `//` and their comparators/compound-assignment forms, plus `?:`
- **Unary Operators**: `!` `-` `!!`

Whitespace and comments are *not* discarded, they're emitted as `TOKEN_WHITESPACE`, `TOKEN_COMMENT`, and `TOKEN_MULTILINE_COMMENT` tokens in the stream. If you only care about meaningful tokens, filter these out yourself; keeping them in the stream is what makes the lexer usable for tooling that needs full source fidelity, such as formatters.

### Basic Usage

```cpp
#include "lexer.hh"

std::string source = "var x = 10;";

LexerType lt{source, /*pos=*/0, /*line=*/1, /*col=*/1, /*abort=*/false};
Lexer lexer(lt);

std::vector<Token> tokens = lexer.scan();

for (const Token& token : tokens)
{
    std::cout << token.line << ":" << token.col << " -> " << token.lexeme << "\n";
}
```

`LexerType` is a small struct that seeds the lexer's initial state. In almost all cases you'll want `pos = 0`, `line = 1`, `col = 1`, and `abort = false`, unless you're deliberately resuming a scan from a mid-file offset.

### Identifier Resolution Order

When the lexer encounters something that looks like an identifier (starts with a letter or `_`), it resolves the resulting lexeme in this order:

1. Check the `Keywords` table (e.g. `var`, `if`, `func`).
2. If not a keyword, check the `Types` table (e.g. `String`, `Int`, `None`).
3. If neither matches, it's emitted as a plain `TOKEN_IDENTIFIER`.

### Error Handling

The lexer doesn't throw exceptions. Recoverable issues (an unrecognized character, for example) emit a `TOKEN_ERROR` token and scanning continues normally. Unrecoverable issues (an unterminated string or an unterminated multiline comment) set an internal `abort` flag, print a diagnostic through `report()` (see `errors.hh`), and stop scanning early. Tokens found before the failure point remain in the returned vector.

```cpp
std::vector<Token> tokens = lexer.scan();
// Even on a fatal error, tokens produced before the failure are still present.
```

Diagnostics are printed directly to `stdout` with the source line, a caret span under the offending column range, and an optional `Help:` hint, using the same `report()` function shared by the rest of the compiler's error-reporting.

### Distinguishing Negation from Subtraction

`-` is context-sensitive. The lexer looks at the last non-trivial token (skipping over whitespace and comments) to decide whether `-` should be emitted as `TOKEN_SUB` (subtraction) or `TOKEN_NEGATE` (unary negation):

```
5 - 3     # SUB, previous token is a value
-5        # NEGATE, nothing meaningful comes before it
(a) - b   # SUB, previous token is a closing paren
f(a, -b)  # NEGATE, previous token is a comma
```

### Running the Built-In Test Suite

The lexer ships with a golden-output test suite. From a build of the `adan` binary:

```sh
adan --test
# or
adan -t
```

This runs every case defined in `tests/cases.cc` against `Lexer::scan()`, serializes the resulting token stream, and reports pass/fail per case.