class ADANError(Exception):
	def __init__(self, Message: str):
		self.Message = Message
		super().__init__(Message)


class FrontendError(ADANError):
	def __init__(self,
				 Message: str,
				 Line: int = None,
				 Column: int = None,
				 Position: int = None):
		self.Line = Line
		self.Column = Column
		self.Position = Position
		super().__init__(Message)

	def __str__(self) -> str:
		Parts = [self.Message]

		if self.Line is not None and self.Column is not None:
			Parts.append(f"(line {self.Line}, column {self.Column})")

		if self.Position is not None:
			Parts.append(f"[pos {self.Position}]")

		return " ".join(Parts)


class LexerError(FrontendError):
	pass


class UnexpectedCharacterError(LexerError):
	def __init__(self,
				 Character: str,
				 Line: int,
				 Column: int,
				 Position: int):
		self.Character = Character
		DisplayChar = Character

		if Character == "\n":
			DisplayChar = "\\n"
		elif Character == "\t":
			DisplayChar = "\\t"

		Message = f"Unexpected character: '{DisplayChar}'"
		super().__init__(Message, Line, Column, Position)
