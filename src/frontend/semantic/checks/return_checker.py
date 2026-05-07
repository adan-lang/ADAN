from typing import List

from frontend.ast.base import ASTNode
from frontend.ast.statements import FunctionDefStmt
from frontend.semantic.analyzer import SemanticAnalyzer
from frontend.semantic.errors import InvalidReturnError, MissingReturnError


def CheckReturns(Nodes: List[ASTNode]):
	Analyzer = SemanticAnalyzer()
	Analyzer.Analyze(Nodes)
	Errors = [Err for Err in Analyzer.Errors if isinstance(Err, InvalidReturnError)]

	for Node in Nodes:
		if isinstance(Node, FunctionDefStmt):
			ExpectReturn = len(Node.ReturnTypes) > 0 and not (len(Node.ReturnTypes) == 1 and Node.ReturnTypes[0].Lexeme in {"none", "nil"})
			if ExpectReturn and not Node.ReturnExpr:
				Line = Node.Name.Line if Node.Name else None
				Column = Node.Name.Column if Node.Name else None
				Errors.append(MissingReturnError("missing return expression for function", Line, Column))
	return Errors
