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

}

#endif