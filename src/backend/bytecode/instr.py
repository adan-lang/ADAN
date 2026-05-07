from dataclasses import dataclass
from .opcode import Op

@dataclass
class Instr:
    Op: Op
    Arg: int = 0