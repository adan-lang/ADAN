from backend.bytecode import Chunk
from middlend.ir.module import IRModule
from .instr_lower import LowerInstr


class Lowering:
    def Lower(self, Mod: IRModule) -> list[Chunk]:
        return [self._LowerFunc(F) for F in Mod.functions]

    def _LowerFunc(self, Func) -> Chunk:
        C = Chunk(Func.name)

        for Param, _ in Func.params:
            C.EmitLocal(Param)

        LabelMap: dict[str, int] = {}
        Patches: list[tuple[int, str]] = []

        for Block in Func.blocks:
            LabelMap[Block.label] = len(C.Instrs)
            for IR in Block.instrs:
                LowerInstr(C, IR, Patches)

        for Idx, Label in Patches:
            C.Instrs[Idx].Arg = LabelMap[Label]

        return C
