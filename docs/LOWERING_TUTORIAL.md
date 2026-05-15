# Complete Lowering Folder Implementation Tutorial

## Part 0: Understanding the Big Picture

### What is "Lowering"?

In a compiler, the **lowering phase** converts high-level code (your AST) into a lower-level representation (IR). Think of it like this:

```
Your ADAN Code:
  function add(a: number, b: number):
      return a + b

    ↓ (Parser creates AST)

Abstract Syntax Tree (AST):
  FunctionDefStmt
    ├─ Name: "add"
    ├─ Parameters: [a, b]
    └─ ReturnExpr: BinaryExpr(+, IdentifierExpr(a), IdentifierExpr(b))

    ↓ (Lowering converts to IR)

Intermediate Representation (IR):
  Block 0:
    %0 = LOAD a              (load variable 'a' into temp 0)
    %1 = LOAD b              (load variable 'b' into temp 1)
    %2 = BINARY %0, %1, "+"  (add temps 0 and 1, store in temp 2)
    RET %2                   (return the result)
```

### Why Lower to IR?

1. **Optimization**: IR is easier to optimize (dead code removal, constant folding, etc.)
2. **Backend Independence**: Same IR can target different backends
3. **Control Flow Analysis**: IR uses basic blocks, making control flow explicit
4. **Type Preservation**: We keep type information during transformation

### The Files You'll Implement

| File | Purpose | Complexity |
|------|---------|-----------|
| `slot_alloc.cpp` | Manages stack variable slots | ⭐ (Easiest) |
| `reader.cpp` | Traverses AST nodes | ⭐⭐ |
| `emitter.cpp` | Creates IR instructions | ⭐⭐ |
| `function.cpp` | Converts function definitions | ⭐⭐⭐ |
| `lowering.cpp` | Main orchestrator | ⭐⭐⭐ |
| `patch.cpp` | Post-processing fixes | ⭐⭐⭐⭐ |

---

## Part 1: Understanding Key Data Structures

### 1.1 IRType (from `/src/middlend/ir/types.py`)

This enum represents all possible types in IR:
```python
class IRType(Enum):
    NUMBER = auto()    # Floats and integers
    STRING = auto()    # Text
    BOOL = auto()      # true/false
    NONE = auto()      # "none" keyword
    NIL = auto()       # "nil" keyword
    UNKNOWN = auto()   # Type we don't know yet
```

**Why?** Every value in IR needs to know its type for optimization and bytecode generation.

### 1.2 Opcode (from `/src/middlend/ir/instr.py`)

These are the "operations" your IR can perform:

```python
class Opcode(Enum):
    CONST = auto()     # Load a constant: %0 = CONST 42
    COPY = auto()      # Copy value: %1 = COPY %0
    BINARY = auto()    # Binary op: %2 = BINARY %0, %1, "+"
    UNARY = auto()     # Unary op: %1 = UNARY %0, "-"
    CALL = auto()      # Function call: %0 = CALL func_name(args)
    RET = auto()       # Return: RET %0
    BRANCH = auto()    # If: BRANCH %0, true_label, false_label
    JUMP = auto()      # Goto: JUMP label
    LOAD = auto()      # Load variable: %0 = LOAD x
    STORE = auto()     # Store variable: STORE x, %0
    INTERP = auto()    # String interpolation: %0 = INTERP parts
    NOP = auto()       # No operation
```

### 1.3 Instr Class

An instruction is a single IR operation:

```python
class Instr:
    op: Opcode          # Which operation? (CONST, LOAD, etc.)
    dst: Optional[Temp] # Where does result go? (%0, %1, etc.)
    operands: List[IRValue]  # What are the inputs?
    extra: any          # Extra data (operator string, labels, etc.)
```

**Example**: `%2 = BINARY %0, %1, "+"`
- `op = Opcode.BINARY`
- `dst = Temp(2)`
- `operands = [Temp(0), Temp(1)]`
- `extra = "+"`

