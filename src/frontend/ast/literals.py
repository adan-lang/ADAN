from .base import Expression


class NumberLiteral(Expression):
    def __init__(self, Value=None):
        self.Value = Value


class StringLiteral(Expression):
    def __init__(self, Value=None):
        self.Value = Value


class BoolLiteral(Expression):
    def __init__(self, Value=None):
        self.Value = Value


class NoneLiteral(Expression):
    def __init__(self):
        self.Value = None


class NilLiteral(Expression):
    def __init__(self):
        self.Value = None
