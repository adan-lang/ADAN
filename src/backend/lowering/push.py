from backend.bytecode import Chunk, Instr, Op
from middlend.ir.value import Const, Temp, Var


def PushVal(C: Chunk, V):
    if isinstance(V, Const):
        C.Emit(Instr(Op.LOAD_CONST, C.EmitConst(V.value)))
    elif isinstance(V, (Temp, Var)):
        C.Emit(Instr(Op.LOAD_VAR, C.EmitLocal(V.name)))
