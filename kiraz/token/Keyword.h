#ifndef KIRAZ_TOKEN_KEYWORD_H
#define KIRAZ_TOKEN_KEYWORD_H

#include <kiraz/Token.h>
#include "parser.hpp"

namespace token {

class Let : public Token {
public:
    Let() : Token(KW_LET) {}
    std::string as_string() const override { return "KW_LET"; }
};

class Func : public Token {
public:
    Func() : Token(KW_FUNC) {}
    std::string as_string() const override { return "KW_FUNC"; }
};

class If : public Token {
public:
    If() : Token(KW_IF) {}
    std::string as_string() const override { return "KW_IF"; }
};

class Else : public Token {
public:
    Else() : Token(KW_ELSE) {}
    std::string as_string() const override { return "KW_ELSE"; }
};

class While : public Token {
public:
    While() : Token(KW_WHILE) {}
    std::string as_string() const override { return "KW_WHILE"; }
};

}

#endif