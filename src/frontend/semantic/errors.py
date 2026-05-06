class SemanticError(Exception):
	pass


class TypeMismatchError(SemanticError):
	pass


class NameResolutionError(SemanticError):
	pass


class MissingReturnError(SemanticError):
	pass
