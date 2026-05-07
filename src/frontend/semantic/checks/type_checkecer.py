from typing import List

from frontend.ast.base import ASTNode
from frontend.ast.statements import FunctionDefStmt
from frontend.ast.expressions import LiteralExpr
from frontend.semantic.errors import TypeMismatchError


def CheckTypes(Nodes: List[ASTNode]):
    Errors = []
    for Node in Nodes:
        if (
            isinstance(Node, FunctionDefStmt)
            and Node.ReturnTypes
            and isinstance(Node.ReturnExpr, LiteralExpr)
        ):
            Allowed = {Tok.Lexeme for Tok in Node.ReturnTypes}
            Lexeme = getattr(Node.ReturnExpr.Value, "Lexeme", None)
            if Lexeme is not None:
                if Lexeme in {"true", "false"}:
                    Actual = "bool"
                elif Lexeme in {"none", "nil"}:
                    Actual = Lexeme
                elif Lexeme.startswith(('"', "'", "`")):
                    Actual = "string"
                else:
                    Actual = "number"
                if Actual not in Allowed:
                    Line = Node.Name.Line if Node.Name else None
                    Column = Node.Name.Column if Node.Name else None
                    Errors.append(
                        TypeMismatchError(
                            f"function return type '{Actual}' not in declared return union",
                            Line,
                            Column,
                        )
                    )
    return Errors
