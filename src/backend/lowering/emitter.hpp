#pragma once

#include <string>
#include <vector>

#include "../../middlend/ir/common.hpp"

class Emitter {
public:
  IRVal EmitConst(const IRVal &val);
  IRVal EmitLoad(const std::string &name, const std::string &type = "UNKNOWN");
  IRVal EmitBinary(const std::string &op, const IRVal &left,
                   const IRVal &right);
  IRVal EmitUnary(const std::string &op, const IRVal &operand);
  IRVal EmitCall(const std::string &func_name, const std::vector<IRVal> &args);
  IRVal EmitInterp(const std::vector<IRVal> &parts);

private:
  ` int _next_temp = 0;

  IRVal MakeTemp(const std::string &type);
};
