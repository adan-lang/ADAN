from typing import Dict, List

from frontend.ast.base import ASTNode
from frontend.ast.statements import (
    AssignmentStmt,
    ExpressionStmt,
    FunctionDefStmt,
    IfStmt,
    PassStmt,
    ReturnStmt,
)
from frontend.ast.expressions import (
    BinaryExpr,
    CallExpr,
    IdentifierExpr,
    InterpolationExpr,
    LiteralExpr,
    UnaryExpr,
)
from frontend.ast.literals import (
    BoolLiteral,
    NilLiteral,
    NoneLiteral,
    NumberLiteral,
    StringLiteral,
)
from frontend.scanner.tokens import TokenType

from .builder import IRBuilder
from .module import IRModule
from .types import IRType, FromStr
from .value import Const, IRValue, Temp


class Lowering:
    def __init__(self):
        self._module = IRModule()
        self._builder = IRBuilder(self._module)
        self._env: Dict[str, IRValue] = {}

    def Lower(self, nodes: List[ASTNode]) -> IRModule:
        for node in nodes:
            if isinstance(node, FunctionDefStmt):
                self._LowerFunc(node)
        return self._module

    def _LowerFunc(self, node: FunctionDefStmt) -> None:
        name = node.Name.Lexeme if node.Name else "__anon__"
        params = [
            (
                pname.Lexeme if hasattr(pname, "Lexeme") else str(pname),
                (
                    FromStr(ptype.Lexeme if hasattr(ptype, "Lexeme") else str(ptype))
                    if ptype
                    else IRType.UNKNOWN
                ),
            )
            for pname, ptype in node.Parameters
        ]
        ret_type = IRType.UNKNOWN
        if node.ReturnTypes:
            t = node.ReturnTypes[0]
            ret_type = FromStr(t.Lexeme if hasattr(t, "Lexeme") else str(t))
        elif node.InferredReturnType:
            ret_type = FromStr(node.InferredReturnType)

        self._builder.BeginFunction(name, params, ret_type)
        self._builder.BeginBlock("entry")

        saved = dict(self._env)
        for pname, ptype in params:
            self._env[pname] = Temp(pname, ptype)
        for stmt in node.Body:
            self._LowerStmt(stmt)

        if node.ReturnExpr:
            self._builder.EmitRet(self._LowerExpr(node.ReturnExpr))
        elif not any(isinstance(s, ReturnStmt) for s in node.Body):
            self._builder.EmitRet(None)

        self._env = saved

    def _LowerStmt(self, node: ASTNode) -> None:
        if isinstance(node, AssignmentStmt):
            name = node.Name.Lexeme if hasattr(node.Name, "Lexeme") else str(node.Name)
            val = self._LowerExpr(node.Value)
            self._builder.EmitStore(name, val)
            self._env[name] = val
        elif isinstance(node, IfStmt):
            self._LowerIf(node)
        elif isinstance(node, ReturnStmt):
            self._builder.EmitRet(
                self._LowerExpr(node.Expression) if node.Expression else None
            )
        elif isinstance(node, ExpressionStmt):
            self._LowerExpr(node.Expression)

    def _LowerIf(self, node: IfStmt) -> None:
        func = self._builder._func
        cond = self._LowerExpr(node.Condition)
        then_b = func.NewBlock()
        else_b = func.NewBlock() if node.ElseBranch else None
        merge_b = func.NewBlock()
        self._builder.EmitBranch(
            cond, then_b.label, else_b.label if else_b else merge_b.label
        )
        self._builder.SetBlock(then_b)
        for stmt in node.ThenBranch:
            self._LowerStmt(stmt)
        self._builder.EmitJump(merge_b.label)
        if else_b:
            self._builder.SetBlock(else_b)
            for stmt in node.ElseBranch:
                self._LowerStmt(stmt)
            self._builder.EmitJump(merge_b.label)
        self._builder.SetBlock(merge_b)

    def _LowerExpr(self, node: ASTNode) -> IRValue:
        if node is None:
            return Const(None, IRType.NONE)
        if isinstance(node, NumberLiteral):
            v = node.Value
            if isinstance(v, str):
                v = float(v) if "." in v else int(v)
            return self._builder.EmitConst(v, IRType.NUMBER)
        if isinstance(node, StringLiteral):
            return self._builder.EmitConst(node.Value, IRType.STRING)
        if isinstance(node, BoolLiteral):
            return self._builder.EmitConst(bool(node.Value), IRType.BOOL)
        if isinstance(node, NoneLiteral):
            return self._builder.EmitConst(None, IRType.NONE)
        if isinstance(node, NilLiteral):
            return self._builder.EmitConst(None, IRType.NIL)
        if isinstance(node, LiteralExpr):
            tok = node.Value
            if not hasattr(tok, "TokType"):
                return Const(tok, IRType.UNKNOWN)
            tt = tok.TokType
            if tt == TokenType.TOKEN_NUMBER:
                v = float(tok.Lexeme) if "." in tok.Lexeme else int(tok.Lexeme)
                return self._builder.EmitConst(v, IRType.NUMBER)
            if tt == TokenType.TOKEN_STRING:
                return self._builder.EmitConst(tok.Lexeme, IRType.STRING)
            if tt in {TokenType.TOKEN_TRUE, TokenType.TOKEN_FALSE}:
                return self._builder.EmitConst(tt == TokenType.TOKEN_TRUE, IRType.BOOL)
            if tt == TokenType.TOKEN_NONE:
                return self._builder.EmitConst(None, IRType.NONE)
            if tt == TokenType.TOKEN_NIL:
                return self._builder.EmitConst(None, IRType.NIL)
            return Const(tok.Lexeme, IRType.UNKNOWN)
        if isinstance(node, IdentifierExpr):
            name = node.Name.Lexeme if hasattr(node.Name, "Lexeme") else str(node.Name)
            return self._env.get(name) or self._builder.EmitLoad(name)
        if isinstance(node, BinaryExpr):
            op = (
                node.Operator.Lexeme
                if hasattr(node.Operator, "Lexeme")
                else str(node.Operator)
            )
            return self._builder.EmitBinary(
                op, self._LowerExpr(node.Left), self._LowerExpr(node.Right)
            )
        if isinstance(node, UnaryExpr):
            op = (
                node.Operator.Lexeme
                if hasattr(node.Operator, "Lexeme")
                else str(node.Operator)
            )
            return self._builder.EmitUnary(op, self._LowerExpr(node.Operand))
        if isinstance(node, CallExpr):
            callee = (
                node.Callee.Lexeme
                if hasattr(node.Callee, "Lexeme")
                else str(node.Callee)
            )
            return self._builder.EmitCall(
                callee, [self._LowerExpr(a) for a in node.Arguments]
            )
        if isinstance(node, InterpolationExpr):
            parts = (
                [self._LowerExpr(p) for p in node.Value]
                if isinstance(node.Value, list)
                else ([self._LowerExpr(node.Value)] if node.Value is not None else [])
            )
            return self._builder.EmitInterp(parts)
        return Const(None, IRType.UNKNOWN)