### 1.4 IRValue Types (from `/src/middlend/ir/value.py`)

Values in IR can be:

```python
class IRValue:
    """Base class for any value"""
    pass

class Const(IRValue):
    """Literal value: 42, "hello", true"""
    value: any
    type: IRType

class Temp(IRValue):
    """Temporary variable: %0, %1, %2"""
    id: int
    type: IRType

class Var(IRValue):
    """Named variable: x, y, result"""
    name: str
    type: IRType
```

### 1.5 BasicBlock

A sequence of instructions with a label. Used for control flow:

```
Block "entry":
  %0 = LOAD x
  %1 = CONST 0
  BRANCH %0, "greater", "not_greater"

Block "greater":
  %2 = CONST 1
  JUMP "end"

Block "not_greater":
  %2 = CONST 0

Block "end":
  RET %2
```

---

## Part 2: Step-by-Step Implementation

### STEP 1: Implement `slot_alloc.cpp` (EASIEST)

**Purpose**: Track which variable names are stored at which stack slot.

**What it does**:
- When you see variable `x` for the first time, assign it slot 0
- Next new variable `y` gets slot 1
- If you see `x` again, you know it's at slot 0

**Implementation**:

```cpp
// slot_alloc.cpp
#include "slot_alloc.hpp"

int SlotAllocator::Alloc(const std::string &name) {
    // If this name already has a slot, return it
    if (Has(name)) {
        return Get(name);
    }
    
    // Otherwise, assign it a new slot
    int slot = _next;
    _slots[name] = slot;
    _next++;
    return slot;
}

int SlotAllocator::Get(const std::string &name) const {
    auto it = _slots.find(name);
    if (it != _slots.end()) {
        return it->second;
    }
    return -1;  // Not found
}

bool SlotAllocator::Has(const std::string &name) const {
    return _slots.find(name) != _slots.end();
}

int SlotAllocator::Count() const {
    return _slots.size();
}

void SlotAllocator::Reset() {
    _slots.clear();
    _next = 0;
}
```

**How to test it**:
```cpp
SlotAllocator alloc;
int slot_x = alloc.Alloc("x");  // Returns 0
int slot_y = alloc.Alloc("y");  // Returns 1
int slot_x_again = alloc.Alloc("x");  // Returns 0 (same variable)
assert(slot_x == slot_x_again);
```

---

### STEP 2: Create Header Files (`reader.hpp`, `emitter.hpp`, `function.hpp`)

Before implementing the C++ files, create their header files:

**reader.hpp**:
```cpp
#pragma once

#include <memory>
#include "../../../middlend/ir/builder.py"  // Note: Python/C++ bridge
// This needs Python type definitions from Python frontend

class Reader {
public:
    // Reads an AST node and returns an IRValue
    // Will implement in reader.cpp
};
```

**emitter.hpp**:
```cpp
#pragma once

#include "../../../middlend/ir/builder.py"

class Emitter {
public:
    // Emits IR instructions
    // Will implement in emitter.cpp
};
```

**function.hpp**:
```cpp
#pragma once

#include "../../../middlend/ir/builder.py"

class FunctionLowering {
public:
    // Converts function AST nodes to IR functions
    // Will implement in function.cpp
};
```

---

### STEP 3: Implement `reader.cpp` (MEDIUM)

**Purpose**: Read an AST node and figure out what type it is, then emit the right IR code.

**What it needs to do**:
- Take an AST expression node
- Figure out what kind of expression it is (literal, identifier, binary, etc.)
- Return an IRValue (either a Const or Temp)

**Pseudocode structure**:
```cpp
IRValue ReadExpression(ASTNode* expr) {
    if (expr is a literal) {
        return EmitConst(expr.value)
    }
    else if (expr is an identifier) {
        return EmitLoad(expr.name)
    }
    else if (expr is a binary operation) {
        left = ReadExpression(expr.left)
        right = ReadExpression(expr.right)
        return EmitBinary(expr.op, left, right)
    }
    else if (expr is a function call) {
        args = ReadExpressions(expr.arguments)
        return EmitCall(expr.function_name, args)
    }
    // ... etc
}
```

