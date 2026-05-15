# Lowering Implementation - Skeleton Code Templates

Use these as starting points for each file. They show the basic structure and what methods you need to implement.

---

## 1. reader.hpp (Header File)

```cpp
#pragma once

#include <memory>
#include <string>
#include "../../../middlend/ir/types.py"
#include "../../../middlend/ir/value.py"

// Forward declarations
class ASTNode;
class Expression;
class Statement;
class Emitter;
class IRValue;

class Reader {
public:
    /**
     * Read an expression from the AST and emit IR code for it
     * 
     * Example:
     *   - LiteralExpr(42) → emit CONST 42, return Temp
     *   - IdentifierExpr("x") → emit LOAD x, return Temp
     *   - BinaryExpr(+, left, right) → emit BINARY left, right, "+", return Temp
     */
    static IRValue ReadExpression(Expression* expr, Emitter& emitter);
    
    /**
     * Helper: read a list of expressions (for function arguments, etc.)
     */
    static std::vector<IRValue> ReadExpressions(
        const std::vector<Expression*>& exprs,
        Emitter& emitter
    );
    
    /**
     * Infer the type of a literal value
     * Example: 42 → NUMBER, "hello" → STRING, true → BOOL
     */
    static IRType InferLiteralType(const std::string& value);
};
```

---

## 2. emitter.hpp (Header File)

```cpp
#pragma once

#include <string>
#include <vector>
#include "../../../middlend/ir/builder.py"
#include "../../../middlend/ir/types.py"
#include "../../../middlend/ir/value.py"

class Emitter {
private:
    IRBuilder builder;
    
public:
    /**
     * Emit a CONST instruction
     * Creates an immediate constant value
     * 
     * Example:
     *   Temp result = EmitConst("42", IRType.NUMBER)
     *   Result IR: %0 = CONST 42
     */
    Temp EmitConst(const std::string& value, IRType type);
    
    /**
     * Emit a LOAD instruction
     * Loads a variable into a temporary
     * 
     * Example:
     *   Temp result = EmitLoad("x", IRType.NUMBER)
     *   Result IR: %0 = LOAD x
     */
    Temp EmitLoad(const std::string& var_name, IRType type = IRType.UNKNOWN);
    
    /**
     * Emit a STORE instruction
     * Stores a value to a variable
     * 
     * Example:
     *   EmitStore("x", temp_value)
     *   Result IR: STORE x, %0
     */
    void EmitStore(const std::string& var_name, const IRValue& value);
    
    /**
     * Emit a BINARY instruction
     * Performs a binary operation
     * 
     * Example:
     *   Temp result = EmitBinary("+", left_temp, right_temp)
     *   Result IR: %2 = BINARY %0, %1, "+"
     */
    Temp EmitBinary(
        const std::string& op,
        const IRValue& left,
        const IRValue& right
    );
    
    /**
     * Emit a UNARY instruction
     * Performs a unary operation
     * 
     * Example:
     *   Temp result = EmitUnary("-", operand)
     *   Result IR: %1 = UNARY %0, "-"
     */
    Temp EmitUnary(const std::string& op, const IRValue& operand);
    
    /**
     * Emit a CALL instruction
     * Calls a function
     * 
     * Example:
     *   std::vector<IRValue> args = {arg1, arg2};
     *   Temp result = EmitCall("add", args);
     *   Result IR: %2 = CALL add(%0, %1)
     */
    Temp EmitCall(const std::string& func_name, const std::vector<IRValue>& args);
    
    /**
     * Emit a BRANCH instruction
     * Branches based on a condition
     * 
     * Example:
     *   EmitBranch(cond, "true_label", "false_label")
     *   Result IR: BRANCH %0, true_label, false_label
     */
    void EmitBranch(const IRValue& condition, const std::string& true_label,
                    const std::string& false_label);
    
    /**
     * Emit a JUMP instruction
     * Unconditional jump
     * 
     * Example:
     *   EmitJump("end_label")
     *   Result IR: JUMP end_label
     */
    void EmitJump(const std::string& label);
    
    /**
     * Emit a RET instruction
     * Return from function
     * 
     * Example:
     *   EmitReturn(return_value)
     *   Result IR: RET %0
     */
    void EmitReturn(const IRValue& value);
    void EmitReturn();  // Return void (none)
    
    /**
     * Emit an INTERP instruction
     * String interpolation
     * 
     * Example:
     *   std::vector<IRValue> parts = {str1, temp1, str2};
     *   Temp result = EmitInterp(parts)
     *   Result IR: %3 = INTERP %0, %1, %2
     */
    Temp EmitInterp(const std::vector<IRValue>& parts);
};
```

---

