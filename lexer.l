%option noyywrap
%option yylineno

%{
#include "main.h"
#include "parser.hpp"
#include <kiraz/Token.h>
#include <kiraz/token/Literal.h>
#include <kiraz/token/Operator.h>
#include <kiraz/token/Keyword.h>
// https://stackoverflow.com/questions/9611682/flexlexer-support-for-unicode/9617585#9617585

using namespace token;
static auto &colno = Token::colno;
extern std::shared_ptr<Token> curtoken;
%}

/* -------------------- LEXER KURALLARI -------------------- */

%%

"let"      { colno += yyleng; curtoken = Token::New<Let>();         return KW_LET; }
"func"     { colno += yyleng; curtoken = Token::New<Func>();        return KW_FUNC; }

"+"        { colno += yyleng; curtoken = Token::New<OpPlus>();       return OP_PLUS; }
"-"        { colno += yyleng; curtoken = Token::New<OpMinus>();      return OP_MINUS; }
"*"        { colno += yyleng; curtoken = Token::New<OpMult>();       return OP_MULT; }
"/"        { colno += yyleng; curtoken = Token::New<OpDivF>();       return OP_DIVF; }
";"        { colno += yyleng; curtoken = Token::New<OpScolon>();     return OP_SCOLON; }
":"        { colno += yyleng; curtoken = Token::New<OpColon>();      return OP_COLON; }
"="        { colno += yyleng; curtoken = Token::New<OpAssign>();     return OP_ASSIGN; }
","        { colno += yyleng; curtoken = Token::New<OpComma>();      return OP_COMMA; }

"("        { colno += yyleng; curtoken = Token::New<OpLparen>();     return OP_LPAREN; }
")"        { colno += yyleng; curtoken = Token::New<OpRparen>();     return OP_RPAREN; }
"{"        { colno += yyleng; curtoken = Token::New<OpLbrace>();     return OP_LBRACE; }
"}"        { colno += yyleng; curtoken = Token::New<OpRbrace>();     return OP_RBRACE; }

"=="       { colno += yyleng; curtoken = Token::New<OpEqual>();      return OP_EQUAL; }
">="       { colno += yyleng; curtoken = Token::New<OpGreaterEq>();  return OP_GREATEREQ; }
"<="       { colno += yyleng; curtoken = Token::New<OpLessEq>();     return OP_LESSEQ; }
">"        { colno += yyleng; curtoken = Token::New<OpGreater>();    return OP_GREATER; }
"<"        { colno += yyleng; curtoken = Token::New<OpLess>();       return OP_LESS; }

[a-zA-Z_][a-zA-Z0-9_]*  { colno += yyleng; curtoken = Token::New<Identifier>(yytext);    return IDENTIFIER; }
[0-9]+                  { colno += yyleng; curtoken = Token::New<Integer>(10, yytext);   return L_INTEGER; }
\"([^\"\\]|\\.)*\"      { colno += yyleng; curtoken = Token::New<String>(yytext);        return L_STRING; }


[ \t\r\n]+              { colno += yyleng; }
.                       { colno += yyleng; curtoken = Token::New<Rejected>(yytext); return YYUNDEF; }
%%