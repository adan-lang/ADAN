#include "utility/fsys.hh"

#include "tokens.hh"
#include "lexer.hh"
#include "lexer-errors.hh"
#include "fsys.hh"

// private

char Lexer::peek(
    int offset
) {
    if ((pos + offset) >= source.length())
    {
        return '\0';
    }

    else
    {
        return source[pos + offset];
    }
}

char Lexer::peek_next()
{
    return peek(1);
}

char Lexer::consume()
{
    char current_char = peek();
    
    if (current_char == '\n')
    {
        line += 1;
        col = 1;
    }

    else
    {
        col += 1;
    }

    pos += 1;

    return current_char;
}

bool Lexer::match(
    char to_match
) {
    consume();
    return peek() != to_match;
}

void Lexer::scan_comments(
    std::vector<Token>& tokens
) {
    if (peek() != '#')
    {
        return;
    }

    int start = pos;
    int start_line = line;
    int start_col = col;
    int delimiter{};

    while (peek() == '#')
    {
        consume();
        delimiter++;
    }

    if (delimiter == 1)
    {
        while (peek() != '\n' && peek() != '\0')
        {
            consume();
        }

        tokens.emplace_back(
            pos,
            start_line,
            start_col,
            source.substr(start, pos - start),
            TokenType::TOKEN_COMMENT
        );

        return;
    }

    while (true)
    {
        if (peek() == '\0')
        {
            abort = true;

            Error error;
            error.message = std::string(message(ErrorCode::UnterminatedMultilineComment));
            error.line = get_line(source, start_line);
            error.severity = SEVERITY_ERROR;
            error.line_num = start_line;
            error.col_num = start_col;

            report(error);
            return;
        }

        if (peek() == '#')
        {
            int count = 0;

            while (peek(count) == '#')
            {
                count++;
            }

            if (count == delimiter)
            {
                for (int i = 0; i < delimiter; i++)
                {
                    consume();
                }

                break;
            }
        }

        consume();
    }

    tokens.emplace_back(
        pos,
        start_line,
        start_col,
        source.substr(start, pos - start),
        TokenType::TOKEN_MULTILINE_COMMENT
    );
}

void Lexer::scan_whitespace(
    std::vector<Token>& tokens
) {
    int start = pos;
    int start_line = line;
    int start_col = col;

    if (peek() == ' ' || peek() == '\t' || peek() == '\r' || peek() == '\n')
    {
        while (
            peek() == ' '  || peek() == '\t' ||
            peek() == '\r' || peek() == '\n'
        ) {
            consume();
        }

        std::string lexeme = source.substr(start, pos - start);

        tokens.emplace_back(
            pos,
            start_line,
            start_col,
            lexeme,
            TokenType::TOKEN_WHITESPACE
        );
    }
}

Lexer::Lexer(
    LexerType lexer
) {
    this->source = lexer.source;
    this->pos = lexer.pos;
    this->line = lexer.line;
    this->col = lexer.col;
    this->abort = lexer.abort;
}

Lexer::~Lexer() { }

