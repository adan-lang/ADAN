class SemanticError(Exception):
    def __init__(self, Message: str, Line: int = None, Column: int = None):
        self.Message = Message
        self.Line = Line
        self.Column = Column
        super().__init__(Message)

    def __str__(self) -> str:
        if self.Line is None or self.Column is None:
            return self.Message
        return f"{self.Message} at line {self.Line}, column {self.Column}"


class TypeMismatchError(SemanticError):
    def __init__(self, Message: str, Line: int = None, Column: int = None):
        super().__init__(Message, Line, Column)


class NameResolutionError(SemanticError):
    def __init__(self, Message: str, Line: int = None, Column: int = None):
        super().__init__(Message, Line, Column)


class MissingReturnError(SemanticError):
    def __init__(self, Message: str, Line: int = None, Column: int = None):
        super().__init__(Message, Line, Column)


class InvalidReturnError(SemanticError):
    def __init__(self, Message: str, Line: int = None, Column: int = None):
        super().__init__(Message, Line, Column)


class RedefinitionError(SemanticError):
    def __init__(self, Message: str, Line: int = None, Column: int = None):
        super().__init__(Message, Line, Column)
