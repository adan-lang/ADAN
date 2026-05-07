class ASTNode:
    def __init__(self, Line: int = None, Column: int = None):
        self.Line = Line
        self.Column = Column

    def __repr__(self) -> str:
        return f"{self.__class__.__name__}({self.__dict__})"


class Expression(ASTNode):
    def __init__(self, Line: int = None, Column: int = None):
        super().__init__(Line, Column)


class Statement(ASTNode):
    def __init__(self, Line: int = None, Column: int = None):
        super().__init__(Line, Column)
