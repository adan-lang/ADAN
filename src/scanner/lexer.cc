#include "lexer.hh"

Token Lexer::next()
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
        break;

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

        if (curr == '"')
        {
            const auto start{position};

            while (!is_eof())
            {
                advance();

                if (peek() == '"')
                {
                    advance();
                    break;
                }
            }
        }
        else if (curr == '\'')
        {
        }
        else if (curr == '`')
        {
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

            return Token{TokenKind::Ident, std::string(source.substr(start, position - start))};
        }

        return Token{TokenKind::Eof, ""};
    }
}