#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct JVal;
using JObj = std::unordered_map<std::string, std::shared_ptr<JVal>>;
using JArr = std::vector<std::shared_ptr<JVal>>;
enum JKind { JK_NULL, JK_BOOL, JK_INT, JK_FLOAT, JK_STR, JK_ARR, JK_OBJ };

struct JVal {
  JKind kind = JK_NULL;
  bool bv = false;
  int64_t iv = 0;
  double dv = 0.0;
  std::string sv;
  JArr arr;
  JObj obj;

  const std::string &str() const { return sv; }
  bool has(const std::string &k) const { return obj.count(k) > 0; }

  std::shared_ptr<JVal> get(const std::string &k) const {
    auto it = obj.find(k);
    return it == obj.end() ? nullptr : it->second;
  }
  std::string gs(const std::string &k, const std::string &def = "") const {
    auto v = get(k);
    return (v && v->kind == JK_STR) ? v->sv : def;
  }
  const JArr &ga(const std::string &k) const {
    static JArr empty;
    auto v = get(k);
    return (v && v->kind == JK_ARR) ? v->arr : empty;
  }
};

static void jskip(const char *&p) {
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
    ++p;
}

static std::string jparse_str(const char *&p) {
  std::string s;
  ++p;
  while (*p && *p != '"') {
    if (*p == '\\') {
      ++p;
      switch (*p) {
      case '"':
        s += '"';
        break;
      case '\\':
        s += '\\';
        break;
      case '/':
        s += '/';
        break;
      case 'n':
        s += '\n';
        break;
      case 'r':
        s += '\r';
        break;
      case 't':
        s += '\t';
        break;
      default:
        s += *p;
      }
    } else {
      s += *p;
    }
    ++p;
  }
  if (*p == '"')
    ++p;
  return s;
}

static std::shared_ptr<JVal> jparse(const char *&p) {
  jskip(p);
  auto v = std::make_shared<JVal>();

  if (*p == '"') {
    v->kind = JK_STR;
    v->sv = jparse_str(p);
  } else if (*p == '{') {
    v->kind = JK_OBJ;
    ++p;
    jskip(p);
    while (*p && *p != '}') {
      jskip(p);
      if (*p != '"')
        break;
      std::string key = jparse_str(p);
      jskip(p);
      if (*p == ':')
        ++p;
      v->obj[key] = jparse(p);
      jskip(p);
      if (*p == ',')
        ++p;
    }
    if (*p == '}')
      ++p;
  } else if (*p == '[') {
    v->kind = JK_ARR;
    ++p;
    jskip(p);
    while (*p && *p != ']') {
      v->arr.push_back(jparse(p));
      jskip(p);
      if (*p == ',')
        ++p;
      jskip(p);
    }
    if (*p == ']')
      ++p;
  } else if (!strncmp(p, "null", 4)) {
    v->kind = JK_NULL;
    p += 4;
  } else if (!strncmp(p, "true", 4)) {
    v->kind = JK_BOOL;
    v->bv = true;
    p += 4;
  } else if (!strncmp(p, "false", 5)) {
    v->kind = JK_BOOL;
    v->bv = false;
    p += 5;
  } else {
    const char *q = p;
    bool is_float = false;
    if (*p == '-')
      ++p;
    while (*p >= '0' && *p <= '9')
      ++p;
    if (*p == '.') {
      is_float = true;
      ++p;
      while (*p >= '0' && *p <= '9')
        ++p;
    }
    if (*p == 'e' || *p == 'E') {
      is_float = true;
      ++p;
      if (*p == '+' || *p == '-')
        ++p;
      while (*p >= '0' && *p <= '9')
        ++p;
    }
    std::string ns(q, p);
    if (is_float) {
      v->kind = JK_FLOAT;
      v->dv = std::stod(ns);
    } else {
      v->kind = JK_INT;
      v->iv = (int64_t)std::stoll(ns);
    }
  }
  jskip(p);
  return v;
}

struct IRVal {
  char kind = '?';
  std::string type;
  std::string name;
  std::string sval;
  double fval = 0.0;
  bool bval = false;
  bool is_null = false;
  bool is_bool = false;
  bool is_str = false;
};

struct IRInstr {
  std::string op;
  IRVal dst;
  bool has_dst = false;
  std::vector<IRVal> operands;
  bool extra_pair = false;
  std::string extra_a;
  std::string extra_b;
  std::string extra_str;
};

struct IRBlock {
  std::string label;
  std::vector<IRInstr> instrs;
  std::vector<int> succs;
  std::vector<int> preds;
};

struct IRFunc {
  std::string name;
  std::string return_type;
  std::vector<std::pair<std::string, std::string>> params;
  std::vector<IRBlock> blocks;
  std::unordered_map<std::string, int> label_idx;
};

