from typing import Callable, List

from middlend.ir.module import IRModule
from middlend.ir import serialize
from . import native

PassFn = Callable[[str], str]


class PassManager:
    def __init__(self):
        self._passes: List[PassFn] = []

    def Add(self, fn: PassFn) -> "PassManager":
        self._passes.append(fn)
        return self

    def Run(self, mod: IRModule) -> IRModule:
        j = serialize.Encode(mod)
        for fn in self._passes:
            j = fn(j)
        return serialize.Decode(j)

    @staticmethod
    def Default() -> "PassManager":
        pm = PassManager()
        pm.Add(native.FoldConstants)
        pm.Add(native.CSE)
        pm.Add(native.DeadCodeElim)
        return pm

    @staticmethod
    def Full(passes: int = 3) -> "PassManager":
        pm = PassManager()
        pm.Add(lambda j: native.RunAll(j, passes))
        return pm
