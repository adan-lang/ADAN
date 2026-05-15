# Lowering Example: Step-by-Step Walkthrough

This document shows **exactly** what happens when you lower a simple ADAN program, step by step.

---

## Example Program

```adan
function add(a: number, b: number):
    return a + b
```

---

## Step 1: The Parser Creates an AST

The parser (frontend) reads the code and creates:

```
FunctionDefStmt
  ├─ Name: "add"
  ├─ Parameters:
  │  ├─ Parameter("a", "number")
  │  └─ Parameter("b", "number")
  ├─ ReturnTypes: ["number"]
  └─ ReturnExpr: BinaryExpr
      ├─ Left: IdentifierExpr("a")
      ├─ Operator: "+"
      └─ Right: IdentifierExpr("b")
```

This is what `Lowering::Lower()` receives as input.

---

## Step 2: Lowering Begins - `Lowering::Lower()` is Called

In `lowering.cpp`:

```cpp
IRModule* module = new IRModule();
Emitter emitter(module);

// Program has one statement - a function definition
FunctionDefStmt* add_func = /* first statement */;

FunctionLowering func_lowering(emitter);
IRFunction* ir_add = func_lowering.Lower(add_func);

return module;
```

**State after this step**: 
- An empty `IRModule` exists
- We're about to lower the `add` function

---

## Step 3: Function Lowering Begins - `FunctionLowering::Lower()` is Called

In `function.cpp`:

```cpp
IRFunction* FunctionLowering::Lower(FunctionDefStmt* func_stmt) {
    // Extract parameters: "a: number", "b: number"
    std::vector<Var> params = {
        Var("a", IRType.NUMBER),
        Var("b", IRType.NUMBER)
    };
    
    // Create IR function
    IRFunction* ir_func = builder.BeginFunction(
        "add",
        params,
        IRType.NUMBER  // return type
    );
    
    // Create entry block
    BasicBlock* entry = builder.BeginBlock("entry");
    
    // The function body is empty (just the return)
    
    // Lower the return expression
    IRValue ret_val = Reader::ReadExpression(
        func_stmt->ReturnExpr,  // BinaryExpr(+, a, b)
        emitter
    );
    
    // Emit return
    emitter.EmitReturn(ret_val);
    
    return ir_func;
}
```

**State after this step**:
- `IRFunction` named "add" has been created
- Entry block exists but is empty
- Parameters are registered

---

## Step 4: Reading the Return Expression - `Reader::ReadExpression()` is Called

The return expression is: `BinaryExpr(+, a, b)`

In `reader.cpp`:

```cpp
IRValue Reader::ReadExpression(BinaryExpr* binop, Emitter& emitter) {
    // Step 4a: Read left operand (IdentifierExpr("a"))
    IRValue left = ReadExpression(
        binop->Left,  // IdentifierExpr("a")
        emitter
    );
    // This calls ReadExpression(IdentifierExpr) → proceeds to Step 4c
    
    // Step 4b: Read right operand (IdentifierExpr("b"))
    IRValue right = ReadExpression(
        binop->Right,  // IdentifierExpr("b")
        emitter
    );
    // This also calls ReadExpression(IdentifierExpr)
    
    // Step 4d: Emit binary operation
    return emitter.EmitBinary("+", left, right);
}
```

**Substeps**:

### Step 4a & 4b: Reading Identifiers `IdentifierExpr("a")` and `IdentifierExpr("b")`

```cpp
IRValue Reader::ReadExpression(IdentifierExpr* id, Emitter& emitter) {
    return emitter.EmitLoad(id->Name, IRType.NUMBER);
}
```

For `a`:
- Call `emitter.EmitLoad("a", IRType.NUMBER)`

In `emitter.cpp`:

```cpp
Temp Emitter::EmitLoad(const std::string& var_name, IRType type) {
    // Create a temporary to hold the loaded value
    Temp result = _nextTemp(type);  // Let's say this gives us %0
    
    // Emit the LOAD instruction
    Instr load_instr(
        Opcode.LOAD,
        dst = result,           // %0
        operands = [Var("a", NUMBER)],
        extra = null
    );
    builder.Emit(load_instr);
    
    return result;  // %0
}
```

**After LOAD a**:
```
Block "entry":
  %0 = LOAD a
```

Similarly, for `b`:

**After LOAD b**:
```
Block "entry":
  %0 = LOAD a
  %1 = LOAD b
```

### Step 4c: Emitting the Binary Operation

Back in `emitter.cpp`:

```cpp
Temp Emitter::EmitBinary("+", %0, %1) {
    // Determine result type
    // "+" with NUMBER, NUMBER → NUMBER
    IRType result_type = IRType.NUMBER;
    
    // Create result temporary
    Temp result = _nextTemp(result_type);  // %2
    
    // Emit the BINARY instruction
    Instr binary_instr(
        Opcode.BINARY,
        dst = result,          // %2
        operands = [%0, %1],
        extra = "+"
    );
    builder.Emit(binary_instr);
    
    return result;  // %2
}
```

**After BINARY**:
```
Block "entry":
  %0 = LOAD a
  %1 = LOAD b
  %2 = BINARY %0, %1, "+"
```

---

## Step 5: Emitting the Return - `emitter.EmitReturn()` is Called

Back in `function.cpp`, we have `ret_val = %2` (from Step 4).

```cpp
emitter.EmitReturn(%2);
```

In `emitter.cpp`:

```cpp
void Emitter::EmitReturn(const IRValue& value) {
    // Emit RET instruction
    Instr ret_instr(
        Opcode.RET,
        dst = null,
        operands = [value],  // [%2]
        extra = null
    );
    builder.Emit(ret_instr);
}
```

