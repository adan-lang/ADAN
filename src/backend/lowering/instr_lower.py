from backend.bytecode import Chunk, Instr, Op
from middlend.ir.instr import Opcode
from .push import PushVal
from .ops import BinaryOp


def LowerInstr(C: Chunk, IR, Patches: list):
    Op_ = IR.op

    if Op_ == Opcode.CONST:
        Val = IR.operands[0].value if IR.operands else None
        C.Emit(Instr(Op.LOAD_CONST, C.EmitConst(Val)))
        if IR.dst:
            C.Emit(Instr(Op.STORE_VAR, C.EmitLocal(IR.dst.name)))

    elif Op_ in (Opcode.COPY, Opcode.LOAD):
        PushVal(C, IR.operands[0])
        if IR.dst:
            C.Emit(Instr(Op.STORE_VAR, C.EmitLocal(IR.dst.name)))

    elif Op_ == Opcode.STORE:
        PushVal(C, IR.operands[0])
        C.Emit(Instr(Op.STORE_VAR, C.EmitLocal(IR.dst.name)))

    elif Op_ == Opcode.BINARY:
        PushVal(C, IR.operands[0])
        PushVal(C, IR.operands[1])
        C.Emit(Instr(BinaryOp(IR.extra)))
        if IR.dst:
            C.Emit(Instr(Op.STORE_VAR, C.EmitLocal(IR.dst.name)))

    elif Op_ == Opcode.UNARY:
        PushVal(C, IR.operands[0])
        C.Emit(Instr(Op.NEG if IR.extra == "-" else Op.NOT))
        if IR.dst:
            C.Emit(Instr(Op.STORE_VAR, C.EmitLocal(IR.dst.name)))

    elif Op_ == Opcode.JUMP:
        Idx = len(C.Instrs)
        C.Emit(Instr(Op.JUMP, 0))
        Patches.append((Idx, IR.extra))

    elif Op_ == Opcode.BRANCH:
        PushVal(C, IR.operands[0])
        Idx = len(C.Instrs)
        C.Emit(Instr(Op.JUMP_IF_FALSE, 0))
        Patches.append((Idx, IR.extra[1]))
        Idx2 = len(C.Instrs)
        C.Emit(Instr(Op.JUMP, 0))
        Patches.append((Idx2, IR.extra[0]))

    elif Op_ == Opcode.CALL:
        for Arg in IR.operands[1:]:
            PushVal(C, Arg)
        PushVal(C, IR.operands[0])
        C.Emit(Instr(Op.CALL, len(IR.operands) - 1))
        if IR.dst:
            C.Emit(Instr(Op.STORE_VAR, C.EmitLocal(IR.dst.name)))

    elif Op_ == Opcode.RET:
        if IR.operands:
            PushVal(C, IR.operands[0])
        C.Emit(Instr(Op.RET))

    elif Op_ == Opcode.NOP:
        pass
