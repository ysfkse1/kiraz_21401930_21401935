#ifndef KIRAZ_TOKEN_LITERAL_H
#define KIRAZ_TOKEN_LITERAL_H

#include <kiraz/Token.h>
#include "parser.hpp"
#include <string>

namespace token {

class Identifier : public Token {
public:
    Identifier(const char* text) : Token(IDENTIFIER), text(text) {}
    std::string as_string() const override { return "Identifier(" + text + ")"; }
private:
    std::string text;
};

class Integer : public Token {
public:
    Integer(int base, const char* text) : Token(L_INTEGER), base(base), text(text) {}
    std::string as_string() const override { return "Integer(" + text + ")"; }
private:
    int base;
    std::string text;
};

}

#endif // KIRAZ_TOKEN_LITERAL_H
