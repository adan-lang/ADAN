#include "utility/fsys.hh"

#include "tokens.hh"
#include "lexer.hh"

// private

char Lexer::peek(
    int offset = 0
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
    int start = pos;
    int start_line = line;
    int start_col = col;

    if (peek() == '#' && peek_next() == '#')
    {
        consume();
        consume();

        while (!(peek() == '#' && peek_next() == '#'))
        {
            if (peek() == '\0')
            {
                abort = true;
                
                Error non_terminating_comment;
                non_terminating_comment.message = "Never ending multiline comment found";
                non_terminating_comment.severity = ErrorSeverity::SEVERITY_FATAL;
                non_terminating_comment.line = line;
                non_terminating_comment.col = col;

                report(non_terminating_comment);
                break;
            }

            consume();
        }

        if (peek() != '\0')
        {
            consume();
            consume();
        }

        std::string lexeme = source.substr(start, pos - start);

        tokens.emplace_back(
            pos,
            start_line,
            start_col,
            lexeme,
            TokenType::TOKEN_MULTILINE_COMMENT
        );
    }
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
                    abort = true;

                    Error incomplete_floating_point;

                    incomplete_floating_point.message = "Incomplete floating point value found; meaning you have `<INTEGER>.`, without anything afterwards found";
                    incomplete_floating_point.severity = ErrorSeverity::SEVERITY_ERROR;
                    incomplete_floating_point.line = start_line;
                    incomplete_floating_point.col = start_col;

                    report(incomplete_floating_point);
                }
            }
            
            std::string lexeme = source.substr(start, pos - start);

            tokens.emplace_back(
                pos,
                start_line,
                start_col,
                lexeme,
                !is_float ? TokenType::TOKEN_INTEGER : TokenType::TOKEN_FLOAT
            );

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

            case '"':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_QUOTE
                );

                continue;

            case '\'':
                consume();

                tokens.emplace_back(
                    pos,
                    start_line,
                    start_col,
                    source.substr(start, pos - start),
                    TokenType::TOKEN_APOSTRAPHE
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
                    tokens.emplace_back(
                        pos,
                        start_line,
                        start_col,
                        source.substr(start, pos - start),
                        TokenType::TOKEN_SUB
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
                    Error unexpected_char;
                    unexpected_char.message = "An unexpected character found after \"!\"";
                    unexpected_char.severity = ErrorSeverity::SEVERITY_ERROR;
                    unexpected_char.line = line;
                    unexpected_char.col = col;

                    report(unexpected_char);

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
                    Error unexpected_char;
                    unexpected_char.message = "An unexpected character found after \"?\"";
                    unexpected_char.severity = ErrorSeverity::SEVERITY_ERROR;
                    unexpected_char.line = line;
                    unexpected_char.col = col;

                    report(unexpected_char);

                    continue;
                }

            default:
                Error unexpected_char;
                unexpected_char.message = "An unexpected character found";
                unexpected_char.severity = ErrorSeverity::SEVERITY_ERROR;
                unexpected_char.line = line;
                unexpected_char.col = col;

                report(unexpected_char);

                continue;
        }

        consume();
    }

    return tokens;
}