#include <iostream>

typedef struct LexerType
{
    std::string source;
    int pos;
    int line;
    int col;
    bool abort;
} LexerType;

class Lexer
{
private:
    
public:
    Lexer(
        std::string source,
        int pos,
        int line,
        int col,
        bool abort
    );
    
    ~Lexer();
};
