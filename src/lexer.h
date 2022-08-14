#ifndef LEXER_H
#define LEXER_H

#include <string>

struct Tok {
    enum {UNKNOWN, NUMERIC, IDENT, EQUALS, END} kind;
    std::string::const_iterator begin;
    std::string::const_iterator end;
    std::string text()
        {return std::string(begin,end);}
};

// Simple lexer class to tokenise a string.
class Lexer {
public:
    Lexer(const char*) = delete;
    Lexer(std::string const& input) : begin(input.begin()), pos(input.begin()), end(input.end())
        {advance();}

    // Retrieve the next token.
    Tok Next()
        { Tok t = curr; advance(); return t; }
    // Retrieve the next token without consuming it.
    Tok Peek()
        { return curr; }

private:
    void advance();

    static bool isSpace(char c) {
        return (c == ' ' || c == '\n' || c == '\r' || c == '\t');
    }
    static bool isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    static bool isDigit(char c) {
        return (c >= '0' && c <= '9');
    }

    std::string::const_iterator begin;
    std::string::const_iterator pos;
    std::string::const_iterator end;

    Tok curr;
};
#endif // LEXER_H
