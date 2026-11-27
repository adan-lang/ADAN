#include "scanner.hh"

#include <cctype>
#include <format>
#include <string>
#include <utility>
#include <vector>

#include "tokens.hh"

Token Lexer::lex()
{
    skip_spaces();
    skip_comments();
    token_line = line;
    token_column = column;

    switch (const auto curr{peek()}; curr)
    {
    case '\0':
        return Token{.kind = TokenKind::Eof, .lexeme = ""};

    case '+':
        advance();
        if (peek() == '+')
        {
            advance();
            return Token{.kind = TokenKind::Increment, .lexeme = "++"};
        }
        if (peek() == '=')
        {
            advance();
            return Token{.kind = TokenKind::PlusAssign, .lexeme = "+="};
        }
        return Token{.kind = TokenKind::Plus, .lexeme = "+"};

    case '-':
        advance();
        if (peek() == '>')
        {
            advance();
            return Token{.kind = TokenKind::Arrow, .lexeme = "->"};
        }
        if (peek() == '-')
        {
            advance();
            return Token{.kind = TokenKind::Decrement, .lexeme = "--"};
        }
        if (peek() == '=')
        {
            advance();
            return Token{.kind = TokenKind::MinusAssign, .lexeme = "-="};
        }
        return Token{.kind = TokenKind::Minus, .lexeme = "-"};

    case '*':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{.kind = TokenKind::MultiplyAssign, .lexeme = "*="};
        }
        return Token{.kind = TokenKind::Multiply, .lexeme = "*"};

    case '/':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{.kind = TokenKind::DivideAssign, .lexeme = "/="};
        }
        return Token{.kind = TokenKind::Divide, .lexeme = "/"};

    case '^':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{.kind = TokenKind::PowerAssign, .lexeme = "^="};
        }
        return Token{.kind = TokenKind::Power, .lexeme = "^"};

    case '%':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{.kind = TokenKind::ModuloAssign, .lexeme = "%="};
        }
        return Token{.kind = TokenKind::Modulo, .lexeme = "%"};

    case '>':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{.kind = TokenKind::GreaterEqual, .lexeme = ">="};
        }
        return Token{.kind = TokenKind::GreaterThan, .lexeme = ">"};

    case '<':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{.kind = TokenKind::LessEqual, .lexeme = "<="};
        }
        if (peek() == '-')
        {
            advance();
            return Token{.kind = TokenKind::BackArrow, .lexeme = "<-"};
        }
        return Token{.kind = TokenKind::LessThan, .lexeme = "<"};

    case '=':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{.kind = TokenKind::EqualEqual, .lexeme = "=="};
        }
        return Token{.kind = TokenKind::Equal, .lexeme = "="};

    case ';':
        advance();
        return Token{.kind = TokenKind::SemiColon, .lexeme = ";"};

    case '#':
        advance();
        return Token{.kind = TokenKind::Hashtag, .lexeme = "#"};

    case '$':
        advance();
        return Token{.kind = TokenKind::Dollar, .lexeme = "$"};

    case '(':
        advance();
        return Token{.kind = TokenKind::LParen, .lexeme = "("};

    case ')':
        advance();
        return Token{.kind = TokenKind::RParen, .lexeme = ")"};

    case '[':
        advance();
        return Token{.kind = TokenKind::LBracket, .lexeme = "["};

    case ']':
        advance();
        return Token{.kind = TokenKind::RBracket, .lexeme = "]"};

    case '{':
        advance();
        return Token{.kind = TokenKind::LBrace, .lexeme = "{"};

    case '}':
        advance();
        return Token{.kind = TokenKind::RBrace, .lexeme = "}"};

    case '.':
        if (peek(1) == '.' && peek(2) == '.')
        {
            advance();
            advance();
            advance();
            return Token{.kind = TokenKind::Ellipsis, .lexeme = "..."};
        }
        report_error("Unexpected character '.'");

    case ',':
        advance();
        return Token{.kind = TokenKind::Comma, .lexeme = ","};

    case '!':
        if (peek(1) == '=' && peek(2) == '=')
        {
            advance();
            advance();
            advance();
            return Token{.kind = TokenKind::NotEqualEqual, .lexeme = "!=="};
        }
        advance();
        return Token{.kind = TokenKind::Not, .lexeme = "!"};

    default:
        if (std::isdigit(curr))
        {
            const auto start{position};
            bool is_float{};

            while (!is_eof())
            {
                if (std::isdigit(source[position]))
                {
                    advance();
                }
                else if (source[position] == '.' && !is_float && position + 1 < source.length() && std::isdigit(source[position + 1]))
                {
                    is_float = true;
                    advance();
                }
                else
                {
                    break;
                }
            }

            const std::string value{source.substr(start, position - start)};
            return Token{.kind = is_float ? TokenKind::Float : TokenKind::Int, .lexeme = value};
        }

        if (curr == '"' || curr == '\'')
        {
            const char delim{curr};
            advance();

            std::string contents;

            while (!is_eof() && peek() != delim)
            {
                if (peek() == '\\')
                {
                    advance();
                    contents += unescape(peek());
                    advance();
                }
                else
                {
                    contents += peek();
                    advance();
                }
            }

            if (is_eof())
                report_error("Unterminated string literal");
            else
                advance();

            return Token{.kind = TokenKind::String, .lexeme = contents};
        }

        if (curr == '`')
        {
            advance();

            std::string segment;
            int segment_line{line};
            int segment_column{column};
            std::vector<char> stack{'`'};

            while (!is_eof() && !stack.empty())
            {
                if (peek() == '\\' && stack.back() == '`')
                {
                    advance();
                    segment += unescape(peek());
                    advance();
                    continue;
                }

                if (const char c{peek()}; c == '`')
                {
                    if (stack.back() == '`')
                    {
                        stack.pop_back();
                        if (stack.empty())
                            break;

                        // segment += c;
                        advance();
                    }
                    else
                    {
                        if (!segment.empty())
                            segment.clear();

                        const auto nested_start{position};
                        const int nested_line{line};
                        const int nested_column{column};
                        advance();

                        // @todo kill myself
                        std::vector<char> local_stack{'`'};

                        while (!is_eof() && !local_stack.empty())
                        {
                            if (peek() == '\\')
                            {
                                advance();
                                advance();
                                continue;
                            }

                            if (peek() == '`')
                            {
                                if (local_stack.back() == '`')
                                    local_stack.pop_back();
                                else
                                    local_stack.push_back('`');
                                if (!local_stack.empty())
                                    advance();
                            }
                            else if (peek() == '$' && peek(1) == '{' && local_stack.back() == '`')
                            {
                                local_stack.push_back('{');

                                advance();
                                advance();
                            }
                            else if (peek() == '}' && local_stack.back() == '{')
                            {
                                local_stack.pop_back();

                                advance();
                            }
                            else
                            {
                                advance();
                            }
                        }

                        advance();

                        const std::string nested{source.substr(nested_start, position - nested_start)};

                        Lexer nested_lexer(nested, diagnostics, nested_line, nested_column);

                        auto t = nested_lexer.next();
                        while (t.kind != TokenKind::Eof)
                        {
                            pending.push(t);
                            t = nested_lexer.next();
                        }
                    }
                }
                else if (c == '$' && peek(1) == '{' && stack.back() == '`')
                {
                    if (!segment.empty())
                    {
                        pending.push(Token{.kind = TokenKind::TemplateString, .lexeme = segment, .line = segment_line, .column = segment_column});
                        segment.clear();
                    }

                    const int interpolation_line{line};
                    const int interpolation_column{column};
                    advance();
                    advance();

                    pending.push(Token{.kind = TokenKind::InterpolateStart, .lexeme = "${", .line = interpolation_line, .column = interpolation_column});
                    segment_line = line;
                    segment_column = column;
                    stack.push_back('{');
                }
                else if (c == '{' && stack.back() == '{')
                {
                    stack.push_back('{');
                    advance();
                    segment += c;
                }
                else if (c == '}' && stack.back() == '{')
                {
                    stack.pop_back();

                    // std::println("test ub bro plz {}", stack.size());
                    // std::println("stack after pop:");
                    // for (auto c : stack)
                    //     std::println("  '{}'", c);

                    if (stack.back() == '`')
                    {
                        // std::println("}} fired at position {}: '{}'", position,
                        // source.substr(position, 20));

                        const std::string expr{segment};
                        segment.clear();

                        // std::println("expr: '{}'", expr);

                        if (!expr.empty())
                        {
                            Lexer inter_lexer(expr, diagnostics, segment_line, segment_column);

                            auto next_token{inter_lexer.next()};
                            while (next_token.kind != TokenKind::Eof)
                            {
                                pending.push(next_token);
                                next_token = inter_lexer.next();
                            }
                        }

                        pending.push(Token{.kind = TokenKind::InterpolateEnd, .lexeme = "}", .line = line, .column = column});
                        advance();
                        segment_line = line;
                        segment_column = column;
                    }
                    else
                    {
                        segment += c;
                        advance();
                    }
                }
                else
                {
                    segment += c;
                    advance();
                }
            }

            if (is_eof())
                report_error("Unterminated template literal");
            else
                advance();

            pending.push(Token{.kind = TokenKind::TemplateString, .lexeme = segment, .line = segment_line, .column = segment_column});

            auto first = pending.front();
            pending.pop();

            return first;
        }

        for (const auto &[key, value] : keywords)
        {
            if (value == source.substr(position, value.length()) && !isalnum(source[position + value.length()]))
            {
                for (std::size_t i{}; i < value.length(); ++i)
                    advance();
                return Token{.kind = key, .lexeme = std::string(value)};
            }
        }

        if (std::isalnum(curr) || curr == '_')
        {
            const auto start{position};
            while (!is_eof() && (std::isalnum(source[position]) || source[position] == '_'))
                advance();

            std::string value{source.substr(start, position - start)};
            if (value == "true" || value == "false")
                return Token{.kind = TokenKind::Bool, .lexeme = std::move(value)};

            return Token{.kind = TokenKind::Ident, .lexeme = std::move(value)};
        }

        report_error(std::format("Unexpected character '{}'", curr));
    }
}