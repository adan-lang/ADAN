from enum import IntEnum, auto


class Op(IntEnum):
    LOAD_CONST = auto()  # push constant[arg] onto stack
    LOAD_VAR = auto()  # push locals[arg] onto stack
    STORE_VAR = auto()  # pop stack → locals[arg]

    ADD = auto()  # pop b, pop a, push a + b
    SUB = auto()  # pop b, pop a, push a - b
    MUL = auto()  # pop b, pop a, push a * b
    DIV = auto()  # pop b, pop a, push a / b
    NEG = auto()  # pop a, push -a
    NOT = auto()  # pop a, push not a

    EQ = auto()  # pop b, pop a, push a == b
    LT = auto()  # pop b, pop a, push a < b

    JUMP = auto()  # unconditional jump to arg
    JUMP_IF_FALSE = auto()  # pop condition, jump to arg if falsy

    CALL = auto()  # call function with arg count
    RET = auto()  # return top of stack

    POP = auto()  # discard top of stack
    HALT = auto()  # stop execution
