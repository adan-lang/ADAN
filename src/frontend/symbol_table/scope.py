from typing import Dict, Optional

from .symbol import Symbol


class Scope():
	def __init__(self, Parent: Optional["Scope"] = None):
		self.Parent = Parent
		self.Symbols: Dict[str, Symbol] = {}

	def Define(self, Entry: Symbol) -> None:
		self.Symbols[Entry.Name] = Entry

	def Resolve(self, Name: str):
		if Name in self.Symbols:
			return self.Symbols[Name]
		if self.Parent:
			return self.Parent.Resolve(Name)
		return None
