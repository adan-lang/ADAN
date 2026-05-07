from typing import List, Optional

from .types import IRType
from .value import IRValue, Const, Temp, Var
from .instr import Instr, Opcode
from .block import BasicBlock
from .function import IRFunction
from .module import IRModule


class IRBuilder:
    def __init__(self, module: IRModule):
        self._module = module
        self._func: Optional[IRFunction] = None
        self._block: Optional[BasicBlock] = None

    def BeginFunction(self, name: str, params: list, return_type: IRType) -> IRFunction:
        f = IRFunction(name, params, return_type)
        self._module.AddFunction(f)
        self._func = f
        return f

    def BeginBlock(self, label: Optional[str] = None) -> BasicBlock:
        b = self._func.NewBlock(label)
        self._block = b
        return b

    def SetBlock(self, block: BasicBlock) -> None:
        self._block = block

    def _Emit(self, instr: Instr) -> None:
        self._block.Add(instr)

    def _Temp(self, type: IRType = IRType.UNKNOWN) -> Temp:
        return Temp(self._func.NewTemp(), type)

    def EmitConst(self, value, type: IRType) -> Temp:
        dst = self._Temp(type)
        self._Emit(Instr(Opcode.CONST, dst, [Const(value, type)]))
        return dst

    def EmitLoad(self, name: str, type: IRType = IRType.UNKNOWN) -> Temp:
        dst = self._Temp(type)
        self._Emit(Instr(Opcode.LOAD, dst, [Var(name, type)]))
        return dst

    def EmitStore(self, name: str, val: IRValue, type: IRType = IRType.UNKNOWN) -> None:
        self._Emit(Instr(Opcode.STORE, None, [Var(name, type), val]))

    def EmitBinary(
        self, op: str, left: IRValue, right: IRValue, type: IRType = IRType.UNKNOWN
    ) -> Temp:
        dst = self._Temp(type)
        self._Emit(Instr(Opcode.BINARY, dst, [left, right], extra=op))
        return dst

    def EmitUnary(
        self, op: str, operand: IRValue, type: IRType = IRType.UNKNOWN
    ) -> Temp:
        dst = self._Temp(type)
        self._Emit(Instr(Opcode.UNARY, dst, [operand], extra=op))
        return dst

    def EmitCall(
        self, func_name: str, args: List[IRValue], type: IRType = IRType.UNKNOWN
    ) -> Temp:
        dst = self._Temp(type)
        self._Emit(Instr(Opcode.CALL, dst, list(args), extra=func_name))
        return dst

    def EmitRet(self, val: Optional[IRValue] = None) -> None:
        self._Emit(Instr(Opcode.RET, None, [val] if val is not None else []))

    def EmitBranch(self, cond: IRValue, true_label: str, false_label: str) -> None:
        self._Emit(Instr(Opcode.BRANCH, None, [cond], extra=(true_label, false_label)))

    def EmitJump(self, label: str) -> None:
        self._Emit(Instr(Opcode.JUMP, None, [], extra=label))

    def EmitInterp(self, parts: List[IRValue]) -> Temp:
        dst = self._Temp(IRType.STRING)
        self._Emit(Instr(Opcode.INTERP, dst, list(parts)))
        return dst

    def EmitCopy(self, src: IRValue, type: IRType = IRType.UNKNOWN) -> Temp:
        dst = self._Temp(type)
        self._Emit(Instr(Opcode.COPY, dst, [src]))
        return dst
