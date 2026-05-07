#include "../ir/common.hpp"

static bool fold_block(IRBlock &blk) {
  bool changed = false;
  std::unordered_map<std::string, IRVal> cv;

  for (auto &i : blk.instrs) {
    for (auto &op : i.operands) {
      if (op.kind == 'T') {
        auto it = cv.find(op.name);
        if (it != cv.end()) {
          op = it->second;
          changed = true;
        }
      }
    }

    if (i.op == "CONST" && i.has_dst && i.dst.kind == 'T' &&
        !i.operands.empty()) {
      cv[i.dst.name] = i.operands[0];
      continue;
    }

    if (!i.has_dst || i.dst.kind != 'T')
      continue;

    auto all_const = [&]() {
      for (auto &op : i.operands)
        if (op.kind != 'C')
          return false;
      return !i.operands.empty();
    };

    if (i.op == "BINARY" && i.operands.size() == 2 && all_const()) {
      auto &L = i.operands[0];
      auto &R = i.operands[1];
      const std::string &op = i.extra_str;
      IRVal res;
      res.kind = 'C';
      bool ok = false;

      if (!L.is_null && !R.is_null) {
        if (op == "-") {
          res.fval = L.fval - R.fval;
          res.type = "NUMBER";
          ok = true;
        } else if (op == "+") {
          res.fval = L.fval + R.fval;
          res.type = "NUMBER";
          ok = true;
        } else if (op == "*") {
          res.fval = L.fval * R.fval;
          res.type = "NUMBER";
          ok = true;
        } else if (op == "/" && R.fval != 0.0) {
          res.fval = L.fval / R.fval;
          res.type = "NUMBER";
          ok = true;
        } else if (op == "or") {
          res.bval = L.bval || R.bval;
          res.is_bool = true;
          res.type = "BOOL";
          ok = true;
        } else if (op == "and") {
          res.bval = L.bval && R.bval;
          res.is_bool = true;
          res.type = "BOOL";
          ok = true;
        }
      }

      if (ok) {
        i.op = "CONST";
        i.operands = {res};
        i.extra_str = "";
        i.extra_pair = false;
        cv[i.dst.name] = res;
        changed = true;
      }
    } else if (i.op == "UNARY" && i.operands.size() == 1 && all_const()) {
      auto &operand = i.operands[0];
      const std::string &op = i.extra_str;
      IRVal res;
      res.kind = 'C';
      bool ok = false;

      if (!operand.is_null) {
        if (op == "-") {
          res.fval = -operand.fval;
          res.type = "NUMBER";
          ok = true;
        } else if (op == "not") {
          res.bval = !operand.bval;
          res.is_bool = true;
          res.type = "BOOL";
          ok = true;
        }
      }

      if (ok) {
        i.op = "CONST";
        i.operands = {res};
        i.extra_str = "";
        i.extra_pair = false;
        cv[i.dst.name] = res;
        changed = true;
      }
    }
  }
  return changed;
}

static bool fold_constants(IRFunc &f) {
  bool any = false;
  for (auto &b : f.blocks)
    any |= fold_block(b);
  return any;
}

static bool dce_func(IRFunc &f) {
  std::unordered_set<std::string> live;

  auto is_effect = [](const IRInstr &i) {
    return i.op == "STORE" || i.op == "RET" || i.op == "BRANCH" ||
           i.op == "JUMP" || i.op == "CALL";
  };

  auto mark_ops = [&](const IRInstr &i) {
    for (auto &op : i.operands)
      if (op.kind == 'T')
        live.insert(op.name);
  };

  for (auto &b : f.blocks)
    for (auto &i : b.instrs)
      if (is_effect(i))
        mark_ops(i);

  bool changed = true;
  while (changed) {
    changed = false;
    for (auto &b : f.blocks) {
      for (auto &i : b.instrs) {
        if (i.has_dst && i.dst.kind == 'T' && live.count(i.dst.name)) {
          for (auto &op : i.operands) {
            if (op.kind == 'T' && !live.count(op.name)) {
              live.insert(op.name);
              changed = true;
            }
          }
        }
      }
    }
  }

  bool removed = false;
  for (auto &b : f.blocks) {
    std::vector<IRInstr> kept;
    for (auto &i : b.instrs) {
      bool keep = is_effect(i) || !i.has_dst ||
                  (i.has_dst && i.dst.kind == 'T' && live.count(i.dst.name));
      if (keep)
        kept.push_back(i);
      else
        removed = true;
    }
    b.instrs = std::move(kept);
  }
  return removed;
}

