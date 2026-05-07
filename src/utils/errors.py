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
	def __init__(self,
				 Message: str = "Lexer error",
				 Line: int = None,
				 Column: int = None,
				 Position: int = None):
		super().__init__(Message, Line, Column, Position)

	@staticmethod
	def FormatCharacter(Character: str) -> str:
		if Character is None:
			return "EOF"

		EscapeMap = {
			"\n": "\\n",
			"\r": "\\r",
			"\t": "\\t",
			"\0": "\\0",
		}

		return EscapeMap.get(Character, Character)

	@classmethod
	def FromCharacter(cls,
					  Prefix: str,
					  Character: str,
					  Line: int,
					  Column: int,
					  Position: int):
		DisplayChar = cls.FormatCharacter(Character)
		Message = f"{Prefix}: '{DisplayChar}'"
		return cls(Message, Line, Column, Position)

class UnexpectedCharacterError(LexerError):
	def __init__(self,
				 Character: str,
				 Line: int,
				 Column: int,
				 Position: int):
		self.Character = Character
		DisplayChar = self.FormatCharacter(Character)
		Message = f"Unexpected character: '{DisplayChar}'"
		super().__init__(Message, Line, Column, Position)
