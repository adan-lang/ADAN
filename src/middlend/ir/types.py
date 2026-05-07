from enum import Enum, auto


class IRType(Enum):
    NUMBER = auto()
    STRING = auto()
    BOOL = auto()
    NONE = auto()
    NIL = auto()
    UNKNOWN = auto()


def FromStr(s: str) -> IRType:
    if not s:
        return IRType.UNKNOWN
    return {
        "number": IRType.NUMBER,
        "string": IRType.STRING,
        "bool": IRType.BOOL,
        "none": IRType.NONE,
        "nil": IRType.NIL,
    }.get(s.lower(), IRType.UNKNOWN)
