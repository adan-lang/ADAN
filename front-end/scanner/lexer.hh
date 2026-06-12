#include <iostream>

struct LexerType
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
    std::string source;
    int pos;
    int line;
    int col;
    bool abort;
    
public:
    Lexer(
        std::string source,
        int pos,
        int line,
        int col,
        bool abort = false
    );
    
    ~Lexer();
};
