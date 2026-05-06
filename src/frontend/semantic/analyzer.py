from typing import List

from frontend.ast.base import ASTNode


class SemanticAnalyzer():
	def __init__(self):
		self.Errors: List[Exception] = []

	def Analyze(self, Nodes: List[ASTNode]) -> None:
		pass

	def AnalyzeNode(self, Node: ASTNode) -> None:
		pass
