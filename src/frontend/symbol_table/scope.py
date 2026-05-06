from typing import Dict, Optional

from .symbol import Symbol


class Scope():
	def __init__(self, Parent: Optional["Scope"] = None):
		self.Parent = Parent
		self.Symbols: Dict[str, Symbol] = {}

	def Define(self, Entry: Symbol) -> None:
		pass

	def Resolve(self, Name: str):
		pass
