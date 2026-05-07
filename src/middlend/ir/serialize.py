import json
from typing import Dict, Optional

from .types import IRType
from .value import Const, IRValue, Temp, Var
from .instr import Instr, Opcode
from .block import BasicBlock
from .function import IRFunction
from .module import IRModule

_T = {t: t.name for t in IRType}
_F = {t.name: t for t in IRType}


def _EncVal(v: Optional[IRValue]) -> Optional[Dict]:
    if v is None:
        return None
    if isinstance(v, Const):
        r = v.value
        if not isinstance(r, (bool, int, float, type(None))):
            r = str(r) if r is not None else None
        return {"k": "C", "t": _T[v.type], "v": r}
    return {"k": "T" if isinstance(v, Temp) else "V", "t": _T[v.type], "n": v.name}


def _DecVal(d: Optional[Dict]) -> Optional[IRValue]:
    if d is None:
        return None
    t = _F.get(d.get("t", "UNKNOWN"), IRType.UNKNOWN)
    k = d.get("k", "?")
    if k == "C":
        return Const(d.get("v"), t)
    return Temp(d.get("n", ""), t) if k == "T" else Var(d.get("n", ""), t)


def _EncInstr(i: Instr) -> Dict:
    ex = (
        {"a": i.extra[0], "b": i.extra[1]}
        if isinstance(i.extra, tuple)
        else (str(i.extra) if i.extra is not None else None)
    )
    return {
        "o": i.op.name,
        "d": _EncVal(i.dst),
        "ops": [_EncVal(o) for o in i.operands],
        "x": ex,
    }


def _DecInstr(d: Dict) -> Instr:
    ex = d.get("x")
    if isinstance(ex, dict):
        ex = (ex["a"], ex["b"])
    return Instr(
        Opcode[d["o"]], _DecVal(d.get("d")), [_DecVal(o) for o in d.get("ops", [])], ex
    )


def _EncFunc(f: IRFunction) -> Dict:
    return {
        "n": f.name,
        "p": [{"n": n, "t": t.name} for n, t in f.params],
        "r": f.return_type.name,
        "b": [{"l": b.label, "i": [_EncInstr(i) for i in b.instrs]} for b in f.blocks],
    }


def _DecFunc(d: Dict) -> IRFunction:
    rt = _F.get(d.get("r", "UNKNOWN"), IRType.UNKNOWN)
    params = [(p["n"], _F.get(p["t"], IRType.UNKNOWN)) for p in d.get("p", [])]
    f = IRFunction(d["n"], params, rt)
    for bd in d.get("b", []):
        b = BasicBlock(bd["l"])
        for id_ in bd.get("i", []):
            b.instrs.append(_DecInstr(id_))
        f.blocks.append(b)
    return f


def Encode(mod: IRModule) -> str:
    return json.dumps(
        {"n": mod.name, "f": [_EncFunc(f) for f in mod.functions]},
        separators=(",", ":"),
    )


def Decode(s: str) -> IRModule:
    d = json.loads(s)
    mod = IRModule(d.get("n", "module"))
    for fd in d.get("f", []):
        mod.functions.append(_DecFunc(fd))
    return mod
