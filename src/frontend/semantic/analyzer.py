from typing import List
from typing import Optional

from frontend.ast.base import ASTNode
from frontend.ast.statements import FunctionDefStmt, IfStmt, ReturnStmt, AssignmentStmt, ExpressionStmt, PassStmt
from frontend.scanner.tokens import TokenType
from frontend.ast.expressions import CallExpr, IdentifierExpr, LiteralExpr, InterpolationExpr, BinaryExpr, UnaryExpr
from frontend.symbol_table.table import SymbolTable
from frontend.symbol_table.symbol import Symbol
from .errors import NameResolutionError, InvalidReturnError, RedefinitionError, MissingReturnError


class SemanticAnalyzer():
	def __init__(self):
		self.Errors: List[Exception] = []
		self.SymbolTable = SymbolTable()
		self.CurrentFunction: Optional[FunctionDefStmt] = None

	def Analyze(self, Nodes: List[ASTNode]) -> None:
		for Node in Nodes:
			self.AnalyzeNode(Node)

	def AnalyzeNode(self, Node: ASTNode) -> None:
		if isinstance(Node, FunctionDefStmt):
			self._AnalyzeFunctionDef(Node)
		elif isinstance(Node, IfStmt):
			self._AnalyzeIfStmt(Node)
		elif isinstance(Node, AssignmentStmt):
			self._AnalyzeAssignment(Node)
		elif isinstance(Node, ReturnStmt):
			self._AnalyzeReturn(Node)
		elif isinstance(Node, ExpressionStmt):
			self._AnalyzeExpressionStmt(Node)
		elif isinstance(Node, PassStmt):
			pass

	def _AnalyzeFunctionDef(self, Node: FunctionDefStmt) -> None:
		if not Node.Name:
			return

		NameStr = Node.Name.Lexeme
		Existing = self.SymbolTable.CurrentScope.Symbols.get(NameStr)
		if Existing:
			self.Errors.append(RedefinitionError(f"Function '{NameStr}' already defined", Node.Name.Line, Node.Name.Column))
			return

		Sym = Symbol(NameStr, "function")
		Sym.Line = Node.Name.Line
		Sym.Column = Node.Name.Column
		self.SymbolTable.Define(Sym)

		PrevFunc = self.CurrentFunction
		self.CurrentFunction = Node
		self.SymbolTable.EnterScope()

		for ParamName, ParamType in Node.Parameters:
			if not ParamName:
				continue
			ParamNameStr = ParamName.Lexeme
			ParamTypeStr = ParamType.Lexeme if ParamType else "unknown"
			ParamSym = Symbol(ParamNameStr, ParamTypeStr)
			ParamSym.Line = ParamName.Line
			ParamSym.Column = ParamName.Column
			self.SymbolTable.Define(ParamSym)

		if Node.Body:
			for Stmt in Node.Body:
				if isinstance(Stmt, ASTNode):
					self.AnalyzeNode(Stmt)

		if Node.ReturnExpr:
			ExprNode = Node.ReturnExpr if isinstance(Node.ReturnExpr, ASTNode) else None
			if ExprNode:
				self._AnalyzeExpr(ExprNode)
			else:
				for Expr in Node.ReturnExpr:
					if isinstance(Expr, ASTNode):
						self._AnalyzeExpr(Expr)
			if not Node.ReturnTypes:
				Node.InferredReturnType = self._InferExprType(ExprNode)
		elif len(Node.ReturnTypes) > 0 and not (len(Node.ReturnTypes) == 1 and Node.ReturnTypes[0].Lexeme in {"none", "nil"}):
			self.Errors.append(MissingReturnError("missing return expression for function", Node.Name.Line, Node.Name.Column))

		self.SymbolTable.ExitScope()
		self.CurrentFunction = PrevFunc

	def _AnalyzeIfStmt(self, Node: IfStmt) -> None:
		if Node.Condition:
			if isinstance(Node.Condition, ASTNode):
				self._AnalyzeExpr(Node.Condition)

		if Node.ThenBranch:
			self.SymbolTable.EnterScope()
			for Stmt in Node.ThenBranch:
				if isinstance(Stmt, ASTNode):
					self.AnalyzeNode(Stmt)
			self.SymbolTable.ExitScope()

		if Node.ElseBranch:
			self.SymbolTable.EnterScope()
			for Stmt in Node.ElseBranch:
				if isinstance(Stmt, ASTNode):
					self.AnalyzeNode(Stmt)
			self.SymbolTable.ExitScope()

	def _AnalyzeAssignment(self, Node: AssignmentStmt) -> None:
		if not Node.Name:
			return

		VarName = Node.Name.Lexeme
		VarSym = Symbol(VarName, "variable")
		VarSym.Line = Node.Name.Line
		VarSym.Column = Node.Name.Column
		self.SymbolTable.Define(VarSym)

		if isinstance(Node.Value, ASTNode):
			self._AnalyzeExpr(Node.Value)

	def _AnalyzeReturn(self, Node: ReturnStmt) -> None:
		if not self.CurrentFunction:
			self.Errors.append(InvalidReturnError("return statement outside of function"))
			return

		if isinstance(Node.Expression, ASTNode):
			self._AnalyzeExpr(Node.Expression)

	def _AnalyzeExpressionStmt(self, Node: ExpressionStmt) -> None:
		if isinstance(Node.Expression, ASTNode):
			self._AnalyzeExpr(Node.Expression)

	def _InferExprType(self, Expr) -> str:
		if Expr is None:
			return "none"
		if isinstance(Expr, LiteralExpr):
			Tok = Expr.Value
			if not hasattr(Tok, "TokType"):
				return "unknown"
			if Tok.TokType == TokenType.TOKEN_NUMBER:
				return "number"
			if Tok.TokType == TokenType.TOKEN_STRING:
				return "string"
			if Tok.TokType in {TokenType.TOKEN_TRUE, TokenType.TOKEN_FALSE}:
				return "bool"
			if Tok.TokType == TokenType.TOKEN_NONE:
				return "none"
			if Tok.TokType == TokenType.TOKEN_NIL:
				return "nil"
		if isinstance(Expr, InterpolationExpr):
			return "string"
		if isinstance(Expr, IdentifierExpr):
			NameStr = Expr.Name.Lexeme if hasattr(Expr.Name, "Lexeme") else str(Expr.Name)
			Sym = self.SymbolTable.Resolve(NameStr)
			if Sym:
				return Sym.SymbolType
		if isinstance(Expr, CallExpr):
			CalleeName = Expr.Callee.Lexeme if hasattr(Expr.Callee, "Lexeme") else str(Expr.Callee)
			Sym = self.SymbolTable.Resolve(CalleeName)
			if Sym and hasattr(Sym, "ReturnType"):
				return Sym.ReturnType
		return "unknown"

	def _AnalyzeExpr(self, Node: ASTNode) -> None:
		if isinstance(Node, IdentifierExpr):
			if Node.Name:
				Sym = self.SymbolTable.Resolve(Node.Name.Lexeme)
				if not Sym:
					self.Errors.append(NameResolutionError(f"Undefined identifier '{Node.Name.Lexeme}'", Node.Name.Line, Node.Name.Column))
		elif isinstance(Node, CallExpr):
			if Node.Callee:
				Sym = self.SymbolTable.Resolve(Node.Callee.Lexeme)
				if not Sym or Sym.SymbolType != "function":
					self.Errors.append(NameResolutionError(f"Undefined function '{Node.Callee.Lexeme}'", Node.Callee.Line, Node.Callee.Column))
			for Arg in Node.Arguments:
				if isinstance(Arg, ASTNode):
					self._AnalyzeExpr(Arg)
		elif isinstance(Node, BinaryExpr):
			if Node.Left and isinstance(Node.Left, ASTNode):
				self._AnalyzeExpr(Node.Left)
			if Node.Right and isinstance(Node.Right, ASTNode):
				self._AnalyzeExpr(Node.Right)
		elif isinstance(Node, UnaryExpr):
			if Node.Operand and isinstance(Node.Operand, ASTNode):
				self._AnalyzeExpr(Node.Operand)
		elif isinstance(Node, LiteralExpr):
			pass
		elif isinstance(Node, InterpolationExpr):
			pass
