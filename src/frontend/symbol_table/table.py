from .scope import Scope
from .symbol import Symbol


class SymbolTable():
	def __init__(self):
		self.GlobalScope = Scope(None)
		self.CurrentScope = self.GlobalScope

	def EnterScope(self) -> None:
		pass

	def ExitScope(self) -> None:
		pass

	def Define(self, Entry: Symbol) -> None:
		pass

	def Resolve(self, Name: str):
		pass