**Implementation skeleton**:
```cpp
// reader.cpp
#include "reader.hpp"

// Helper to determine AST node type and dispatch
IRValue Reader::ReadExpression(ASTNode* expr, Emitter& emit) {
    // Pseudocode - actual implementation depends on AST structure
    
    // Check if it's a LiteralExpr
    if (auto lit = dynamic_cast<LiteralExpr*>(expr)) {
        return emit.Const(lit->Value, InferType(lit->Value));
    }
    
    // Check if it's an IdentifierExpr
    if (auto id = dynamic_cast<IdentifierExpr*>(expr)) {
        return emit.Load(id->Name);
    }
    
    // Check if it's a BinaryExpr
    if (auto binop = dynamic_cast<BinaryExpr*>(expr)) {
        IRValue left = ReadExpression(binop->Left, emit);
        IRValue right = ReadExpression(binop->Right, emit);
        return emit.Binary(binop->Operator, left, right);
    }
    
    // ... more cases for CallExpr, InterpolationExpr, etc.
}
```

---

### STEP 4: Implement `emitter.cpp` (MEDIUM)

**Purpose**: Provide convenient methods to create IR instructions.

**Implementation skeleton**:
```cpp
// emitter.cpp
#include "emitter.hpp"

// Wraps IRBuilder methods with C++ convenience
class Emitter {
private:
    IRBuilder builder;

public:
    Temp Const(const std::string& value, IRType type) {
        // Create a Const IRValue and emit a CONST instruction
        return builder.EmitConst(value, type);
    }
    
    Temp Load(const std::string& var_name) {
        // Emit a LOAD instruction for a variable
        return builder.EmitLoad(var_name);
    }
    
    void Store(const std::string& var_name, IRValue value) {
        // Emit a STORE instruction
        builder.EmitStore(var_name, value);
    }
    
    Temp Binary(const std::string& op, IRValue left, IRValue right) {
        // Infer result type from operator
        IRType result_type = InferBinaryType(op, left.type, right.type);
        return builder.EmitBinary(op, left, right, result_type);
    }
    
    // ... more convenience methods
};
```

---

### STEP 5: Implement `function.cpp` (HARD)

**Purpose**: Convert a FunctionDefStmt (AST) into an IRFunction (IR).

**High-level algorithm**:

```
1. Create an IRFunction with the function name
2. Create a basic block for the function body
3. For each parameter, allocate a slot
4. For each statement in the body:
   - If it's an assignment, read the expression and store it
   - If it's an if statement, create blocks for true/false branches
   - If it's an expression statement, just read the expression
5. For the return statement:
   - Read the return expression
   - Emit a RET instruction with that value
```

**Implementation skeleton**:

