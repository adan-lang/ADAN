#include <print>
#include <utility>

#include "lexer.hh"

Token Lexer::lex()
{
    skip_spaces();
    skip_comments();

    switch (const auto curr{peek()}; curr)
    {
    case '+':
        advance();
        if (peek() == '+')
        {
            advance();
            return Token{TokenKind::Increment, "++"};
        }
        if (peek() == '=')
        {
            advance();
            return Token{TokenKind::PlusAssign, "+="};
        }
        return Token{TokenKind::Plus, "+"};

    case '-':
        advance();
        if (peek() == '>')
        {
            advance();
            return Token{TokenKind::Arrow, "->"};
        }
        if (peek() == '-')
        {
            advance();
            return Token{TokenKind::Decrement, "--"};
        }
        if (peek() == '=')
        {
            advance();
            return Token{TokenKind::MinusAssign, "-="};
        }
        return Token{TokenKind::Minus, "-"};

    case '*':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{TokenKind::MultiplyAssign, "*="};
        }
        return Token{TokenKind::Multiply, "*"};

    case '/':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{TokenKind::DivideAssign, "/="};
        }
        return Token{TokenKind::Divide, "/"};

    case '%':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{TokenKind::ModuloAssign, "%="};
        }
        return Token{TokenKind::Modulo, "%"};

    case '>':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{TokenKind::GreaterEqual, ">="};
        }
        return Token{TokenKind::GreaterThan, ">"};

    case '<':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{TokenKind::LessEqual, "<="};
        }
        if (peek() == '-')
        {
            advance();
            return Token{TokenKind::BackArrow, "<-"};
        }
        return Token{TokenKind::LessThan, "<"};

    case '=':
        advance();
        if (peek() == '=')
        {
            advance();
            return Token{TokenKind::EqualEqual, "=="};
        }
        return Token{TokenKind::Equal, "="};

    case ';':
        advance();
        return Token{TokenKind::SemiColon, ";"};

    case '#':
        advance();
        return Token{TokenKind::Hashtag, "#"};

    case '$':
        advance();
        return Token{TokenKind::Dollar, "$"};

    case '(':
        advance();
        return Token{TokenKind::LParen, "("};

    case ')':
        advance();
        return Token{TokenKind::RParen, ")"};

    case '[':
        advance();
        return Token{TokenKind::LBracket, "["};

    case ']':
        advance();
        return Token{TokenKind::RBracket, "]"};

    case '{':
        advance();
        return Token{TokenKind::LBrace, "{"};

    case '}':
        advance();
        return Token{TokenKind::RBrace, "}"};

        // case '"':
        //     advance();
        //     return Token{TokenKind::Quote, "\""};

        // case '\'':
        //     advance();
        //     return Token{TokenKind::Apostrophe, "'"};

    case '.':
        if (peek(1) == '.' && peek(2) == '.')
        {
            advance();
            advance();
            advance();
            return Token{TokenKind::Ellipsis, "..."};
        }
        advance();
        return Token{TokenKind::Eof, ""};

        // case '`':
        //     advance();
        //     return Token{TokenKind::Tick, "`"};

    case ',':
        advance();
        return Token{TokenKind::Comma, ","};

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
                else if (source[position] == '.' &&
                         !is_float && position + 1 < source.length() &&
                         std::isdigit(source[position + 1]))
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
            return Token{is_float ? TokenKind::Float : TokenKind::Int, value};
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
                // @todo @important HEY LILY!! do this sometime soon please!
                std::println("Unterminated string literal");
            else
                advance();

            return Token{TokenKind::String, contents};
        }

        if (curr == '`')
        {
            advance();

            std::string segment;
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

                const char c{peek()};

                if (c == '`')
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

                        Lexer nested_lexer(nested);

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
                        pending.push(Token{TokenKind::TemplateString, segment});
                        segment.clear();
                    }

                    advance();
                    advance();

                    pending.push(Token{TokenKind::InterpolateStart, "${"});
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
                        // std::println("}} fired at position {}: '{}'", position, source.substr(position, 20));

                        const std::string expr{segment};
                        segment.clear();

                        // std::println("expr: '{}'", expr);

                        if (!expr.empty())
                        {
                            Lexer inter_lexer(expr);

                            auto next_token{inter_lexer.next()};
                            while (next_token.kind != TokenKind::Eof)
                            {
                                pending.push(next_token);
                                next_token = inter_lexer.next();
                            }
                        }

                        pending.push(Token{TokenKind::InterpolateEnd, "}"});
                        advance();
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
                // @todo @important do fancy shit later
                std::println("Unterminated template literal");
            else
                advance();

            pending.push(Token{TokenKind::TemplateString, segment});

            auto first = pending.front();
            pending.pop();

            return first;
        }

        for (const auto &[key, value] : keywords)
        {
            if (value == source.substr(position, value.length()) &&
                !isalnum(source[position + value.length()]))
            {
                position += value.length();
                return Token{key, std::string(value)};
            }
        }

        if (std::isalnum(curr) || curr == '_')
        {
            const auto start{position};
            while (!is_eof() && std::isalnum(source[position]) || source[position] == '_')
                advance();

            std::string value{source.substr(start, position - start)};
            if (value == "true" || value == "false")
                return Token{TokenKind::Bool, std::move(value)};

            return Token{TokenKind::Ident, std::move(value)};
        }

        return Token{TokenKind::Eof, ""};
    }
}