%code requires {
    #include <memory>
    class Node;
}

%{
#include "lexer.hpp"

#include <kiraz/ast/Operator.h>
#include <kiraz/ast/Literal.h>
#include <kiraz/ast/Module.h>
#include <kiraz/ast/Statement.h>

#include <kiraz/token/Literal.h>

int yyerror(const char *msg);
extern std::shared_ptr<Token> curtoken;
extern int yylineno;

#define YYSTYPE std::shared_ptr<Node>
#define YYSTYPE_IS_DECLARED 1
%}

%token    REJECTED

%token    KW_LET
%token    OP_PLUS OP_MINUS OP_MULT OP_DIVF
%token    OP_SCOLON OP_COLON OP_ASSIGN
%token    L_INTEGER
%token    IDENTIFIER
%token    OP_LPAREN OP_RPAREN

%left OP_PLUS OP_MINUS
%left OP_MULT OP_DIVF
%right UMINUS

%%

module: stmt { 
        std::vector<Node::Ptr> stmts = {$1};
        Node::add<ast::Module>(stmts);
    }
    ;

stmt: expr OP_SCOLON { $$ = $1; }
    | let_stmt { $$ = $1; }
    ;

identifier: IDENTIFIER {
        auto tok = std::dynamic_pointer_cast<token::Identifier>(curtoken);
        $$ = Node::add<ast::Id>(tok->get_text());
    }
    ;

let_stmt: KW_LET identifier OP_ASSIGN expr OP_SCOLON {
        $$ = Node::add<ast::Let>($2, nullptr, $4);
    }
    | KW_LET identifier OP_COLON identifier OP_SCOLON {
        $$ = Node::add<ast::Let>($2, $4, nullptr);
    }
    | KW_LET identifier OP_COLON identifier OP_ASSIGN expr OP_SCOLON {
        $$ = Node::add<ast::Let>($2, $4, $6);
    }
    ;

expr: L_INTEGER { 
        auto tok = std::dynamic_pointer_cast<token::Integer>(curtoken);
        $$ = Node::add<ast::Integer>(tok->get_value()); 
    }
    | identifier { $$ = $1; }
    | OP_LPAREN expr OP_RPAREN { $$ = $2;}
    | OP_MINUS expr %prec UMINUS { $$ = Node::add<ast::SignedNode>($2); }
    | expr OP_PLUS expr { $$ = Node::add<ast::Add>($1, $3); }
    | expr OP_MINUS expr { $$ = Node::add<ast::Sub>($1, $3); }
    | expr OP_MULT expr { $$ = Node::add<ast::Mult>($1, $3); }
    | expr OP_DIVF expr { $$ = Node::add<ast::DivF>($1, $3); }
    ;

%%

int yyerror(const char *s) {
    if (curtoken) {
        fmt::print("** Parser Error at {}:{} at token: {}\n",
            yylineno, Token::colno, curtoken->as_string());
    }
    else {
        fmt::print("** Parser Error at {}:{}, null token\n",
            yylineno, Token::colno);
    }

    Token::colno = 0;
    Node::reset_root();

    return 1;
}
