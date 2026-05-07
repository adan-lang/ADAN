import sys
import os

from frontend.scanner.scanner import Scanner, Scan
from frontend.parser.parser import Parser
from frontend.semantic.analyzer import SemanticAnalyzer

VERSION = "0.1.0"

HELP = f"""\
ADAN Compiler v{VERSION}

Usage:
  adan [flags] <file.adn>
  adan [flags] -f <file.adn>
  adan [flags] -d <directory>

Flags:
  -f, --file <path>         Input .adn file to compile
  -d, --directory <path>    Compile all .adn files in a directory
  -o, --output <path>       Output file or directory (default: same as input)
  -s, --silent-compile      Suppress all output except errors
  -v, --version             Print version and exit
  -h, --help                Print this help and exit
"""


def CompileFile(FilePath: str, OutputPath: str, Silent: bool) -> bool:
    if not FilePath.endswith(".adn"):
        print(f"Error: '{FilePath}' is not a .adn file.")
        return False

    if not os.path.isfile(FilePath):
        print(f"Error: file '{FilePath}' does not exist.")
        return False

    with open(FilePath, "r") as F:
        Source = F.read()

    if not Silent:
        print(f"Compiling: {FilePath}")

    Lexer = Scanner(Source, 1, 1, 1)
    Tokens = Scan(Lexer)

    LexErrors = [T for T in Tokens if T.TokType.name == "TOKEN_ERROR"]
    if LexErrors:
        for T in LexErrors:
            print(f"  Lexer error at line {T.Line}, column {T.Column}: unexpected '{T.Lexeme}'")
        return False

    State = Parser(Tokens)
    try:
        Nodes = State.Parse()
    except ValueError as E:
        print(f"  Parse error: {E}")
        return False

    Analyzer = SemanticAnalyzer()
    Analyzer.Analyze(Nodes)
    if Analyzer.Errors:
        for E in Analyzer.Errors:
            print(f"  Semantic error: {E}")
        return False

    if OutputPath is None:
        Base = os.path.splitext(FilePath)[0]
        OutputPath = Base + ".out"
    elif os.path.isdir(OutputPath):
        Base = os.path.splitext(os.path.basename(FilePath))[0]
        OutputPath = os.path.join(OutputPath, Base + ".out")

    with open(OutputPath, "w") as F:
        F.write("")

    if not Silent:
        print(f"  Output: {OutputPath}")

    return True


def ParseArgs(Args):
    FilePath = None
    OutputPath = None
    Directory = None
    Silent = False

    i = 0
    while i < len(Args):
        Arg = Args[i]

        if Arg in ("-v", "--version"):
            print(f"ADAN v{VERSION}")
            sys.exit(0)

        elif Arg in ("-h", "--help"):
            print(HELP)
            sys.exit(0)

        elif Arg in ("-s", "--silent-compile"):
            Silent = True

        elif Arg in ("-f", "--file"):
            i += 1
            if i >= len(Args):
                print("Error: -f/--file requires a path argument.")
                sys.exit(1)
            FilePath = Args[i]

        elif Arg in ("-d", "--directory"):
            i += 1
            if i >= len(Args):
                print("Error: -d/--directory requires a path argument.")
                sys.exit(1)
            Directory = Args[i]

        elif Arg in ("-o", "--output"):
            i += 1
            if i >= len(Args):
                print("Error: -o/--output requires a path argument.")
                sys.exit(1)
            OutputPath = Args[i]

        elif not Arg.startswith("-") and FilePath is None and Directory is None:
            FilePath = Arg

        else:
            print(f"Error: unknown argument '{Arg}'.")
            sys.exit(1)

        i += 1

    return FilePath, Directory, OutputPath, Silent


if __name__ == "__main__":
    Args = sys.argv[1:]

    if not Args:
        print(HELP)
        sys.exit(0)

    FilePath, Directory, OutputPath, Silent = ParseArgs(Args)

    if Directory is not None:
        if not os.path.isdir(Directory):
            print(f"Error: '{Directory}' is not a directory.")
            sys.exit(1)

        Files = [
            os.path.join(Directory, F)
            for F in os.listdir(Directory)
            if F.endswith(".adn")
        ]

        if not Files:
            print(f"No .adn files found in '{Directory}'.")
            sys.exit(0)

        Success = 0
        for F in sorted(Files):
            if CompileFile(F, OutputPath, Silent):
                Success += 1

        if not Silent:
            print(f"\n{Success}/{len(Files)} files compiled successfully.")

        sys.exit(0 if Success == len(Files) else 1)

    if FilePath is None:
        print("Error: no input file specified. Use -h for help.")
        sys.exit(1)

    OK = CompileFile(FilePath, OutputPath, Silent)
    sys.exit(0 if OK else 1)