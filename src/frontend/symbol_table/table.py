from .scope import Scope
from .symbol import Symbol


class SymbolTable():
	def __init__(self):
		self.GlobalScope = Scope(None)
		self.CurrentScope = self.GlobalScope

	def EnterScope(self) -> None:
		self.CurrentScope = Scope(self.CurrentScope)

	def ExitScope(self) -> None:
		if self.CurrentScope.Parent:
			self.CurrentScope = self.CurrentScope.Parent

	def Define(self, Entry: Symbol) -> None:
		self.CurrentScope.Define(Entry)

	def Resolve(self, Name: str):
		return self.CurrentScope.Resolve(Name)