## 3. reader.cpp (Implementation)

```cpp
#include "reader.hpp"
#include "emitter.hpp"
#include "../../../frontend/ast/expressions.py"
#include "../../../frontend/ast/literals.py"

// ============ EXPRESSION READING ============

IRValue Reader::ReadExpression(Expression* expr, Emitter& emitter) {
    if (!expr) {
        // Handle null expression
        return Const(Null, IRType.NONE);
    }
    
    // TODO: Add type checking using RTTI or type field
    // Pseudocode:
    //
    // if (auto lit = dynamic_cast<LiteralExpr*>(expr)) {
    //     return ReadLiteral(lit, emitter);
    // }
    // else if (auto id = dynamic_cast<IdentifierExpr*>(expr)) {
    //     return ReadIdentifier(id, emitter);
    // }
    // ... etc
    
    throw std::runtime_error("Unknown expression type");
}

// ============ SPECIFIC EXPRESSION HANDLERS ============

// TODO: Implement these helper methods:

// IRValue ReadLiteral(LiteralExpr* lit, Emitter& emitter) {
//     IRType type = InferLiteralType(lit->Value);
//     return emitter.EmitConst(lit->Value, type);
// }

// IRValue ReadIdentifier(IdentifierExpr* id, Emitter& emitter) {
//     // TODO: Look up variable type from symbol table
//     return emitter.EmitLoad(id->Name, IRType.UNKNOWN);
// }

// IRValue ReadBinary(BinaryExpr* binop, Emitter& emitter) {
//     IRValue left = ReadExpression(binop->Left, emitter);
//     IRValue right = ReadExpression(binop->Right, emitter);
//     return emitter.EmitBinary(binop->Operator, left, right);
// }

// IRValue ReadCall(CallExpr* call, Emitter& emitter) {
//     std::vector<IRValue> args = ReadExpressions(call->Arguments, emitter);
//     return emitter.EmitCall(call->Callee, args);
// }

// ============ BULK OPERATIONS ============

std::vector<IRValue> Reader::ReadExpressions(
    const std::vector<Expression*>& exprs,
    Emitter& emitter
) {
    std::vector<IRValue> results;
    for (auto expr : exprs) {
        results.push_back(ReadExpression(expr, emitter));
    }
    return results;
}

// ============ TYPE INFERENCE ============

IRType Reader::InferLiteralType(const std::string& value) {
    // TODO: Implement logic to determine type from value
    // Hints:
    // - If it's "true" or "false" → BOOL
    // - If it's a number (starts with digit) → NUMBER
    // - If it's quoted (starts with " or ') → STRING
    // - Otherwise → UNKNOWN
    
    return IRType.UNKNOWN;
}
```

---

## 4. emitter.cpp (Implementation)

```cpp
#include "emitter.hpp"

// ============ CONST INSTRUCTION ============

Temp Emitter::EmitConst(const std::string& value, IRType type) {
    // TODO: Create a Const IRValue and emit CONST instruction
    // Hint: Use builder.EmitConst(value, type)
    
    return Temp(-1, type);  // Placeholder
}

// ============ LOAD INSTRUCTION ============

Temp Emitter::EmitLoad(const std::string& var_name, IRType type) {
    // TODO: Emit a LOAD instruction
    // Hint: Use builder.EmitLoad(var_name, type)
    
    return Temp(-1, type);  // Placeholder
}

// ============ STORE INSTRUCTION ============

void Emitter::EmitStore(const std::string& var_name, const IRValue& value) {
    // TODO: Emit a STORE instruction
    // Hint: Use builder.EmitStore(var_name, value)
}

// ============ BINARY INSTRUCTION ============

Temp Emitter::EmitBinary(
    const std::string& op,
    const IRValue& left,
    const IRValue& right
) {
    // TODO: Determine result type based on operator and operand types
    // Hints:
    // - For "+", "-", "*", "/" with NUMBER operands → NUMBER
    // - For "==" with any operands → BOOL
    // - For "and", "or" with BOOL operands → BOOL
    // - Otherwise might be type error
    
    IRType result_type = IRType.UNKNOWN;  // TODO: Infer actual type
    
    // Emit the instruction using builder
    return Temp(-1, result_type);  // Placeholder
}

// ============ UNARY INSTRUCTION ============

Temp Emitter::EmitUnary(const std::string& op, const IRValue& operand) {
    // TODO: Emit a UNARY instruction
    // Hints:
    // - "-" operator: operand should be NUMBER, result is NUMBER
    // - "not" operator: operand should be BOOL, result is BOOL
    
    IRType result_type = IRType.UNKNOWN;  // TODO: Infer actual type
    
    return Temp(-1, result_type);  // Placeholder
}

// ============ CALL INSTRUCTION ============

Temp Emitter::EmitCall(
    const std::string& func_name,
    const std::vector<IRValue>& args
) {
    // TODO: Emit a CALL instruction
    // Hint: Use builder.EmitCall(func_name, args, result_type)
    // Note: Result type depends on function signature - you'll need to look it up!
    
    return Temp(-1, IRType.UNKNOWN);  // Placeholder
}

// ============ CONTROL FLOW INSTRUCTIONS ============

void Emitter::EmitBranch(const IRValue& condition, const std::string& true_label,
                         const std::string& false_label) {
    // TODO: Emit a BRANCH instruction
    // Hint: Use builder.EmitBranch(condition, true_label, false_label)
}

void Emitter::EmitJump(const std::string& label) {
    // TODO: Emit a JUMP instruction
    // Hint: Use builder.EmitJump(label)
}

// ============ RETURN INSTRUCTION ============

void Emitter::EmitReturn(const IRValue& value) {
    // TODO: Emit a RET instruction
    // Hint: Use builder.EmitRet(value)
}

void Emitter::EmitReturn() {
    // TODO: Emit a RET instruction with no value
    // Hint: Use builder.EmitRet()
}

// ============ STRING INTERPOLATION ============

Temp Emitter::EmitInterp(const std::vector<IRValue>& parts) {
    // TODO: Emit an INTERP instruction
    // Hint: Use builder.EmitInterp(parts)
    // Note: Result type is always STRING
    
    return Temp(-1, IRType.STRING);  // Placeholder
}
```