struct IRMod {
  std::string name;
  std::vector<IRFunc> functions;
};

static IRVal parse_val(const std::shared_ptr<JVal> &jv) {
  IRVal v;
  if (!jv || jv->kind == JK_NULL) {
    v.is_null = true;
    return v;
  }
  const std::string ks = jv->gs("k", "?");
  v.kind = ks.empty() ? '?' : ks[0];
  v.type = jv->gs("t", "UNKNOWN");
  v.name = jv->gs("n", "");
  auto jr = jv->get("v");
  if (jr) {
    switch (jr->kind) {
    case JK_NULL:
      v.is_null = true;
      break;
    case JK_BOOL:
      v.bval = jr->bv;
      v.is_bool = true;
      break;
    case JK_STR:
      v.sval = jr->sv;
      v.is_str = true;
      break;
    case JK_INT:
      v.fval = (double)jr->iv;
      break;
    case JK_FLOAT:
      v.fval = jr->dv;
      break;
    default:
      break;
    }
  }
  return v;
}

static IRInstr parse_instr(const std::shared_ptr<JVal> &ji) {
  IRInstr i;
  i.op = ji->gs("o", "NOP");
  auto jd = ji->get("d");
  i.has_dst = jd && jd->kind != JK_NULL;
  if (i.has_dst)
    i.dst = parse_val(jd);
  for (auto &jo : ji->ga("ops"))
    i.operands.push_back(parse_val(jo));
  auto jx = ji->get("x");
  if (jx && jx->kind != JK_NULL) {
    if (jx->kind == JK_STR) {
      i.extra_str = jx->sv;
      i.extra_pair = false;
    } else if (jx->kind == JK_OBJ) {
      i.extra_a = jx->gs("a", "");
      i.extra_b = jx->gs("b", "");
      i.extra_pair = true;
    }
  }
  return i;
}

static IRFunc parse_func(const std::shared_ptr<JVal> &jf) {
  IRFunc f;
  f.name = jf->gs("n", "__anon__");
  f.return_type = jf->gs("r", "UNKNOWN");
  for (auto &jp : jf->ga("p"))
    f.params.push_back({jp->gs("n", ""), jp->gs("t", "UNKNOWN")});
  for (auto &jb : jf->ga("b")) {
    IRBlock blk;
    blk.label = jb->gs("l", "");
    for (auto &ji : jb->ga("i"))
      blk.instrs.push_back(parse_instr(ji));
    int idx = (int)f.blocks.size();
    f.label_idx[blk.label] = idx;
    f.blocks.push_back(std::move(blk));
  }
  return f;
}

static IRMod *ir_parse(const char *json) {
  const char *p = json;
  auto root = jparse(p);
  auto *mod = new IRMod();
  mod->name = root->gs("n", "module");
  for (auto &jf : root->ga("f"))
    mod->functions.push_back(parse_func(jf));
  return mod;
}

static std::string json_esc(const std::string &s) {
  std::string r;
  r.reserve(s.size());
  for (unsigned char c : s) {
    if (c == '"')
      r += "\\\"";
    else if (c == '\\')
      r += "\\\\";
    else if (c == '\n')
      r += "\\n";
    else if (c == '\r')
      r += "\\r";
    else if (c == '\t')
      r += "\\t";
    else
      r += (char)c;
  }
  return r;
}

static std::string num_str(double d) {
  if (d == (double)(int64_t)d)
    return std::to_string((int64_t)d);
  std::ostringstream ss;
  ss << d;
  return ss.str();
}

static std::string ser_raw_val(const IRVal &v) {
  if (v.is_null)
    return "null";
  if (v.is_bool)
    return v.bval ? "true" : "false";
  if (v.is_str)
    return "\"" + json_esc(v.sval) + "\"";
  return num_str(v.fval);
}

static std::string ser_val(const IRVal &v) {
  if (v.kind == '?')
    return "null";
  if (v.kind == 'C') {
    return "{\"k\":\"C\",\"t\":\"" + v.type + "\",\"v\":" + ser_raw_val(v) +
           "}";
  }
  std::string ks(1, v.kind);
  return "{\"k\":\"" + ks + "\",\"t\":\"" + json_esc(v.type) + "\",\"n\":\"" +
         json_esc(v.name) + "\"}";
}

static std::string ser_extra(const IRInstr &i) {
  if (i.extra_pair)
    return "{\"a\":\"" + json_esc(i.extra_a) + "\",\"b\":\"" +
           json_esc(i.extra_b) + "\"}";
  if (!i.extra_str.empty())
    return "\"" + json_esc(i.extra_str) + "\"";
  return "null";
}

