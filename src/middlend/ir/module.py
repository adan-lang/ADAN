from typing import List, Optional

from .function import IRFunction


class IRModule:
    def __init__(self, name: str = "module"):
        self.name = name
        self.functions: List[IRFunction] = []

    def AddFunction(self, func: IRFunction) -> None:
        self.functions.append(func)

    def GetFunction(self, name: str) -> Optional[IRFunction]:
        return next((f for f in self.functions if f.name == name), None)

    def __repr__(self) -> str:
        return "\n\n".join(repr(f) for f in self.functions)
