from middlend.ir.module import IRModule
from middlend.ir.instr import Opcode
from middlend.ir.value import Temp


def InlineCopies(mod: IRModule) -> IRModule:
    for func in mod.functions:
        for blk in func.blocks:
            copy_map = {
                i.dst.name: i.operands[0]
                for i in blk.instrs
                if i.op == Opcode.COPY and i.dst and i.operands
            }
            for instr in blk.instrs:
                if instr.op != Opcode.COPY:
                    for idx, op in enumerate(instr.operands):
                        if isinstance(op, Temp) and op.name in copy_map:
                            instr.operands[idx] = copy_map[op.name]
    return mod


def RemoveNops(mod: IRModule) -> IRModule:
    for func in mod.functions:
        for blk in func.blocks:
            blk.instrs = [i for i in blk.instrs if i.op != Opcode.NOP]
    return mod


def RemoveUnreachable(mod: IRModule) -> IRModule:
    for func in mod.functions:
        if not func.blocks:
            continue
        visited: set = set()
        stack = [func.blocks[0].label]
        label_map = {b.label: b for b in func.blocks}
        while stack:
            lbl = stack.pop()
            if lbl in visited or lbl not in label_map:
                continue
            visited.add(lbl)
            for instr in label_map[lbl].instrs:
                if instr.op == Opcode.JUMP and instr.extra:
                    stack.append(instr.extra)
                elif instr.op == Opcode.BRANCH and isinstance(instr.extra, tuple):
                    stack.extend(instr.extra)
        func.blocks = [b for b in func.blocks if b.label in visited]
    return mod
