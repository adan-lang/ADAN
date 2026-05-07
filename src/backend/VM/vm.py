from backend.bytecode import Op, Chunk

class CallFrame:
    def __init__(self, Chunk: Chunk):
        self.Chunk  = Chunk
        self.IP     = 0
        self.Locals = {}

class VM:
    def __init__(self):
        self.Stack:  list           = []
        self.Frames: list[CallFrame] = []

    def Run(self, Chunk: Chunk):
        self.Frames.append(CallFrame(Chunk))
        while self.Frames:
            Frame = self.Frames[-1]
            if Frame.IP >= len(Frame.Chunk.Instrs):
                break
            Instr   = Frame.Chunk.Instrs[Frame.IP]
            Frame.IP += 1
            self._Exec(Frame, Instr)

    def _Exec(self, Frame: CallFrame, Instr):
        Op_ = Instr.Op

        if   Op_ == Op.HALT:           self.Frames.clear()
        elif Op_ == Op.LOAD_CONST:     self.Stack.append(Frame.Chunk.Consts[Instr.Arg])
        elif Op_ == Op.LOAD_VAR:       self.Stack.append(Frame.Locals[Frame.Chunk.Locals[Instr.Arg]])
        elif Op_ == Op.STORE_VAR:      Frame.Locals[Frame.Chunk.Locals[Instr.Arg]] = self.Stack.pop()
        elif Op_ == Op.POP:            self.Stack.pop()
        elif Op_ == Op.ADD:
            B, A = self.Stack.pop(), self.Stack.pop(); self.Stack.append(A + B)
        elif Op_ == Op.SUB:
            B, A = self.Stack.pop(), self.Stack.pop(); self.Stack.append(A - B)
        elif Op_ == Op.MUL:
            B, A = self.Stack.pop(), self.Stack.pop(); self.Stack.append(A * B)
        elif Op_ == Op.DIV:
            B, A = self.Stack.pop(), self.Stack.pop(); self.Stack.append(A / B)
        elif Op_ == Op.NEG:            self.Stack.append(-self.Stack.pop())
        elif Op_ == Op.NOT:            self.Stack.append(not self.Stack.pop())
        elif Op_ == Op.EQ:
            B, A = self.Stack.pop(), self.Stack.pop(); self.Stack.append(A == B)
        elif Op_ == Op.LT:
            B, A = self.Stack.pop(), self.Stack.pop(); self.Stack.append(A < B)
        elif Op_ == Op.JUMP:           Frame.IP = Instr.Arg
        elif Op_ == Op.JUMP_IF_FALSE:
            if not self.Stack.pop(): Frame.IP = Instr.Arg
        elif Op_ == Op.RET:            self.Frames.pop()