#include "common.hpp"

using TSet = std::unordered_set<std::string>;

static TSet block_defs(const IRBlock &b) {
  TSet s;
  for (auto &i : b.instrs)
    if (i.has_dst && i.dst.kind == 'T')
      s.insert(i.dst.name);
  return s;
}

static TSet block_uses(const IRBlock &b) {
  TSet s, defs;
  for (auto &i : b.instrs) {
    for (auto &op : i.operands)
      if (op.kind == 'T' && !defs.count(op.name))
        s.insert(op.name);
    if (i.has_dst && i.dst.kind == 'T')
      defs.insert(i.dst.name);
  }
  return s;
}

struct Liveness {
  std::vector<TSet> in;
  std::vector<TSet> out;
};

static Liveness compute_liveness(const IRFunc &f) {
  int n = (int)f.blocks.size();
  Liveness lv;
  lv.in.resize(n);
  lv.out.resize(n);

  std::vector<TSet> uses(n), defs(n);
  for (int i = 0; i < n; ++i) {
    uses[i] = block_uses(f.blocks[i]);
    defs[i] = block_defs(f.blocks[i]);
  }

  bool changed = true;
  while (changed) {
    changed = false;
    for (int i = n - 1; i >= 0; --i) {
      TSet new_out;
      for (int s : f.blocks[i].succs)
        for (auto &t : lv.in[s])
          new_out.insert(t);

      TSet new_in = uses[i];
      for (auto &t : new_out)
        if (!defs[i].count(t))
          new_in.insert(t);

      if (new_in != lv.in[i] || new_out != lv.out[i]) {
        lv.in[i] = std::move(new_in);
        lv.out[i] = std::move(new_out);
        changed = true;
      }
    }
  }
  return lv;
}

static std::string tset_json(const TSet &s) {
  std::ostringstream o;
  o << "[";
  bool first = true;
  for (auto &t : s) {
    if (!first)
      o << ",";
    o << "\"" << json_esc(t) << "\"";
    first = false;
  }
  o << "]";
  return o.str();
}

static std::string cfg_json(const IRMod *mod) {
  std::ostringstream o;
  o << "{\"functions\":[";
  for (int fi = 0; fi < (int)mod->functions.size(); ++fi) {
    if (fi)
      o << ",";
    auto &f = mod->functions[fi];
    o << "{\"name\":\"" << json_esc(f.name) << "\",\"blocks\":[";
    for (int bi = 0; bi < (int)f.blocks.size(); ++bi) {
      if (bi)
        o << ",";
      auto &b = f.blocks[bi];
      o << "{\"label\":\"" << json_esc(b.label) << "\"," << "\"succs\":[";
      for (int i = 0; i < (int)b.succs.size(); ++i) {
        if (i)
          o << ",";
        o << b.succs[i];
      }
      o << "],\"preds\":[";
      for (int i = 0; i < (int)b.preds.size(); ++i) {
        if (i)
          o << ",";
        o << b.preds[i];
      }
      o << "]}";
    }
    o << "]}";
  }
  o << "]}";
  return o.str();
}

static std::string liveness_json(IRMod *mod) {
  std::ostringstream o;
  o << "{\"functions\":[";
  for (int fi = 0; fi < (int)mod->functions.size(); ++fi) {
    if (fi)
      o << ",";
    auto &f = mod->functions[fi];
    build_cfg(f);
    auto lv = compute_liveness(f);
    o << "{\"name\":\"" << json_esc(f.name) << "\",\"blocks\":[";
    for (int bi = 0; bi < (int)f.blocks.size(); ++bi) {
      if (bi)
        o << ",";
      o << "{\"label\":\"" << json_esc(f.blocks[bi].label) << "\","
        << "\"live_in\":" << tset_json(lv.in[bi]) << ","
        << "\"live_out\":" << tset_json(lv.out[bi]) << "}";
    }
    o << "]}";
  }
  o << "]}";
  return o.str();
}

extern "C" {

void *ir_new(const char *json) {
  if (!json)
    return nullptr;
  try {
    return ir_parse(json);
  } catch (...) {
    return nullptr;
  }
}

void ir_free(void *h) { delete static_cast<IRMod *>(h); }

char *ir_pretty_print(void *h) {
  if (!h)
    return strdup("");
  return strdup(pretty_print(static_cast<IRMod *>(h)).c_str());
}

char *ir_build_cfg(void *h) {
  if (!h)
    return strdup("{}");
  IRMod *mod = static_cast<IRMod *>(h);
  for (auto &f : mod->functions)
    build_cfg(f);
  return strdup(cfg_json(mod).c_str());
}

char *ir_liveness(void *h) {
  if (!h)
    return strdup("{}");
  return strdup(liveness_json(static_cast<IRMod *>(h)).c_str());
}

void ir_free_str(char *s) { free(s); }
}
