from dataclasses import dataclass, field
from .instr import Instr


@dataclass
class Chunk:
    Name: str
    Instrs: list[Instr] = field(default_factory=list)
    Consts: list = field(default_factory=list)
    Locals: list[str] = field(default_factory=list)

    def EmitConst(self, val) -> int:
        self.Consts.append(val)
        return len(self.Consts) - 1

    def EmitLocal(self, name: str) -> int:
        if name not in self.Locals:
            self.Locals.append(name)
        return self.Locals.index(name)

    def Emit(self, instr: Instr):
        self.Instrs.append(instr)
