from typing import List

from .instr import Instr


class BasicBlock:
    __slots__ = ("label", "instrs", "preds", "succs")

    def __init__(self, label: str):
        self.label: str = label
        self.instrs: List[Instr] = []
        self.preds: List["BasicBlock"] = []
        self.succs: List["BasicBlock"] = []

    def Add(self, instr: Instr) -> None:
        self.instrs.append(instr)

    def __repr__(self) -> str:
        lines = [f"{self.label}:"]
        for i in self.instrs:
            lines.append(repr(i))
        return "\n".join(lines)