static bool cse_func(IRFunc &f) {
  bool changed = false;
  for (auto &b : f.blocks) {
    std::unordered_map<std::string, std::string> expr_map;

    for (auto &i : b.instrs) {
      if ((i.op != "BINARY" && i.op != "UNARY") || !i.has_dst ||
          i.dst.kind != 'T')
        continue;

      std::ostringstream key;
      key << i.op << "|";
      for (auto &op : i.operands) {
        if (op.kind == 'T')
          key << "T:" << op.name;
        else if (op.kind == 'C') {
          if (op.is_null)
            key << "C:null";
          else if (op.is_bool)
            key << "C:b:" << op.bval;
          else if (op.is_str)
            key << "C:s:" << op.sval;
          else
            key << "C:n:" << op.fval;
        } else
          key << "?";
        key << ",";
      }
      key << i.extra_str;

      std::string k = key.str();
      auto it = expr_map.find(k);
      if (it != expr_map.end()) {
        IRVal src;
        src.kind = 'T';
        src.name = it->second;
        src.type = i.dst.type;
        i.op = "COPY";
        i.operands = {src};
        i.extra_str = "";
        i.extra_pair = false;
        changed = true;
      } else {
        expr_map[k] = i.dst.name;
      }
    }
  }
  return changed;
}

static bool jump_thread(IRFunc &f) {
  bool changed = false;
  for (auto &b : f.blocks) {
    if (b.instrs.empty())
      continue;
    auto &last = b.instrs.back();
    if (last.op != "JUMP")
      continue;

    const std::string &lbl = last.extra_str;
    auto it = f.label_idx.find(lbl);
    if (it == f.label_idx.end())
      continue;

    auto &tgt = f.blocks[it->second];
    if (tgt.instrs.size() == 1 && tgt.instrs[0].op == "JUMP") {
      last.extra_str = tgt.instrs[0].extra_str;
      changed = true;
    }
  }
  return changed;
}

extern "C" {

char *opt_fold_constants(const char *json_ir) {
  if (!json_ir)
    return strdup("{}");
  try {
    IRMod *mod = ir_parse(json_ir);
    for (auto &f : mod->functions)
      fold_constants(f);
    std::string out = ir_serialize(mod);
    delete mod;
    return strdup(out.c_str());
  } catch (...) {
    return strdup(json_ir);
  }
}

char *opt_dead_code_elim(const char *json_ir) {
  if (!json_ir)
    return strdup("{}");
  try {
    IRMod *mod = ir_parse(json_ir);
    for (auto &f : mod->functions)
      dce_func(f);
    std::string out = ir_serialize(mod);
    delete mod;
    return strdup(out.c_str());
  } catch (...) {
    return strdup(json_ir);
  }
}

char *opt_cse(const char *json_ir) {
  if (!json_ir)
    return strdup("{}");
  try {
    IRMod *mod = ir_parse(json_ir);
    for (auto &f : mod->functions)
      cse_func(f);
    std::string out = ir_serialize(mod);
    delete mod;
    return strdup(out.c_str());
  } catch (...) {
    return strdup(json_ir);
  }
}

char *opt_all(const char *json_ir, int passes) {
  if (!json_ir)
    return strdup("{}");
  if (passes <= 0)
    passes = 3;
  try {
    IRMod *mod = ir_parse(json_ir);
    for (int p = 0; p < passes; ++p) {
      for (auto &f : mod->functions) {
        fold_constants(f);
        cse_func(f);
        dce_func(f);
        jump_thread(f);
      }
    }
    std::string out = ir_serialize(mod);
    delete mod;
    return strdup(out.c_str());
  } catch (...) {
    return strdup(json_ir);
  }
}

void opt_free_str(char *s) { free(s); }
}
