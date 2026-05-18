#pragma once

#include "../../middlend/ir/common.hpp"
#include <stdexcept>
#include <string>
#include <vector>

class Reader {
public:
  // Dispatch on IRVal kind: 'C' = Const, 'T' = Temp, 'V' = Var
  static IRVal ReadValue(const IRVal &val, Emitter &emitter);

  // Dispatch on IRInstr opcode for expression-producing instructions
  static IRVal ReadInstr(const IRInstr &instr, Emitter &emitter);

  static std::vector<IRVal> ReadValues(const std::vector<IRVal> &vals,
                                       Emitter &emitter);

  static std::string InferLiteralType(const IRVal &val);

private:
  static IRVal ReadConst(const IRVal &val, Emitter &emitter);
  static IRVal ReadLoad(const IRVal &val, Emitter &emitter);
  static IRVal ReadBinary(const IRInstr &instr, Emitter &emitter);
  static IRVal ReadUnary(const IRInstr &instr, Emitter &emitter);
  static IRVal ReadCall(const IRInstr &instr, Emitter &emitter);
  static IRVal ReadInterp(const IRInstr &instr, Emitter &emitter);
};