static std::string ser_instr(const IRInstr &i) {
  std::ostringstream s;
  s << "{\"o\":\"" << json_esc(i.op) << "\","
    << "\"d\":" << (i.has_dst ? ser_val(i.dst) : "null") << "," << "\"ops\":[";
  for (int k = 0; k < (int)i.operands.size(); ++k) {
    if (k)
      s << ",";
    s << ser_val(i.operands[k]);
  }
  s << "],\"x\":" << ser_extra(i) << "}";
  return s.str();
}

static std::string ser_block(const IRBlock &b) {
  std::ostringstream s;
  s << "{\"l\":\"" << json_esc(b.label) << "\",\"i\":[";
  for (int k = 0; k < (int)b.instrs.size(); ++k) {
    if (k)
      s << ",";
    s << ser_instr(b.instrs[k]);
  }
  s << "]}";
  return s.str();
}

static std::string ser_func(const IRFunc &f) {
  std::ostringstream s;
  s << "{\"n\":\"" << json_esc(f.name) << "\"," << "\"p\":[";
  for (int i = 0; i < (int)f.params.size(); ++i) {
    if (i)
      s << ",";
    s << "{\"n\":\"" << json_esc(f.params[i].first) << "\",\"t\":\""
      << json_esc(f.params[i].second) << "\"}";
  }
  s << "],\"r\":\"" << json_esc(f.return_type) << "\",\"b\":[";
  for (int i = 0; i < (int)f.blocks.size(); ++i) {
    if (i)
      s << ",";
    s << ser_block(f.blocks[i]);
  }
  s << "]}";
  return s.str();
}

static std::string ir_serialize(const IRMod *mod) {
  std::ostringstream s;
  s << "{\"n\":\"" << json_esc(mod->name) << "\",\"f\":[";
  for (int i = 0; i < (int)mod->functions.size(); ++i) {
    if (i)
      s << ",";
    s << ser_func(mod->functions[i]);
  }
  s << "]}";
  return s.str();
}

static void build_cfg(IRFunc &f) {
  int n = (int)f.blocks.size();
  for (auto &b : f.blocks) {
    b.succs.clear();
    b.preds.clear();
  }

  for (int bi = 0; bi < n; ++bi) {
    auto &blk = f.blocks[bi];
    if (blk.instrs.empty())
      continue;
    const auto &last = blk.instrs.back();

    auto link = [&](int tgt) {
      blk.succs.push_back(tgt);
      f.blocks[tgt].preds.push_back(bi);
    };

    if (last.op == "BRANCH" && last.extra_pair) {
      auto it = f.label_idx.find(last.extra_a);
      if (it != f.label_idx.end())
        link(it->second);
      it = f.label_idx.find(last.extra_b);
      if (it != f.label_idx.end())
        link(it->second);
    } else if (last.op == "JUMP") {
      auto it = f.label_idx.find(last.extra_str);
      if (it != f.label_idx.end())
        link(it->second);
    } else if (last.op != "RET" && bi + 1 < n) {
      link(bi + 1);
    }
  }
}

static std::string val_repr(const IRVal &v) {
  if (v.is_null && v.kind == 'C')
    return "#null";
  if (v.kind == 'C') {
    if (v.is_null)
      return "#null";
    if (v.is_bool)
      return v.bval ? "#true" : "#false";
    if (v.is_str)
      return "#\"" + v.sval + "\"";
    return "#" + num_str(v.fval);
  }
  if (v.kind == 'T')
    return "%" + v.name;
  if (v.kind == 'V')
    return "@" + v.name;
  return "?";
}

static std::string instr_repr(const IRInstr &i) {
  std::ostringstream s;
  s << "    " << i.op;
  if (i.has_dst)
    s << " " << val_repr(i.dst);
  for (auto &op : i.operands)
    s << " " << val_repr(op);
  if (i.extra_pair)
    s << " [" << i.extra_a << "|" << i.extra_b << "]";
  else if (!i.extra_str.empty())
    s << " " << i.extra_str;
  return s.str();
}

static std::string pretty_print(const IRMod *mod) {
  std::ostringstream s;
  s << "module " << mod->name << "\n";
  for (auto &func : mod->functions) {
    s << "\nfn " << func.name << "(";
    for (int i = 0; i < (int)func.params.size(); ++i) {
      if (i)
        s << ", ";
      s << "%" << func.params[i].first << ": " << func.params[i].second;
    }
    s << ") -> " << func.return_type << " {\n";
    for (auto &blk : func.blocks) {
      s << "  " << blk.label << ":\n";
      for (auto &instr : blk.instrs)
        s << instr_repr(instr) << "\n";
    }
    s << "}\n";
  }
  return s.str();
}