std::vector<Token> Lexer::scan()
{
    std::vector<Token> tokens;

    for (;;)
    {
        if (abort)
        {
            break;
        }

        for (;;)
        {
            int current_pos{pos};

            scan_whitespace(tokens);
            scan_comments(tokens);
            
            if (pos == current_pos)
            {
                break;
            }
        }

        char current = peek();

        int start_line{line};
        int start_col{col};

        if (current == '\0')
        {
            tokens.emplace_back(
                pos,
                start_line,
                start_col,
                std::string{},
                TokenType::TOKEN_EOF
            );

            break;
        }

        if (std::isalpha(current) || current == '_')
        {
            int start{pos};
            
            consume();

            while (std::isalnum(peek()) || peek() == '_')
            {
                consume();
            }

            std::string lexeme = source.substr(start, pos - start);

            auto iterator = Keywords.find(lexeme);
            auto type_iterator = Types.find(lexeme);
            
            TokenType token_type;

            if (iterator != Keywords.end())
                token_type = static_cast<TokenType>(iterator->second);
            else if (type_iterator != Types.end())
                token_type = static_cast<TokenType>(type_iterator->second);
            else
                token_type = TokenType::TOKEN_IDENTIFIER;

            tokens.emplace_back(
                pos,
                start_line,
                start_col,
                lexeme,
                token_type
            );

            continue;
        }

        if (std::isdigit(current))
        {
            int start{pos};
            bool is_float = false;

            consume();

            while (std::isdigit(peek()))
            {
                consume();
            }

            if (peek() == '.')
            {
                if (std::isdigit(peek_next()))
                {
                    is_float = true;
                    
                    consume();
                    
                    while (std::isdigit(peek()))
                    {
                        consume();
                    }
                }

                else
                {
                    Error error;
                    error.message = std::string(message(ErrorCode::IncompleteFloat));
                    error.line = get_line(source, start_line);
                    error.severity = ErrorSeverity::SEVERITY_ERROR;
                    error.line_num = start_line;
                    error.col_num = start_col;

                    report(error);
                }
            }

            if (std::isalpha(peek()) || peek() == '_')
            {
                while (std::isalnum(peek()) || peek() == '_')
                {
                    consume();
                }

                std::string lexeme = source.substr(start, pos - start);

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    lexeme,
                    TokenType::TOKEN_IDENTIFIER
                );

                continue;
            }

            std::string lexeme = source.substr(start, pos - start);

            tokens.emplace_back(
                pos,
                start_line,
                start_col,
                lexeme,
                !is_float ? TokenType::TOKEN_INT_LITERAL : TokenType::TOKEN_FLOAT_LITERAL
            );

            continue;
        }

        if (current == '"')
        {
            int start{pos};
            int str_start_line{start_line};
            int str_start_col{start_col};

            consume();

            while (true)
            {
                if (peek() == '\0')
                {
                    abort = true;

                    Error error;
                    error.message = std::string(message(ErrorCode::UnterminatedString));
                    error.line = get_line(source, start_line);
                    error.severity = SEVERITY_ERROR;
                    error.line_num = str_start_line;
                    error.col_num = str_start_col;

                    report(error);
                    break;
                }

                if (peek() == '\\')
                {
                    consume(); // '\'

                    switch (peek())
                    {
                        case 'n':
                        case 'r':
                        case 't':
                        case '"':
                        case '\'':
                        case '\\':
                            consume();
                            break;

                        default:
                        {
                            Error error;
                            error.message = std::string(message(ErrorCode::InvalidCharacter));
                            error.line = get_line(source, start_line);
                            error.severity = SEVERITY_ERROR;
                            error.line_num = start_line;
                            error.col_num = start_col;

                            report(error);

                            consume();
                            break;
                        }
                    }

                    continue;
                }

                if (peek() == '"')
                {
                    consume();
                    break;
                }

                consume();
            }

            if (!abort)
            {
                std::string lexeme = source.substr(start, pos - start);

                tokens.emplace_back(
                    pos,
                    str_start_line,
                    str_start_col,
                    lexeme,
                    TokenType::TOKEN_STRING_LITERAL
                );
            }

            continue;
        }

        int start{pos};

        switch (peek())
        {
            /* simple baby tokens */
            case ',':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_COMMA
                );

                continue;

            case '.':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_PERIOD
                );

                continue;

            case ':':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_COLON
                );

                continue;

            case '(':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_LPAREN
                );

                continue;

            case ')':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_RPAREN
                );

                continue;

            case '{':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_LBRACE
                );

                continue;

            case '}':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_RBRACE
                );

                continue;

            case '[':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_LBRACKET
                );

                continue;

            case ']':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_RBRACKET
                );

                continue;

            case ';':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_SEMICOLON
                );

                continue;

            case '\'':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_APOSTROPHE
                );

                continue;

            /* tokens dat use lotta logic */
            case '=':
                consume();

                if (peek() == '=')
                {
                    consume();

                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_EQUALS
                    );

                    continue;
                }

                else
                {
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_ASSIGN
                    );

                    continue;
                }

            case '+':
                consume();

                if (peek() == '=')
                {
                    consume();
                    
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_ADD_ASSIGN
                    );

                    continue;
                }

                else
                {
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_ADD
                    );

                    continue;
                }

            case '-':
                consume();

                if (peek() == '=')
                {
                    consume();
                    
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_SUB_ASSIGN
                    );

                    continue;
                }

                else
                {
                    auto it = tokens.rbegin();
                    
                    while (it != tokens.rend() &&
                          (it->type == TokenType::TOKEN_WHITESPACE ||
                           it->type == TokenType::TOKEN_COMMENT    ||
                           it->type == TokenType::TOKEN_MULTILINE_COMMENT))
                    {
                        ++it;
                    }

                    bool prev_ends_expression =
                        it != tokens.rend() &&
                       (it->type == TokenType::TOKEN_IDENTIFIER     ||
                        it->type == TokenType::TOKEN_INT_LITERAL    ||
                        it->type == TokenType::TOKEN_FLOAT_LITERAL  ||
                        it->type == TokenType::TOKEN_STRING_LITERAL ||
                        it->type == TokenType::TOKEN_RPAREN         ||
                        it->type == TokenType::TOKEN_RBRACKET);

                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        prev_ends_expression ? TokenType::TOKEN_SUB : TokenType::TOKEN_NEGATE
                    );

                    continue;
                }

            case '*':
                consume();

                if (peek() == '=')
                {
                    consume();
                    
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_MUL_ASSIGN
                    );

                    continue;
                }

                else if (peek() == '*')
                {
                    consume();
                    
                    if (peek() == '=')
                    {
                        consume();
                        
                        tokens.emplace_back(
                            pos,
                            start_line,
                            start_col,
                            source.substr(start, pos - start),
                            TokenType::TOKEN_POW_ASSIGN
                        );
                    }

                    else
                    {
                        tokens.emplace_back(
                            pos,
                            start_line,
                            start_col,
                            source.substr(start, pos - start),
                            TokenType::TOKEN_POW
                        );
                    }

                    continue;
                }

                else
                {
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_MUL
                    );

                    continue;
                }

            case '/':
                consume();

                if (peek() == '=')
                {
                    consume();
                    
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_DIV_ASSIGN
                    );

                    continue;
                }

                else if (peek() == '/')
                {
                    consume();
                    
                    if (peek() == '=')
                    {
                        consume();

                        tokens.emplace_back(
                            pos,
                            start_line,
                            start_col,
                            source.substr(start, pos - start),
                            TokenType::TOKEN_FLOOR_DIV_ASSIGN
                        );
                    }

                    else
                    {
                        tokens.emplace_back(
                            pos,
                            start_line,
                            start_col,
                            source.substr(start, pos - start),
                            TokenType::TOKEN_FLOOR_DIV
                        );
                    }

                    continue;
                }

                else
                {
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_DIV
                    );

                    continue;
                }

            case '%':
                consume();

                if (peek() == '=')
                {
                    consume();
                    
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_REMAINDER_ASSIGN
                    );

                    continue;
                }

                else
                {
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_REMAINDER
                    );

                    continue;
                }

            case '>':
                consume();

                if (peek() == '=')
                {
                    consume();
                    
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_GREATER_EQUAL
                    );

                    continue;
                }

                else
                {
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_GREATER
                    );

                    continue;
                }

            case '<':
                consume();

                if (peek() == '=')
                {
                    consume();
                    
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_LESS_EQUAL
                    );

                    continue;
                }

                else
                {
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_LESS
                    );

                    continue;
                }

            case '!':
                consume();

                if (peek() == '=')
                {
                    consume();

                    tokens.emplace_back(
                        pos, 
                        start_line, 
                        start_col, 
                        source.substr(start, pos - start), 
                        TokenType::TOKEN_NOT_EQUALS
                    );
                    
                    continue;
                }

                else if (peek() == '!')
                {
                    consume();
                    
                    tokens.emplace_back(
                        pos, 
                        start_line, 
                        start_col, 
                        source.substr(start, pos - start), 
                        TokenType::TOKEN_NOT_NULL_ASSERT
                    );
                    
                    continue;
                }

                else
                {
                    tokens.emplace_back(
                        pos, 
                        start_line, 
                        start_col, 
                        source.substr(start, pos - start), 
                        TokenType::TOKEN_NOT
                    );
                    
                    continue;
                }

            case '?':
                consume();

                if (peek() == ':')
                {
                    consume();
                    
                    tokens.emplace_back(
                        pos, 
                        start_line, 
                        start_col, 
                        source.substr(start, pos - start), 
                        TokenType::TOKEN_ELVIS
                    );

                    continue;
                }

                else
                {
                    tokens.emplace_back(
                        pos, 
                        start_line, 
                        start_col, 
                        source.substr(start, pos - start), 
                        TokenType::TOKEN_QUESTION
                    );
                    
                    continue;
                }

            default:
                Error error;
                error.message = std::string(message(ErrorCode::InvalidCharacter));
                error.line = get_line(source, start_line);
                error.severity = ErrorSeverity::SEVERITY_ERROR;
                error.line_num = start_line;
                error.col_num = start_col;

                report(error);

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    std::string(1, peek()),
                    TokenType::TOKEN_ERROR
                );

                consume();

                continue;
        }

        consume();
    }

    return tokens;
}