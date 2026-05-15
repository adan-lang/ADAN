# Lowering Folder - Quick Reference Cheat Sheet

## File Dependency Order (Implement in This Order)

```
1. slot_alloc.cpp       ✓ No dependencies
   ↓
2. reader.hpp + emitter.hpp (headers only)
   ↓
3. emitter.cpp          ← Uses IRBuilder
   ↓
4. reader.cpp           ← Uses Emitter
   ↓
5. function.hpp + lowering.hpp (headers)
   ↓
6. function.cpp         ← Uses Reader & Emitter
   ↓
7. lowering.cpp         ← Uses FunctionLowering
   ↓
8. patch.cpp            ← Uses IRModule & IRFunction
```

---

## One-Page Reference

### Key Classes

| Class | Purpose | Key Methods |
|-------|---------|------------|
| **Emitter** | Creates IR instructions | `EmitConst()`, `EmitLoad()`, `EmitBinary()`, `EmitBranch()` |
| **Reader** | Traverses AST | `ReadExpression()`, `InferLiteralType()` |
| **FunctionLowering** | Converts functions | `Lower()`, `LowerStatement()` |
| **Lowering** | Main orchestrator | `Lower(Program*)` |
| **Patcher** | Validates IR | `Patch()`, `ValidateControlFlow()` |
| **SlotAllocator** | Tracks variables | `Alloc()`, `Get()`, `Has()` |

### Common Patterns

**Reading an Expression and Emitting IR:**
```cpp
// Pattern: Read left, read right, emit operation
IRValue left = Reader::ReadExpression(expr->Left, emitter);
IRValue right = Reader::ReadExpression(expr->Right, emitter);
IRValue result = emitter.EmitBinary("+", left, right);
```

**Creating a Block:**
```cpp
BasicBlock* block = builder.BeginBlock("block_name");
// ... emit instructions ...
builder.SetBlock(other_block);  // Switch to another block
```

**Emitting Instruction:**
```cpp
emitter.EmitLoad("variable_name", IRType.NUMBER);
emitter.EmitBinary("+", temp1, temp2);
emitter.EmitBranch(condition, "true_label", "false_label");
```

### Type System

```python
IRType.NUMBER   → Floats and integers
IRType.STRING   → Text (quoted strings)
IRType.BOOL     → true/false
IRType.NONE     → "none" keyword
IRType.NIL      → "nil" keyword
IRType.UNKNOWN  → Type not yet determined
```

### Instruction Types

```python
CONST   → %0 = CONST 42
COPY    → %1 = COPY %0
LOAD    → %0 = LOAD x
STORE   → STORE x, %0
BINARY  → %2 = BINARY %0, %1, "+"
UNARY   → %1 = UNARY %0, "-"
CALL    → %2 = CALL func(%0, %1)
BRANCH  → BRANCH %0, true_label, false_label
JUMP    → JUMP label
RET     → RET %0
INTERP  → %0 = INTERP parts
NOP     → NOP
```

---

## Testing Checklist

- [ ] Compile without errors
- [ ] `test_slot_alloc.cpp` passes
- [ ] `test_emitter.cpp` creates correct IR instructions
- [ ] `test_reader.cpp` reads expressions correctly
- [ ] `test_function.cpp` lowers simple functions
- [ ] `test_lowering.cpp` lowers complete programs
- [ ] `test_patch.cpp` catches invalid IR

---

## Common Mistakes to Avoid

❌ **Forgetting type information:**
```cpp
// WRONG: Type is lost
Temp result = emitter.EmitLoad("x");

// RIGHT: Type is preserved
Temp result = emitter.EmitLoad("x", IRType.NUMBER);
```

❌ **Not setting the current block before emitting:**
```cpp
// WRONG: Emitting without a current block
emitter.EmitConst("42", IRType.NUMBER);

// RIGHT: Ensure block is set
builder.BeginBlock("entry");
emitter.EmitConst("42", IRType.NUMBER);
```

