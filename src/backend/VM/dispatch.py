from backend.bytecode import Op

def Exec(Frame, Instr, Stack: list, Frames: list):
    Op_ = Instr.Op

    if   Op_ == Op.HALT:          Frames.clear()
    elif Op_ == Op.LOAD_CONST:    Stack.append(Frame.Chunk.Consts[Instr.Arg])
    elif Op_ == Op.LOAD_VAR:      Stack.append(Frame.Locals[Frame.Chunk.Locals[Instr.Arg]])
    elif Op_ == Op.STORE_VAR:     Frame.Locals[Frame.Chunk.Locals[Instr.Arg]] = Stack.pop()
    elif Op_ == Op.POP:           Stack.pop()
    elif Op_ == Op.ADD:
        B, A = Stack.pop(), Stack.pop(); Stack.append(A + B)
    elif Op_ == Op.SUB:
        B, A = Stack.pop(), Stack.pop(); Stack.append(A - B)
    elif Op_ == Op.MUL:
        B, A = Stack.pop(), Stack.pop(); Stack.append(A * B)
    elif Op_ == Op.DIV:
        B, A = Stack.pop(), Stack.pop(); Stack.append(A / B)
    elif Op_ == Op.NEG:           Stack.append(-Stack.pop())
    elif Op_ == Op.NOT:           Stack.append(not Stack.pop())
    elif Op_ == Op.EQ:
        B, A = Stack.pop(), Stack.pop(); Stack.append(A == B)
    elif Op_ == Op.LT:
        B, A = Stack.pop(), Stack.pop(); Stack.append(A < B)
    elif Op_ == Op.JUMP:          Frame.IP = Instr.Arg
    elif Op_ == Op.JUMP_IF_FALSE:
        if not Stack.pop(): Frame.IP = Instr.Arg
    elif Op_ == Op.RET:           Frames.pop()