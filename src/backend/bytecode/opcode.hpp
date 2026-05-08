#pragma once
#include <cstdint>

// Bytecode instruction set for the ADAN stack machine.
// Each opcode is one byte. Operands (if any) follow as additional bytes.
enum class Opcode : uint8_t {
  // --- Constants & locals ---
  PUSH_CONST, // PUSH_CONST <u16:pool_idx>  — push constants[pool_idx] onto
              // stack
  LOAD_LOCAL, // LOAD_LOCAL  <u8:slot>       — push locals[slot] onto stack
  STORE_LOCAL, // STORE_LOCAL <u8:slot>       — pop top of stack → locals[slot]
  LOAD_GLOBAL,  // LOAD_GLOBAL <u16:name_idx>  — push global variable by name
                // index
  STORE_GLOBAL, // STORE_GLOBAL <u16:name_idx> — pop top of stack → global
                // variable

  // --- Arithmetic (binary) ---
  ADD, // pop rhs, pop lhs → push lhs + rhs
  SUB, // pop rhs, pop lhs → push lhs - rhs
  MUL, // pop rhs, pop lhs → push lhs * rhs
  DIV, // pop rhs, pop lhs → push lhs / rhs
  MOD, // pop rhs, pop lhs → push lhs % rhs

  // --- Comparison (binary) ---
  EQ,  // pop rhs, pop lhs → push lhs == rhs
  NEQ, // pop rhs, pop lhs → push lhs != rhs
  LT,  // pop rhs, pop lhs → push lhs <  rhs
  LTE, // pop rhs, pop lhs → push lhs <= rhs
  GT,  // pop rhs, pop lhs → push lhs >  rhs
  GTE, // pop rhs, pop lhs → push lhs >= rhs

  // --- Logical (binary) ---
  AND, // pop rhs, pop lhs → push lhs && rhs
  OR,  // pop rhs, pop lhs → push lhs || rhs

  // --- Unary ---
  NEG, // pop operand → push -operand
  NOT, // pop operand → push !operand

  // --- Control flow ---
  JUMP,          // JUMP <i16:offset>            — unconditional relative jump
  JUMP_IF_FALSE, // JUMP_IF_FALSE <i16:offset>   — pop cond; jump if falsy
  JUMP_IF_TRUE,  // JUMP_IF_TRUE  <i16:offset>   — pop cond; jump if truthy

  // --- Functions ---
  CALL, // CALL <u8:nargs>  — pop nargs args + callee name, push return value
  RETURN, // pop return value, restore frame, push into caller's stack

  // --- Strings ---
  BUILD_STRING, // BUILD_STRING <u8:nparts> — pop nparts values, concat → push
                // result

  // --- Misc ---
  NOP,  // no operation
  HALT, // stop execution
};