❌ **Forgetting to jump between blocks in if statements:**
```cpp
// WRONG: No connection between blocks
builder.SetBlock(true_block);
emitter.EmitConst("1", IRType.NUMBER);
builder.SetBlock(false_block);  // Hanging connection!

// RIGHT: Jump connects blocks
builder.SetBlock(true_block);
emitter.EmitConst("1", IRType.NUMBER);
emitter.EmitJump("end_block");
builder.SetBlock(false_block);
```

❌ **Type mismatch in operations:**
```cpp
// WRONG: Trying to add a string to a number
emitter.EmitBinary("+", Var("x", STRING), Var("y", NUMBER));

// RIGHT: Both operands have same type
emitter.EmitBinary("+", Var("x", NUMBER), Var("y", NUMBER));
```

❌ **Not inferring return type for operations:**
```cpp
// WRONG: Result type is unknown
Temp result = builder.EmitBinary("+", left, right, IRType.UNKNOWN);

// RIGHT: Infer from operator and operands
Temp result = emitter.EmitBinary("+", left, right);  // Infers NUMBER
```

---

## Debugging Tips

### 1. Print IR as it's Generated
```cpp
std::cout << "Emitting: " << instr << std::endl;
```

### 2. Trace Through a Simple Example
Use the example program:
```adan
function add(a: number, b: number):
    return a + b
```
Step through lowering manually, writing down what IR is created.

### 3. Check AST Structure
Before implementing reader, print out the AST to see what you're reading:
```cpp
std::cout << "AST Node: " << ast_node << std::endl;
```

### 4. Validate Each Step
After each file, test it:
- SlotAllocator: Can allocate slots?
- Emitter: Does it create valid instructions?
- Reader: Does it traverse expressions correctly?

### 5. Use Unit Tests
Test small pieces in isolation:
```cpp
TEST(EmitterTest, EmitConst) {
    Emitter emitter;
    Temp result = emitter.EmitConst("42", IRType.NUMBER);
    EXPECT_EQ(result.type, IRType.NUMBER);
}
```

---

## File Size Expectations

Rough line counts (don't expect exact, these are guidelines):

- `slot_alloc.cpp`: 30-50 lines
- `emitter.cpp`: 100-150 lines
- `reader.cpp`: 150-250 lines
- `function.cpp`: 150-250 lines
- `lowering.cpp`: 50-100 lines
- `patch.cpp`: 200-300 lines

Total: ~700-1100 lines of actual implementation code

---

## When You Get Stuck

1. **Can't figure out AST structure?**
   - Look at `/src/frontend/ast/` files
   - Check what the parser produces

2. **Don't know which instruction to emit?**
   - Look at `/src/middlend/ir/instr.py` for available opcodes
   - Look at `IRBuilder` methods for examples

3. **Type inference not working?**
   - Check what the semantic analyzer determined
   - Look at how types flow through the AST

4. **Control flow wrong for if statements?**
   - Draw out blocks on paper first
   - Make sure jumps connect blocks

5. **Compilation errors?**
   - Check for missing `#include` statements
   - Look at Python/C++ bindings (Pybind11)

---

## Resources in This Workspace

| File | What's Inside |
|------|--------------|
| `LOWERING_TUTORIAL.md` | Comprehensive guide (READ THIS FIRST) |
| `LOWERING_EXAMPLE.md` | Concrete step-by-step examples |
| `LOWERING_SKELETON_CODE.md` | Code templates for each file |
| `/src/frontend/ast/` | AST class definitions |
| `/src/middlend/ir/` | IR classes and builder |
| `/grammar.ebnf` | Language grammar |
| `README.md` | Compiler pipeline overview |

---

## Next Steps

1. ✅ Read `LOWERING_TUTORIAL.md` (Part 0-2)
2. ✅ Understand the architecture diagram
3. ✅ Review the concrete example (`LOWERING_EXAMPLE.md`)
4. 🔄 **Implement `slot_alloc.cpp` first** (easiest)
5. 🔄 Test it with a simple unit test
6. 🔄 Move on to `emitter.cpp`, then `reader.cpp`
7. 🔄 Implement function-level lowering
8. 🔄 Implement main lowering orchestrator
9. 🔄 Add patching and validation
10. 🔄 Run full end-to-end tests

You've got this! 🚀
