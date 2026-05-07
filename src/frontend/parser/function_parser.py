from .parser import Parser
from .expression_parser import ParseExpression

def ParseFunctionDef(State: Parser):
	if State.IsAtEnd() or State.Peek().Lexeme != "function":
		Token = State.Peek() if not State.IsAtEnd() else None
		Message = "Expected 'function' to start function-statement"
		if Token is None:
			raise ValueError(f"{Message} at EOF")
		raise ValueError(f"{Message} at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'")
	State.Advance()

	# function <IDENTIFIER>():
	# function <IDENTIFIER>() -> <OPTIONAL_TYPE>:
	# function <IDENTIFIER>(<OPTIONAL_NEW_IDENT_1>):
	# function <IDENTIFIER>(<OPTIONAL_NEW_IDENT_1>: <OPTIONAL_TYPE>, ...):

	