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
    std::string get_text() const { return text; }
private:
    std::string text;
};

class Integer : public Token {
public:
    Integer(int base, const char* text) : Token(L_INTEGER), base(base), text(text) {}
    std::string as_string() const override { return "Integer(" + text + ")"; }
    long long get_value() const { return std::stoll(text, nullptr, base); }
private:
    int base;
    std::string text;
};

class String : public Token {
public:
    String(const char* text) : Token(L_STRING), text(text) {}
    std::string as_string() const override { return "String(" + text + ")"; }
    std::string get_value() const { 
        
        std::string result = text.substr(1, text.length() - 2);
        size_t pos = 0;
        while ((pos = result.find("\\n", pos)) != std::string::npos) {
            result.replace(pos, 2, "\n");
            pos += 1;
        }
        return result;
    }
private:
    std::string text;
};

}
#endif // KIRAZ_TOKEN_LITERAL_H