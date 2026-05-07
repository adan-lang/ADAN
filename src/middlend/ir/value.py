from .types import IRType


class IRValue:
    __slots__ = ("type",)

    def __init__(self, type: IRType = IRType.UNKNOWN):
        self.type = type


class Const(IRValue):
    __slots__ = ("type", "value")

    def __init__(self, value, type: IRType = IRType.UNKNOWN):
        super().__init__(type)
        self.value = value

    def __repr__(self) -> str:
        return f"#{self.value}"


class Temp(IRValue):
    __slots__ = ("type", "name")

    def __init__(self, name: str, type: IRType = IRType.UNKNOWN):
        super().__init__(type)
        self.name = name

    def __repr__(self) -> str:
        return f"%{self.name}"


class Var(IRValue):
    __slots__ = ("type", "name")

    def __init__(self, name: str, type: IRType = IRType.UNKNOWN):
        super().__init__(type)
        self.name = name

    def __repr__(self) -> str:
        return f"@{self.name}"
