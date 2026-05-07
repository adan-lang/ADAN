from .base import Expression


class IdentifierExpr(Expression):
    def __init__(self):
        self.Name = None


class LiteralExpr(Expression):
    def __init__(self):
        self.Value = None


class UnaryExpr(Expression):
    def __init__(self):
        self.Operator = None
        self.Operand = None


class BinaryExpr(Expression):
    def __init__(self):
        self.Left = None
        self.Operator = None
        self.Right = None


class CallExpr(Expression):
    def __init__(self):
        self.Callee = None
        self.Arguments = []


class InterpolationExpr(Expression):
    def __init__(self):
        self.Value = None
