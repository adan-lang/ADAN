from backend.bytecode import Chunk
from .frame import CallFrame
from .dispatch import Exec

class VM:
    def __init__(self):
        self.Stack:  list            = []
        self.Frames: list[CallFrame] = []

    def Run(self, Chunk: Chunk):
        self.Frames.append(CallFrame(Chunk))
        while self.Frames:
            Frame = self.Frames[-1]
            if Frame.IP >= len(Frame.Chunk.Instrs):
                break
            Instr    = Frame.Chunk.Instrs[Frame.IP]
            Frame.IP += 1
            Exec(Frame, Instr, self.Stack, self.Frames)