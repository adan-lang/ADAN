<div align="center">
    <h1>The ADAN Programming Language</h1>
    <img align="right" width="180" height="180" alt="image" src="./assets/logo.png" />
    <p align="left">
        A dynamically typed language using the <i>design of</i> Python and the <i>speed of</i> C++. <b>Made for focusing on problem solving over complexity</b>.
    </p>
</div>

<h3 align="left">
 <i>Simplify low level programming</i>.
</h3>

> [!WARNING]
> This project currently only supports the Linux operating system. Support for Windows and macOS is coming shortly after base compilation is finished.

## Preview

```adan
-- Variables
name  = "ADAN"
count = 42
flag  = true

-- Interpolation
message = `Hello, {name}!`

-- Functions
function add(a: number, b: number): -- Compiler assumes type: number
    return a + b

function greet(user: string) -> string:
    return `Hey, {user}!`

-- If / else
function sign(n: number) -> string:
    if n > 0:
        return "positive"
    else:
        return "negative"

-- Calls
result  = add(10, 32)
greeted = greet(name)
label   = sign(-5)
```

## How It's Built

- **Scanner**: Tokenizes raw `.adn` source into structured symbols  
- **Parser**: Builds a fully formed **AST (Abstract Syntax Tree)** from tokens  
- **Semantic Analyzer**: Enforces correctness by resolving identifiers, validating types, and ensuring control-flow integrity  

- **IR Lowering**: Converts the verified AST into a strongly typed, block-based **Intermediate Representation (IR)**  
- **Serialization**: Encodes IR into a module of basic blocks for downstream processing  

- **Optimization Pipeline**:
  - **C++ optimizer pass system**:
    - Constant folding  
    - Common subexpression elimination (CSE)  
    - Dead code elimination  
    - Jump threading  
  - **Python pass manager layer**:
    - Copy propagation  
    - Function inlining  
    - Unreachable block removal  

- **Bytecode Lowering**: Translates optimized IR into a flat, stack-based **bytecode format** stored in executable chunks  

- **Verifier**: Ensures bytecode correctness before execution  

- **Virtual Machine (VM)**: Executes bytecode using a frame-based loop and operand stack  

- **Runtime System**:
  - Handles arithmetic, comparisons, and dynamic dispatch  
  - Manages garbage-collected heap memory  
  - Provides built-in standard library functions 

### ASCII Diagram (Visual Representation)

```
+----------------------------+     +------------------------------+     +------------------------------+     +----------------------------------------------+
|      .adn SOURCE FILE      | --> |           SCANNER            | --> |            PARSER            | --> |              SEMANTIC ANALYZER               |
|                            |     | lexical analysis             |     | grammar parsing (AST build)  |     | symbol resolution + validation               |
| raw text program input     |     | tokens: keywords, ids, ops   |     | nodes: exprs, stmts, funcs   |     | type checking + control-flow verification    |
+----------------------------+     +------------------------------+     +------------------------------+     +----------------------------------------------+
                                                                                          |
                                                                                          v
+----------------------------------------------+     +----------------------------------------------+     +----------------------------------------------+
|                 IR LOWERING                  | --> |              IR SERIALIZATION                | --> |            OPTIMIZATION PIPELINE             |
| AST → typed, block-based IR                  |     | IR → module of basic blocks (JSON format)    |     | multi-stage optimization system              |
| SSA-like structure, control-flow graph       |     | preserves typing + structure                 |     |                                              |
+----------------------------------------------+     +----------------------------------------------+     | C++ optimizer:                               |
                                                                                                          |            | - constant folding              |
                                                                                                          |            | - CSE                           |
                                                                                                          |            | - dead code elimination         |
                                                                                                          |            | - jump threading                |
                                                                                                          | Python pass manager:                         |
                                                                                                          |            | - inlining                      |
                                                                                                          |            | - copy propagation              |
                                                                                                          |            | - unreachable block removal     |
                                                                                                          +----------------------------------------------+
                                                                                                                                 |
                                                                                                                                 v
+----------------------------------------------+     +----------------------------------------------+     +----------------------------------------------+
|              BYTECODE LOWERING               | --> |                  VERIFIER                    | --> |               VIRTUAL MACHINE                |
| IR → stack-based instruction set             |     | ensures bytecode structural correctness      |     | frame-based execution loop                   |
| chunks of linear opcodes                     |     | validates control flow + stack safety        |     | operand stack + call frames                  |
+----------------------------------------------+     +----------------------------------------------+     +----------------------------------------------+
                                                                                                                                 |
                                                                                                                                 v
                                                                                                          +----------------------------------------------+
                                                                                                          |                   RUNTIME                    |
                                                                                                          | dynamic execution layer                      |
                                                                                                          | - arithmetic + comparisons                   |
                                                                                                          | - garbage-collected heap system              |
                                                                                                          | - built-in standard library                  |
                                                                                                          | - native dispatch + system ops               |
                                                                                                          +----------------------------------------------+
```