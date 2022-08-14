#include "lexer.h"

void Lexer::advance() {
    // skip any whitespace
    while(pos != end && isSpace(*pos)) {
        ++pos;
    }
    curr.begin = pos;
    if (pos == end) {
        curr.kind = Tok::END;
        curr.end = pos;
        return;
    }
    char c = *pos++;
    if (isDigit(c)) {
        while(pos != end && isDigit(*pos)) {
            ++pos;
        }
        curr.kind = Tok::NUMERIC;
        curr.end = pos;
        return;
    }
    if (isAlpha(c)) {
        while(pos != end && (isAlpha(*pos) || isDigit(*pos) || *pos=='_' || *pos=='-')) {
            ++pos;
        }
        curr.kind = Tok::IDENT;
        curr.end = pos;
        return;
    }
    if (c == '=' ) {
        curr.kind = Tok::EQUALS;
        curr.end = pos;
        return;
    }

    curr.kind = Tok::UNKNOWN;
    curr.end = pos;
}


#ifdef LEXTEST
// Noddy little interactive driver for trying out the lexer eg: 
// $ g++ -ggdb -DLEXTEST -o lextest src/lexer.cpp
// $ ./lextest "foo=42 bar=12"

int main( int argc, char* argv[]) {
    for (int i=1; i<argc; ++i) {
        std::string data(argv[i]);
        Lexer lexer(data);
        while (true) {
            Tok tok = lexer.Next();
            // Show input string with token highlighted.
            printf("'%s\033[7m%s\033[0m%s'",
                std::string(data.cbegin(), tok.begin).c_str(),
                std::string(tok.begin, tok.end).c_str(),
                std::string(tok.end, data.cend()).c_str());
            const char* kind;
            switch (tok.kind) {
                case Tok::UNKNOWN: kind = "UNKNOWN"; break;
                case Tok::NUMERIC: kind = "NUMERIC"; break;
                case Tok::IDENT: kind = "IDENT"; break;
                case Tok::EQUALS: kind = "EQUALS"; break;
                case Tok::END: kind = "END"; break;
                default: kind = "????"; break;
            }
            printf(" %s ('%s')\n", kind, tok.text().c_str());
            if (tok.kind==Tok::END) {
                break;
            }
        }
    }
}
#endif // LEXTEST

