#include <vector>
#include <string>

#include "lexer.hh"
#include "suite.hh"

std::string serialize_tokens( // in summary, "serializing" refers to turning a Vector of tokens
                              //  into one long string, so that way we can compare it later on.
    const std::vector<Token>& tokens
) {
    std::string serialized_result;

    for (const Token& token : tokens)
    {
        // @todo: figure out how to concatenate strings
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

    /// LEXER TESTS

    lexer_suite.add_case("single number", "42", "NUMBER(42) EOF");

    ///

    std::vector<TestResult> results = lexer_suite.run_all();
    lexer_suite.report(results);
}