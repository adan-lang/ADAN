#include "emitter.hpp"

IRVal Emitter::MakeTemp(const std::string &type) {
  IRVal t;
  t.kind = 'T';
  t.type = type.empty() ? "UNKNOWN" : type;
  t.name = "%t" + std::to_string(_next_temp++);
  return t;
}

IRVal Emitter::EmitConst(const IRVal &val) {
  // TODO: Append a real CONST instruction to the lowering output stream.
  return MakeTemp(val.type.empty() ? "UNKNOWN" : val.type);
}

IRVal Emitter::EmitLoad(const std::string &name, const std::string &type) {
  (void)name;
  // TODO: Append a real LOAD instruction for `name`.
  return MakeTemp(type.empty() ? "UNKNOWN" : type);
}

IRVal Emitter::EmitBinary(const std::string &op, const IRVal &left,
                          const IRVal &right) {
  (void)op;
  (void)right;
  // TODO: Append a real BINARY instruction and infer result type from op.
  return MakeTemp(left.type.empty() ? "UNKNOWN" : left.type);
}

IRVal Emitter::EmitUnary(const std::string &op, const IRVal &operand) {
  (void)op;
  // TODO: Append a real UNARY instruction and infer result type from op.
  return MakeTemp(operand.type.empty() ? "UNKNOWN" : operand.type);
}

IRVal Emitter::EmitCall(const std::string &func_name,
                        const std::vector<IRVal> &args) {
  (void)func_name;
  (void)args;
  // TODO: Append a real CALL instruction and resolve return type.
  return MakeTemp("UNKNOWN");
}

IRVal Emitter::EmitInterp(const std::vector<IRVal> &parts) {
  (void)parts;
  // TODO: Append a real INTERP instruction.
  return MakeTemp("STRING");
}
