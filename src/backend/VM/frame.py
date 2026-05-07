from backend.bytecode import Chunk


class CallFrame:
    def __init__(self, Chunk: Chunk):
        self.Chunk = Chunk
        self.IP = 0
        self.Locals = {}
