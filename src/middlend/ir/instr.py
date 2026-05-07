from enum import Enum, auto
from typing import List, Optional

from .value import IRValue, Temp


class Opcode(Enum):
    CONST = auto()
    COPY = auto()
    BINARY = auto()
    UNARY = auto()
    CALL = auto()
    RET = auto()
    BRANCH = auto()
    JUMP = auto()
    LOAD = auto()
    STORE = auto()
    INTERP = auto()
    NOP = auto()


class Instr:
    __slots__ = ("op", "dst", "operands", "extra")

    def __init__(
        self,
        op: Opcode,
        dst: Optional[Temp] = None,
        operands: Optional[List[IRValue]] = None,
        extra=None,
    ):
        self.op = op
        self.dst = dst
        self.operands: List[IRValue] = operands if operands is not None else []
        self.extra = extra

    def __repr__(self) -> str:
        parts = [self.op.name]
        if self.dst:
            parts.append(str(self.dst))
        for o in self.operands:
            parts.append(str(o))
        if self.extra is not None:
            parts.append(str(self.extra))
        return "  " + " ".join(parts)
