from typing import List, Optional, Tuple

from .block import BasicBlock
from .types import IRType


class IRFunction:
    def __init__(
        self, name: str, params: List[Tuple[str, IRType]], return_type: IRType
    ):
        self.name = name
        self.params: List[Tuple[str, IRType]] = params
        self.return_type: IRType = return_type
        self.blocks: List[BasicBlock] = []
        self._tc: int = 0

    def NewBlock(self, label: Optional[str] = None) -> BasicBlock:
        b = BasicBlock(label or f"bb{len(self.blocks)}")
        self.blocks.append(b)
        return b

    def NewTemp(self, prefix: str = "t") -> str:
        n = f"{prefix}{self._tc}"
        self._tc += 1
        return n

    def Entry(self) -> Optional[BasicBlock]:
        return self.blocks[0] if self.blocks else None

    def __repr__(self) -> str:
        params = ", ".join(f"%{n}: {t.name}" for n, t in self.params)
        lines = [f"fn {self.name}({params}) -> {self.return_type.name}:"]
        for b in self.blocks:
            lines.append(repr(b))
        return "\n".join(lines)
