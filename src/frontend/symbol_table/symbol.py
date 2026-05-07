class Symbol():
	def __init__(self, Name: str, SymbolType: str = "", Value=None):
		self.Name = Name
		self.SymbolType = SymbolType
		self.Value = Value
		self.Line = None
		self.Column = None
