from typing import List

from frontend.scanner.tokens import Token, TokenType


class Parser():
	def __init__(self, Tokens: List[Token]):
		self.Tokens = Tokens
		self.Current = 0

	def Parse(self):
		pass

	def Peek(self):
		return self.Tokens[self.Current]

	def Previous(self):
		return self.Tokens[self.Current - 1]

	def Advance(self):
		Current = self.Tokens[self.Current]
		self.Current += 1
		
		return Current

	def Match(self, *TokTypes):
		for Tok in TokTypes:
			if self.Peek().TokType == Tok:
				self.Advance()
				return True
		return False

	def Consume(self, TokType, Message: str):
		if self.Tokens[self.Current].TokType == TokType:
			self.Advance()
		else:
			pass # raise parse error later on

	def IsAtEnd(self) -> bool:
		return self.Current >= len(self.Tokens) or self.Peek().TokType == TokenType.TOKEN_EOF