**After RET**:
```
Block "entry":
  %0 = LOAD a
  %1 = LOAD b
  %2 = BINARY %0, %1, "+"
  RET %2
```

---

## Step 6: Patching - `Patcher::Patch()` is Called

In `patch.cpp`:

```cpp
void Patcher::Patch(IRModule* module) {
    for (auto& func : module->Functions) {
        PatchFunction(func);
    }
}

void Patcher::PatchFunction(IRFunction* func) {
    RemoveUnreachableBlocks(func);
    ValidateControlFlow(func);
    ValidateTypes(func);
}
```

For our simple function:
- **RemoveUnreachableBlocks**: Only one block, and it's reachable → nothing to remove
- **ValidateControlFlow**: Block ends with RET → valid ✓
- **ValidateTypes**: RET value is NUMBER, return type is NUMBER ✓

**Final IR**:
```
function add(a: number, b: number) -> number
  Block "entry":
    %0 = LOAD a
    %1 = LOAD b
    %2 = BINARY %0, %1, "+"
    RET %2
```

---

## Visual Summary

```
┌─────────────────────────────────────────────────────────────┐
│                   ADAN Source Code                          │
│                                                             │
│  function add(a: number, b: number):                        │
│      return a + b                                           │
└────────────┬────────────────────────────────────────────────┘
             │ Parser creates AST
             ▼
┌─────────────────────────────────────────────────────────────┐
│                    AST Tree                                 │
│                                                             │
│  FunctionDefStmt(                                           │
│    name="add",                                              │
│    params=[a, b],                                           │
│    returnExpr=BinaryExpr(+, a, b)                          │
│  )                                                          │
└────────────┬────────────────────────────────────────────────┘
             │ Lowering::Lower()
             │
             ├─→ FunctionLowering::Lower()
             │   ├─→ Create IRFunction "add"
             │   ├─→ Reader::ReadExpression(BinaryExpr)
             │   │   ├─→ Reader::ReadExpression(IdentifierExpr a)
             │   │   │   └─→ Emit: %0 = LOAD a
             │   │   ├─→ Reader::ReadExpression(IdentifierExpr b)
             │   │   │   └─→ Emit: %1 = LOAD b
             │   │   └─→ Emit: %2 = BINARY %0, %1, "+"
             │   │       returns %2
             │   └─→ Emit: RET %2
             │
             └─→ Patcher::Patch() [validation only]
             │
             ▼
┌─────────────────────────────────────────────────────────────┐
│                   Final IR                                  │
│                                                             │
│  function add(a: number, b: number) -> number              │
│    Block "entry":                                           │
│      %0 = LOAD a                                            │
│      %1 = LOAD b                                            │
│      %2 = BINARY %0, %1, "+"                               │
│      RET %2                                                 │
└─────────────────────────────────────────────────────────────┘
```

---

## More Complex Example: If Statement

Now let's see how an if statement is lowered:

```adan
function sign(n: number):
    if n > 0:
        return 1
    else:
        return -1
```

### Step 1: Parser creates AST

```
FunctionDefStmt
  ├─ Name: "sign"
  ├─ Parameters: [n: number]
  └─ Body: [
       IfStmt(
         condition: BinaryExpr(>, n, 0),
         thenBranch: [ReturnStmt(1)],
         elseBranch: [ReturnStmt(-1)]
       )
     ]
```

### Step 2: Lowering creates blocks

`FunctionLowering::LowerIfStatement()` does this:

```cpp
void FunctionLowering::LowerIfStatement(IfStmt* if_stmt) {
    // Step 1: Create blocks
    BasicBlock* true_block = builder.BeginBlock("if_true");
    BasicBlock* false_block = builder.BeginBlock("if_false");
    BasicBlock* end_block = builder.BeginBlock("if_end");
    
    // Step 2: Read condition and branch
    IRValue cond = Reader::ReadExpression(if_stmt->Condition, emitter);
    // This emits: %0 = LOAD n, %1 = CONST 0, %2 = BINARY %0, %1, ">"
    
    emitter.EmitBranch(cond, "if_true", "if_false");
    // Emits: BRANCH %2, if_true, if_false
    
    // Step 3: Lower true branch
    builder.SetBlock(true_block);
    emitter.EmitReturn(Const(1, NUMBER));
    // Emits: %3 = CONST 1, RET %3
    
    // Step 4: Lower false branch
    builder.SetBlock(false_block);
    emitter.EmitReturn(Const(-1, NUMBER));
    // Emits: %4 = CONST -1, RET %4
    
    // Note: No jump to end_block because both branches return
}
```

### Step 3: Final IR

```
function sign(n: number):
  Block "entry":
    %0 = LOAD n
    %1 = CONST 0
    %2 = BINARY %0, %1, ">"
    BRANCH %2, if_true, if_false
  
  Block "if_true":
    %3 = CONST 1
    RET %3
  
  Block "if_false":
    %4 = CONST -1
    RET %4
```

Notice:
- Three separate blocks (one for each path)
- Condition evaluation happens in entry block
- Each branch path is separate
- No merging needed because both return

---

## Key Takeaways

1. **Lowering is recursive**: Reading expressions recursively emits IR as it goes
2. **Blocks are created for control flow**: If statements create multiple blocks
3. **Temporaries are unique**: Each operation gets a new temp (%0, %1, %2, ...)
4. **Values flow through IR**: The return value of one operation becomes input to the next
5. **Patching validates**: After lowering, the patcher checks everything is correct

Now you're ready to implement! Start with `slot_alloc.cpp` and work your way up.