```cpp
// function.cpp
#include "function.hpp"

IRFunction* FunctionLowering::Lower(FunctionDefStmt* func_stmt) {
    // Create IR function with name and parameters
    std::vector<Var> params;
    for (auto& param : func_stmt->Parameters) {
        params.push_back(Var(param.name, FromStr(param.type)));
    }
    
    IRType return_type = FromStr(func_stmt->ReturnTypes[0]);
    auto ir_func = builder.BeginFunction(func_stmt->Name, params, return_type);
    
    // Create entry block
    auto entry_block = builder.BeginBlock("entry");
    
    // Lower each statement
    for (auto& stmt : func_stmt->Body) {
        LowerStatement(stmt);
    }
    
    // Lower return statement
    IRValue ret_val = ReadExpression(func_stmt->ReturnExpr);
    builder.EmitRet(ret_val);
    
    return ir_func;
}

void FunctionLowering::LowerStatement(Statement* stmt) {
    if (auto assign = dynamic_cast<AssignmentStmt*>(stmt)) {
        IRValue val = ReadExpression(assign->Value);
        builder.EmitStore(assign->Name, val);
    }
    else if (auto if_stmt = dynamic_cast<IfStmt*>(stmt)) {
        // Create blocks for true and false branches
        auto true_block = builder.BeginBlock("if_true");
        auto false_block = builder.BeginBlock("if_false");
        auto end_block = builder.BeginBlock("if_end");
        
        // Emit branch instruction
        IRValue cond = ReadExpression(if_stmt->Condition);
        builder.EmitBranch(cond, "if_true", "if_false");
        
        // Lower true branch
        builder.SetBlock(true_block);
        for (auto& s : if_stmt->ThenBranch) {
            LowerStatement(s);
        }
        builder.EmitJump("if_end");
        
        // Lower false branch (if exists)
        builder.SetBlock(false_block);
        if (if_stmt->ElseBranch) {
            for (auto& s : *if_stmt->ElseBranch) {
                LowerStatement(s);
            }
        }
        builder.EmitJump("if_end");
        
        builder.SetBlock(end_block);
    }
}
```

---

### STEP 6: Implement `lowering.cpp` (HARD)

**Purpose**: Main entry point. Takes a whole program (AST) and converts it all to IR.

**Algorithm**:

```
1. Create an IRModule (represents entire program)
2. For each statement in the program:
   - If it's a FunctionDefStmt, lower it using FunctionLowering
   - If it's an assignment (global), create and lower it
3. Return the populated IRModule
```

**Implementation skeleton**:

```cpp
// lowering.cpp
#include "lowering.hpp"

IRModule* Lowering::Lower(Program* ast) {
    auto module = new IRModule();
    
    for (auto& stmt : ast->Statements) {
        if (auto func = dynamic_cast<FunctionDefStmt*>(stmt)) {
            FunctionLowering func_lower(builder);
            func_lower.Lower(func);
        }
        else if (auto assign = dynamic_cast<AssignmentStmt*>(stmt)) {
            // Global assignment - store in module-level scope
            Reader reader;
            IRValue val = reader.ReadExpression(assign->Value);
            module->AddGlobalAssignment(assign->Name, val);
        }
    }
    
    return module;
}
```

---

### STEP 7: Implement `patch.cpp` (HARDEST)

**Purpose**: Post-processing. After IR is created, clean it up:

- Remove dead blocks
- Fix control flow issues
- Ensure all paths return a value
- Validate the IR

**Implementation skeleton**:

```cpp
// patch.cpp
#include "patch.hpp"

void Patcher::Patch(IRModule* module) {
    for (auto& func : module->Functions) {
        PatchFunction(func);
    }
}

void Patcher::PatchFunction(IRFunction* func) {
    RemoveUnreachableBlocks(func);
    ValidateControlFlow(func);
    ValidateTypeConsistency(func);
}

void Patcher::RemoveUnreachableBlocks(IRFunction* func) {
    // Starting from entry block, mark all reachable blocks
    // Delete blocks that weren't marked
}

void Patcher::ValidateControlFlow(IRFunction* func) {
    // Ensure:
    // - All blocks ending in branch/jump point to real blocks
    // - All code paths return a value
    // - No blocks with unreachable code after return
}
```

---

## Part 3: Building and Testing

### Building the C++ Code

The C++ files in `backend/lowering/` need to interface with Python. This requires:

1. **Pybind11 bindings** to expose C++ to Python
2. **Python/C API** calls from C++

### Testing Strategy

1. **Unit tests** for each file:
   ```
   test_slot_alloc.cpp
   test_emitter.cpp
   test_reader.cpp
   ```

