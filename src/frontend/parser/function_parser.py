from __future__ import annotations
from typing import TYPE_CHECKING
if TYPE_CHECKING:
    from .parser import Parser
from .expression_parser import ParseExpression
from frontend.ast.statements import FunctionDefStmt


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

	if State.IsAtEnd():
		raise ValueError("Expected identifier after 'function' at EOF")
	NameToken = State.Advance()
	if not NameToken.Lexeme.isidentifier():
		raise ValueError(f"Expected identifier after 'function' at line {NameToken.Line}, column {NameToken.Column}: got '{NameToken.Lexeme}'")

	if State.IsAtEnd() or State.Peek().Lexeme != "(":
		Token = State.Peek() if not State.IsAtEnd() else None
		Message = "Expected '(' after identifier"
		if Token is None:
			raise ValueError(f"{Message} at EOF")
		raise ValueError(f"{Message} at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'")
	State.Advance()

	TYPES = {"number", "string", "bool", "table", "none", "nil"}
	Parameters = []
	while not State.IsAtEnd() and State.Peek().Lexeme != ")":
		ParamName = State.Advance()
		if not ParamName.Lexeme.isidentifier():
			raise ValueError(f"Expected parameter name at line {ParamName.Line}, column {ParamName.Column}: got '{ParamName.Lexeme}'")

		ParamType = None
		if not State.IsAtEnd() and State.Peek().Lexeme == ":":
			State.Advance()
			if State.IsAtEnd() or State.Peek().Lexeme not in TYPES:
				Token = State.Peek() if not State.IsAtEnd() else None
				Message = "Expected type after ':' in parameter"
				if Token is None:
					raise ValueError(f"{Message} at EOF")
				raise ValueError(f"{Message} at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'")
			ParamType = State.Advance()

		Parameters.append((ParamName, ParamType))

		if not State.IsAtEnd() and State.Peek().Lexeme == ",":
			State.Advance()
		elif not State.IsAtEnd() and State.Peek().Lexeme != ")":
			Token = State.Peek()
			raise ValueError(f"Expected ',' or ')' in parameter list at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'")

	if State.IsAtEnd() or State.Peek().Lexeme != ")":
		Token = State.Peek() if not State.IsAtEnd() else None
		Message = "Expected ')' to close parameter list"
		if Token is None:
			raise ValueError(f"{Message} at EOF")
		raise ValueError(f"{Message} at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'")
	State.Advance()

	ReturnTypes = []
	if not State.IsAtEnd() and State.Peek().Lexeme == "->":
		State.Advance()

		if State.IsAtEnd() or State.Peek().Lexeme not in TYPES:
			Token = State.Peek() if not State.IsAtEnd() else None
			Message = "Expected return type after '->'"
			if Token is None:
				raise ValueError(f"{Message} at EOF")
			raise ValueError(f"{Message} at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'")
		ReturnTypes.append(State.Advance())

		while not State.IsAtEnd() and State.Peek().Lexeme == "or":
			State.Advance()
			if State.IsAtEnd() or State.Peek().Lexeme not in TYPES:
				Token = State.Peek() if not State.IsAtEnd() else None
				Message = "Expected type after 'or' in return type union"
				if Token is None:
					raise ValueError(f"{Message} at EOF")
				raise ValueError(f"{Message} at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'")
			ReturnTypes.append(State.Advance())

	if State.IsAtEnd() or State.Peek().Lexeme != ":":
		Token = State.Peek() if not State.IsAtEnd() else None
		Message = "Expected ':' after function signature"
		if Token is None:
			raise ValueError(f"{Message} at EOF")
		raise ValueError(f"{Message} at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'")
	State.Advance()

	NeedsReturn = len(ReturnTypes) > 0 and not (
		len(ReturnTypes) == 1 and ReturnTypes[0].Lexeme in {"none", "nil"}
	)

	Body = []
	while not State.IsAtEnd() and State.Peek().Lexeme != "return":
		Body.append(State.Advance())

	ReturnExpr = None
	if not State.IsAtEnd() and State.Peek().Lexeme == "return":
		State.Advance()
		ReturnExpr = ParseExpression(State)
		if not ReturnExpr:
			if State.IsAtEnd():
				raise ValueError("Expected expression after 'return' at EOF")
			ReturnExpr = []
			while not State.IsAtEnd():
				ReturnExpr.append(State.Advance())
	elif NeedsReturn:
		Token = State.Peek() if not State.IsAtEnd() else None
		Message = "Expected 'return' in function body"
		if Token is None:
			raise ValueError(f"{Message} at EOF")
		raise ValueError(f"{Message} at line {Token.Line}, column {Token.Column}: got '{Token.Lexeme}'")

	Node = FunctionDefStmt()
	Node.Name = NameToken
	Node.Parameters = Parameters
	Node.ReturnTypes = ReturnTypes
	Node.Body = Body
	Node.ReturnExpr = ReturnExpr
	return Node