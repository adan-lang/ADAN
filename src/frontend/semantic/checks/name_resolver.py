from typing import List

from frontend.ast.base import ASTNode
from frontend.semantic.analyzer import SemanticAnalyzer
from frontend.semantic.errors import NameResolutionError, RedefinitionError


def ResolveNames(Nodes: List[ASTNode]):
	Analyzer = SemanticAnalyzer()
	Analyzer.Analyze(Nodes)
	return [Err for Err in Analyzer.Errors if isinstance(Err, (NameResolutionError, RedefinitionError))]
