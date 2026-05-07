from .base import Statement


class AssignmentStmt(Statement):
    def __init__(self):
        self.Name = None
        self.Value = None


class FunctionDefStmt(Statement):
    def __init__(self):
        self.Name = None
        self.Parameters = []
        self.ReturnTypes = []
        self.Body = []
        self.ReturnExpr = None
        self.InferredReturnType = None


class IfStmt(Statement):
    def __init__(self):
        self.Condition = None
        self.ThenBranch = []
        self.ElseBranch = None


class ReturnStmt(Statement):
    def __init__(self):
        self.Expression = None


class ExpressionStmt(Statement):
    def __init__(self):
        self.Expression = None


class PassStmt(Statement):
    def __init__(self):
        pass
