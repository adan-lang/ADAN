from backend.bytecode import Op


def BinaryOp(Symbol: str) -> Op:
    return {
        "+": Op.ADD,
        "-": Op.SUB,
        "*": Op.MUL,
        "/": Op.DIV,
        "==": Op.EQ,
        "<": Op.LT,
    }.get(Symbol, Op.ADD)
