#include <vector>
#include <string>
#include <string_view>

#include "lexer-tests.hh"
#include "lexer.hh"
#include "suite.hh"
#include "cases.hh"

std::string_view token_name(
    TokenType type
) {
    switch (type)
    {
        /* Special */
        case TOKEN_ERROR:
            return "ERROR";
        case TOKEN_EOF:
            return "EOF";
        case TOKEN_IDENTIFIER:
            return "IDENTIFIER";
        case TOKEN_WHITESPACE:
            return "WHITESPACE";

        /* Keywords */
        case TOKEN_VAR:
            return "VAR";
        case TOKEN_GLOBAL:
            return "GLOBAL";
        case TOKEN_READONLY:
            return "READONLY";
        case TOKEN_IF:
            return "IF";
        case TOKEN_FUNC:
            return "FUNC";
        case TOKEN_FOR:
            return "FOR";
        case TOKEN_WHILE:
            return "WHILE";
        case TOKEN_REPEAT:
            return "REPEAT";
        case TOKEN_UNTIL:
            return "UNTIL";
        case TOKEN_BREAK:
            return "BREAK";
        case TOKEN_PASS:
            return "PASS";
        case TOKEN_IN:
            return "IN";
        case TOKEN_INCLUDE:
            return "INCLUDE";
        case TOKEN_USING:
            return "USING";
        case TOKEN_EXPORT:
            return "EXPORT";
        case TOKEN_TODO:
            return "TODO";
        case TOKEN_AND:
            return "AND";
        case TOKEN_OR:
            return "OR";
        case TOKEN_ELSE:
            return "ELSE";

        /* Types */
        case TOKEN_STRING:
            return "STRING";
        case TOKEN_CHAR:
            return "CHAR";
        case TOKEN_NUMBER:
            return "NUMBER";
        case TOKEN_INTEGER:
            return "INTEGER";
        case TOKEN_FLOAT:
            return "FLOAT";
        case TOKEN_BOOL:
            return "BOOL";
        case TOKEN_NONE:
            return "NONE";
        case TOKEN_VOID:
            return "VOID";
        case TOKEN_AUTO:
            return "AUTO";

        /* Symbols */
        case TOKEN_COMMA:
            return "COMMA";
        case TOKEN_COLON:
            return "COLON";
        case TOKEN_LPAREN:
            return "LPAREN";
        case TOKEN_RPAREN:
            return "RPAREN";
        case TOKEN_LBRACE:
            return "LBRACE";
        case TOKEN_RBRACE:
            return "RBRACE";
        case TOKEN_LBRACKET:
            return "LBRACKET";
        case TOKEN_RBRACKET:
            return "RBRACKET";
        case TOKEN_SEMICOLON:
            return "SEMICOLON";
        case TOKEN_QUOTE:
            return "QUOTE";
        case TOKEN_APOSTROPHE:
            return "APOSTROPHE";
        case TOKEN_COMMENT:
            return "COMMENT";
        case TOKEN_MULTILINE_COMMENT:
            return "MULTILINE_COMMENT";
        case TOKEN_QUESTION:
            return "QUESTION";
        case TOKEN_PERIOD:
            return "PERIOD";

        /* Literals */
        case TOKEN_INT_LITERAL:
            return "INT_LITERAL";
        case TOKEN_FLOAT_LITERAL:
            return "FLOAT_LITERAL";
        case TOKEN_STRING_LITERAL:
            return "STRING_LITERAL";

        /* Binary Operators */
        case TOKEN_ADD:
            return "ADD";
        case TOKEN_SUB:
            return "SUB";
        case TOKEN_MUL:
            return "MUL";
        case TOKEN_POW:
            return "POW";
        case TOKEN_DIV:
            return "DIV";
        case TOKEN_REMAINDER:
            return "REMAINDER";
        case TOKEN_EQUALS:
            return "EQUALS";
        case TOKEN_NOT_EQUALS:
            return "NOT_EQUALS";
        case TOKEN_LESS:
            return "LESS";
        case TOKEN_LESS_EQUAL:
            return "LESS_EQUAL";
        case TOKEN_GREATER:
            return "GREATER";
        case TOKEN_GREATER_EQUAL:
            return "GREATER_EQUAL";
        case TOKEN_ASSIGN:
            return "ASSIGN";
        case TOKEN_ADD_ASSIGN:
            return "ADD_ASSIGN";
        case TOKEN_SUB_ASSIGN:
            return "SUB_ASSIGN";
        case TOKEN_MUL_ASSIGN:
            return "MUL_ASSIGN";
        case TOKEN_POW_ASSIGN:
            return "POW_ASSIGN";
        case TOKEN_DIV_ASSIGN:
            return "DIV_ASSIGN";
        case TOKEN_REMAINDER_ASSIGN:
            return "REMAINDER_ASSIGN";
        case TOKEN_FLOOR_DIV:
            return "FLOOR_DIV";
        case TOKEN_FLOOR_DIV_ASSIGN:
            return "FLOOR_DIV_ASSIGN";
        case TOKEN_ELVIS:
            return "ELVIS";

        /* Unary Operators */
        case TOKEN_NOT_NULL_ASSERT:
            return "NOT_NULL_ASSERT";
        case TOKEN_NOT:
            return "NOT";
        case TOKEN_NEGATE:
            return "NEGATE";

        default:
            return "UNKNOWN";
    }
}

std::string serialize_tokens( // in summary, "serializing" refers to turning a Vector of tokens
                              //  into one long string, so that way we can compare it later on.
    const std::vector<Token>& tokens
) {
    std::string serialized_result;

    for (const Token& token : tokens)
    {
        if (token.type == TokenType::TOKEN_EOF)
        {
            continue;
        }

        serialized_result += token_name(token.type);
        serialized_result += '(';
        serialized_result += token.lexeme;
        serialized_result += ')';
        serialized_result += ' ';
    }

    if (!serialized_result.empty())
    {
        serialized_result.pop_back();
    }

    return serialized_result;
}

void run_lexer_tests()
{
    TestSuite<std::vector<Token>> lexer_suite("lexer", [](std::string_view input) {
        LexerType lt{std::string(input), 0, 1, 1, false};
        Lexer lexer(lt);

        return lexer.scan();
    }, serialize_tokens);

    for (const auto& [test_name, test_data] : lexer_cases)
    {
        std::string input;
        std::string expected;

        for (const auto& [test_input, test_expected] : test_data)
        {
            input = test_input;
            expected = test_expected;
        }

        lexer_suite.add_case(test_name, input, expected);
    }

    std::vector<TestResult> results = lexer_suite.run_all();
    lexer_suite.report(results);
}