---

## 5. function.hpp (Header File)

```cpp
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../../../middlend/ir/builder.py"

// Forward declarations
class FunctionDefStmt;
class Statement;
class Emitter;
class IRFunction;

class FunctionLowering {
private:
    Emitter& emitter;
    
public:
    FunctionLowering(Emitter& em) : emitter(em) {}
    
    /**
     * Lower a function definition from AST to IR
     * 
     * Algorithm:
     * 1. Create an IRFunction with name and parameters
     * 2. Create an entry block
     * 3. Lower each statement in the function body
     * 4. Emit return instruction for the return expression
     */
    IRFunction* Lower(FunctionDefStmt* func_stmt);
    
private:
    /**
     * Lower a single statement
     * Dispatches based on statement type
     */
    void LowerStatement(Statement* stmt);
    
    /**
     * Helper: Lower an assignment statement
     */
    void LowerAssignment(AssignmentStmt* assign);
    
    /**
     * Helper: Lower an if statement
     * Creates multiple basic blocks for true/false branches
     */
    void LowerIfStatement(IfStmt* if_stmt);
};
```

---

## 6. function.cpp (Implementation Skeleton)

```cpp
#include "function.hpp"
#include "reader.hpp"
#include "emitter.hpp"
#include "../../../frontend/ast/statements.py"
#include "../../../middlend/ir/module.py"

IRFunction* FunctionLowering::Lower(FunctionDefStmt* func_stmt) {
    // TODO: Implement function lowering
    //
    // Pseudocode:
    // 1. Extract parameters and return type
    // 2. Call emitter.builder.BeginFunction(name, params, return_type)
    // 3. Call emitter.builder.BeginBlock("entry") to create entry block
    // 4. For each statement in func_stmt->Body:
    //    - Call LowerStatement(stmt)
    // 5. Read the return expression
    // 6. Emit RET instruction
    // 7. Return the IR function
    
    return nullptr;  // Placeholder
}

void FunctionLowering::LowerStatement(Statement* stmt) {
    // TODO: Dispatch based on statement type
    //
    // if (auto assign = dynamic_cast<AssignmentStmt*>(stmt)) {
    //     LowerAssignment(assign);
    // }
    // else if (auto if_stmt = dynamic_cast<IfStmt*>(stmt)) {
    //     LowerIfStatement(if_stmt);
    // }
    // else if (auto expr_stmt = dynamic_cast<ExpressionStmt*>(stmt)) {
    //     Reader::ReadExpression(expr_stmt->Expression, emitter);
    // }
}

void FunctionLowering::LowerAssignment(AssignmentStmt* assign) {
    // TODO: Read the right-hand side expression
    // TODO: Emit a STORE instruction to the variable
    
    // Pseudocode:
    // IRValue value = Reader::ReadExpression(assign->Value, emitter);
    // emitter.EmitStore(assign->Name, value);
}

void FunctionLowering::LowerIfStatement(IfStmt* if_stmt) {
    // TODO: Implement if-statement lowering
    //
    // Algorithm:
    // 1. Read the condition expression
    // 2. Create "if_true" and "if_false" blocks
    // 3. Create "if_end" block
    // 4. Emit BRANCH instruction
    // 5. Switch to true_block and lower true branch statements
    // 6. Emit JUMP to end block
    // 7. Switch to false_block and lower false branch statements (if exists)
    // 8. Emit JUMP to end block
    // 9. Switch to end_block for subsequent code
}
```