2. **Integration test**:
   ```python
   # In main.py or test file
   from backend.lowering import Lowering
   
   ast = parser.parse("function add(a: number, b: number): return a + b")
   ir = Lowering().Lower(ast)
   
   # Check that IR looks correct
   assert len(ir.Functions) == 1
   assert ir.Functions[0].Name == "add"
   ```

---

## Part 4: Key Insights & Common Mistakes

### 1. **Type Inference**
Always infer types as you lower code:
```python
# In reader.py
if op == "+":
    # Both operands must be NUMBER
    if left.type != NUMBER or right.type != NUMBER:
        raise TypeError("Cannot add non-numbers")
    result_type = NUMBER
```

### 2. **Control Flow Graphs**
If statements create multiple basic blocks:
```
if_stmt:
  condition = read_expression()
  branch condition -> true_block, false_block
  
true_block:
  ... lower true statements ...
  jump -> end_block
  
false_block:
  ... lower false statements ...
  jump -> end_block
  
end_block:
  ... continue ...
```

### 3. **Temporaries vs Variables**
- **Temporaries** (`%0`, `%1`) are single-assignment (SSA-like)
- **Variables** (`x`, `y`) can be assigned multiple times
- Use LOAD/STORE for variables, temps for intermediate results

### 4. **Type Information**
Preserve type information throughout:
```cpp
// RIGHT:
Temp load_x = builder.EmitLoad("x", IRType.NUMBER);  // We know x is NUMBER

// WRONG:
Temp load_x = builder.EmitLoad("x");  // Loses type info!
```

---

## Part 5: Reference Architecture Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    AST from Frontend                        │
│         (FunctionDefStmt, AssignmentStmt, etc.)             │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
            ┌────────────────────────────┐
            │    Lowering::Lower()       │
            │  (lowering.cpp - main)     │
            └────────────────┬───────────┘
                             │
                  ┌──────────┴──────────┐
                  │                     │
                  ▼                     ▼
          ┌──────────────┐      ┌──────────────┐
          │ For each     │      │ For each     │
          │ Function: use│      │ Statement:   │
          │ FunctionLwr  │      │ use Reader   │
          └──────┬───────┘      └──────┬───────┘
                 │                     │
                 ▼                     ▼
         ┌──────────────────────────────────────┐
         │  Reader & Emitter (helper classes)   │
         │  - ReadExpression() traverses AST    │
         │  - Emitter creates IR instructions  │
         └────────────┬─────────────────────────┘
                      │
                      ▼
         ┌──────────────────────────────────┐
         │  IRBuilder (from middlend/)      │
         │  - BeginFunction()                │
         │  - BeginBlock()                   │
         │  - Emit*(...)  methods            │
         └────────────┬──────────────────────┘
                      │
                      ▼
         ┌──────────────────────────────────┐
         │  IRModule (collection of IR      │
         │  functions with basic blocks)    │
         └────────────┬──────────────────────┘
                      │
                      ▼
         ┌──────────────────────────────────┐
         │  Patcher::Patch() (patch.cpp)    │
         │  - Remove unreachable blocks     │
         │  - Validate control flow         │
         │  - Verify types                  │
         └────────────┬──────────────────────┘
                      │
                      ▼
         ┌──────────────────────────────────┐
         │  Finalized IR (ready for        │
         │  optimization and bytecode)     │
         └──────────────────────────────────┘
```

---

## Next Steps

1. **Start with `slot_alloc.cpp`** - It's simple and you'll feel successful
2. **Then do `emitter.cpp` and `reader.cpp`** - These are related
3. **Then `function.cpp`** - Combines the others
4. **Then `lowering.cpp`** - The orchestrator
5. **Finally `patch.cpp`** - Polish

Each step builds on the previous one. Don't skip around!

---

## Questions to Ask Yourself While Implementing

- "What AST node type am I looking at?"
- "What type does this value have?"
- "Which IR instruction should I emit?"
- "Where does the result go (which temporary)?"
- "For if statements, what blocks do I need?"

Good luck! 🚀