---

## 7. lowering.hpp (Header File)

```cpp
#pragma once

#include <memory>

// Forward declarations
class Program;  // From AST
class IRModule; // From IR

class Lowering {
public:
    /**
     * Lower an entire program (from AST to IR)
     * 
     * Algorithm:
     * 1. Create an IRModule
     * 2. For each statement in the program:
     *    - If it's a FunctionDefStmt, lower it
     *    - If it's an assignment, handle as global
     * 3. Return the module
     */
    IRModule* Lower(Program* ast);
};
```

---

## 8. lowering.cpp (Implementation Skeleton)

```cpp
#include "lowering.hpp"
#include "function.hpp"
#include "reader.hpp"
#include "emitter.hpp"
#include "../../../frontend/ast/base.py"
#include "../../../frontend/ast/statements.py"
#include "../../../middlend/ir/module.py"

IRModule* Lowering::Lower(Program* ast) {
    // TODO: Implement main lowering logic
    //
    // Pseudocode:
    // 1. Create a new IRModule
    // 2. Create an Emitter with the module's IRBuilder
    // 3. For each statement in ast->Statements:
    //    - If it's FunctionDefStmt: use FunctionLowering to lower it
    //    - If it's AssignmentStmt: handle as global assignment
    // 4. Return the module
    
    auto module = new IRModule();
    
    // TODO: Iterate through statements and lower them
    
    return module;
}
```

---

## 9. patch.hpp (Header File)

```cpp
#pragma once

// Forward declarations
class IRModule;
class IRFunction;

class Patcher {
public:
    /**
     * Post-process and validate IR
     * 
     * Operations:
     * 1. Remove unreachable blocks
     * 2. Validate control flow (all paths return)
     * 3. Validate type consistency
     * 4. Fix any invalid IR
     */
    void Patch(IRModule* module);
    
private:
    /**
     * Patch a single function
     */
    void PatchFunction(IRFunction* func);
    
    /**
     * Remove blocks that can never be reached
     */
    void RemoveUnreachableBlocks(IRFunction* func);
    
    /**
     * Ensure all code paths return a value
     */
    void ValidateControlFlow(IRFunction* func);
    
    /**
     * Ensure types are used consistently
     */
    void ValidateTypes(IRFunction* func);
};
```

---

## 10. patch.cpp (Implementation Skeleton)

```cpp
#include "patch.hpp"
#include "../../../middlend/ir/module.py"
#include "../../../middlend/ir/function.py"

void Patcher::Patch(IRModule* module) {
    // TODO: For each function in module, call PatchFunction
}

void Patcher::PatchFunction(IRFunction* func) {
    // TODO: Call all three validation functions in order
    RemoveUnreachableBlocks(func);
    ValidateControlFlow(func);
    ValidateTypes(func);
}

void Patcher::RemoveUnreachableBlocks(IRFunction* func) {
    // TODO: Implement reachability analysis
    //
    // Algorithm:
    // 1. Starting from entry block, mark all blocks that can be reached
    // 2. Delete all unmarked blocks
    // 3. Update jump targets if necessary
}

void Patcher::ValidateControlFlow(IRFunction* func) {
    // TODO: Ensure control flow is valid
    //
    // Checks:
    // 1. All BRANCH/JUMP instructions point to real blocks
    // 2. No code after RET in a block
    // 3. All non-return blocks end with JUMP, BRANCH, or RET
}

void Patcher::ValidateTypes(IRFunction* func) {
    // TODO: Ensure type consistency
    //
    // Checks:
    // 1. BINARY operators have compatible operand types
    // 2. STORE operations store correct type
    // 3. RETURN value matches function return type
}
```

---

## Tips for Implementation

1. **Start Simple**: Implement the easy cases first (literals, identifiers)
2. **Test Early**: Write unit tests for each method as you implement it
3. **Use Debug Prints**: Add cout statements to see what IR is being generated
4. **Reference the Existing Code**: Look at how IRBuilder is used in middlend/
5. **One Method at a Time**: Don't try to implement everything at once

---

## Compilation Checklist

Before you can compile, you need:
- [ ] All header files written
- [ ] All cpp files have implementations (not just TODO comments)
- [ ] Python/C++ bridge set up (may already be done)
- [ ] Build system configured (CMakeLists.txt or similar)

Run: `make` or `cmake --build .` to compile.
