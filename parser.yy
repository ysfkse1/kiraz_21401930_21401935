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

%token    KW_LET KW_FUNC KW_IF KW_ELSE KW_WHILE KW_RETURN KW_IMPORT KW_CLASS
%token    OP_PLUS OP_MINUS OP_MULT OP_DIVF
%token    OP_SCOLON OP_COLON OP_ASSIGN OP_COMMA OP_DOT
%token    OP_EQUAL OP_GREATER OP_GREATEREQ OP_LESS OP_LESSEQ
%token    L_INTEGER L_STRING
%token    IDENTIFIER
%token    OP_LPAREN OP_RPAREN OP_LBRACE OP_RBRACE

%right OP_ASSIGN
%left OP_EQUAL OP_GREATER OP_GREATEREQ OP_LESS OP_LESSEQ
%left OP_PLUS OP_MINUS
%left OP_MULT OP_DIVF
%left OP_DOT
%right UMINUS

%%

module: stmt_list_top {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($1);
        std::vector<Node::Ptr> stmts = list->get_items();
        Node::add<ast::Module>(stmts);
    }
    ;

stmt_list_top: stmt {
        std::vector<Node::Ptr> stmts = {$1};
        $$ = Node::add<ast::ListNode>(stmts);
    }
    | stmt_list_top stmt {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($1);
        std::vector<Node::Ptr> stmts = list->get_items();
        stmts.push_back($2);
        $$ = Node::add<ast::ListNode>(stmts);
    }
    ;

stmt: expr OP_SCOLON { $$ = $1; }
    | let_stmt { $$ = $1; }
    | expr OP_ASSIGN expr OP_SCOLON { $$ = Node::add<ast::Assign>($1, $3); }
    | func_stmt { $$ = $1; }
    | if_stmt { $$ = $1; }
    | while_stmt { $$ = $1; }
    | return_stmt { $$ = $1; }
    | import_stmt { $$ = $1; }
    | class_stmt { $$ = $1; }
    | OP_LBRACE stmt_list_multi OP_RBRACE OP_SCOLON {
        $$ = $2;
    }
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

return_stmt: KW_RETURN expr OP_SCOLON {
        $$ = Node::add<ast::Return>($2);
    }
    ;

import_stmt: KW_IMPORT identifier OP_SCOLON {
        $$ = Node::add<ast::Import>($2);
    }
    ;

class_stmt: KW_CLASS identifier OP_LBRACE OP_RBRACE OP_SCOLON {
        std::vector<Node::Ptr> empty_stmts;
        $$ = Node::add<ast::Class>($2, empty_stmts);
    }
    | KW_CLASS identifier OP_LBRACE stmt_list_multi OP_RBRACE OP_SCOLON {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($4);
        $$ = Node::add<ast::Class>($2, list->get_items());
    }
    | KW_CLASS identifier OP_COLON identifier OP_LBRACE OP_RBRACE OP_SCOLON {
        std::vector<Node::Ptr> empty_stmts;
        $$ = Node::add<ast::Class>($2, empty_stmts, $4);
    }
    | KW_CLASS identifier OP_COLON identifier OP_LBRACE stmt_list_multi OP_RBRACE OP_SCOLON {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($6);
        $$ = Node::add<ast::Class>($2, list->get_items(), $4);
    }
    ;

func_stmt: KW_FUNC identifier OP_LPAREN OP_RPAREN OP_COLON identifier OP_LBRACE OP_RBRACE OP_SCOLON {
        std::vector<Node::Ptr> empty_stmts;
        $$ = Node::add<ast::Func>($2, nullptr, $6, empty_stmts);
    }
    | KW_FUNC identifier OP_LPAREN OP_RPAREN OP_COLON identifier OP_LBRACE stmt_list_multi OP_RBRACE OP_SCOLON {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($8);
        $$ = Node::add<ast::Func>($2, nullptr, $6, list->get_items());
    }
    | KW_FUNC identifier OP_LPAREN func_args OP_RPAREN OP_COLON identifier OP_LBRACE OP_RBRACE OP_SCOLON {
        // Convert ListNode to FuncArgs
        auto list = std::dynamic_pointer_cast<ast::ListNode>($4);
        auto func_args_node = Node::add<ast::FuncArgs>(list->get_items());
        std::vector<Node::Ptr> empty_stmts;
        $$ = Node::add<ast::Func>($2, func_args_node, $7, empty_stmts);
    }
    | KW_FUNC identifier OP_LPAREN func_args OP_RPAREN OP_COLON identifier OP_LBRACE stmt_list_multi OP_RBRACE OP_SCOLON {
        // Convert ListNode to FuncArgs
        auto list = std::dynamic_pointer_cast<ast::ListNode>($4);
        auto func_args_node = Node::add<ast::FuncArgs>(list->get_items());
        auto stmt_list = std::dynamic_pointer_cast<ast::ListNode>($9);
        $$ = Node::add<ast::Func>($2, func_args_node, $7, stmt_list->get_items());
    }
    ;

func_args: func_arg {
        std::vector<Node::Ptr> args = {$1};
        $$ = Node::add<ast::ListNode>(args);
    }
    | func_args OP_COMMA func_arg {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($1);
        std::vector<Node::Ptr> args = list->get_items();
        args.push_back($3);
        $$ = Node::add<ast::ListNode>(args);
    }
    ;

func_arg: identifier OP_COLON identifier {
        $$ = Node::add<ast::FArg>($1, $3);
    }
    ;

call_args: expr {
        std::vector<Node::Ptr> args = {$1};
        $$ = Node::add<ast::ListNode>(args);
    }
    | call_args OP_COMMA expr {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($1);
        std::vector<Node::Ptr> args = list->get_items();
        args.push_back($3);
        $$ = Node::add<ast::ListNode>(args);
    }
    ;

stmt_list: stmt {
        // Single statement - we'll handle as vector in func_stmt
        $$ = $1;
    }
    ;

if_stmt: KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE OP_RBRACE OP_SCOLON {
        std::vector<Node::Ptr> empty_then;
        $$ = Node::add<ast::If>($3, empty_then, nullptr);
    }
    | KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE stmt_list_multi OP_RBRACE OP_SCOLON {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($6);
        $$ = Node::add<ast::If>($3, list->get_items(), nullptr);
    }
    | KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE OP_RBRACE KW_ELSE OP_LBRACE OP_RBRACE OP_SCOLON {
        std::vector<Node::Ptr> empty_then;
        std::vector<Node::Ptr> empty_else;
        auto else_block = Node::add<ast::ElseBlock>(empty_else);
        $$ = Node::add<ast::If>($3, empty_then, else_block);
    }
    | KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE stmt_list_multi OP_RBRACE KW_ELSE OP_LBRACE OP_RBRACE OP_SCOLON {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($6);
        std::vector<Node::Ptr> empty_else;
        auto else_block = Node::add<ast::ElseBlock>(empty_else);
        $$ = Node::add<ast::If>($3, list->get_items(), else_block);
    }
    | KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE OP_RBRACE KW_ELSE OP_LBRACE stmt_list_multi OP_RBRACE OP_SCOLON {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($9);
        std::vector<Node::Ptr> empty_then;
        auto else_block = Node::add<ast::ElseBlock>(list->get_items());
        $$ = Node::add<ast::If>($3, empty_then, else_block);
    }
    | KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE stmt_list_multi OP_RBRACE KW_ELSE OP_LBRACE stmt_list_multi OP_RBRACE OP_SCOLON {
        auto then_list = std::dynamic_pointer_cast<ast::ListNode>($6);
        auto else_list = std::dynamic_pointer_cast<ast::ListNode>($10);
        auto else_block = Node::add<ast::ElseBlock>(else_list->get_items());
        $$ = Node::add<ast::If>($3, then_list->get_items(), else_block);
    }
    | KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE OP_RBRACE KW_ELSE if_stmt {
        std::vector<Node::Ptr> empty_then;
        $$ = Node::add<ast::If>($3, empty_then, $8);
    }
    | KW_IF OP_LPAREN expr OP_RPAREN OP_LBRACE stmt_list_multi OP_RBRACE KW_ELSE if_stmt {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($6);
        $$ = Node::add<ast::If>($3, list->get_items(), $9);
    }
    ;

stmt_list_multi: stmt {
        std::vector<Node::Ptr> stmts = {$1};
        $$ = Node::add<ast::ListNode>(stmts);
    }
    | stmt_list_multi stmt {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($1);
        std::vector<Node::Ptr> stmts = list->get_items();
        stmts.push_back($2);
        $$ = Node::add<ast::ListNode>(stmts);
    }
    ;

while_stmt: KW_WHILE OP_LPAREN expr OP_RPAREN OP_LBRACE OP_RBRACE OP_SCOLON {
        std::vector<Node::Ptr> empty_stmts;
        $$ = Node::add<ast::While>($3, empty_stmts);
    }
    | KW_WHILE OP_LPAREN expr OP_RPAREN OP_LBRACE stmt_list_multi OP_RBRACE OP_SCOLON {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($6);
        $$ = Node::add<ast::While>($3, list->get_items());
    }
    ;

expr: L_INTEGER { 
        auto tok = std::dynamic_pointer_cast<token::Integer>(curtoken);
        $$ = Node::add<ast::Integer>(tok->get_value()); 
    }
    | L_STRING {
        auto tok = std::dynamic_pointer_cast<token::String>(curtoken);
        $$ = Node::add<ast::Str>(tok->get_value());
    }
    | identifier { $$ = $1; }
    | OP_LPAREN expr OP_RPAREN { $$ = $2;}
    | OP_MINUS expr %prec UMINUS { $$ = Node::add<ast::SignedNode>($2); }
    | expr OP_PLUS expr { $$ = Node::add<ast::Add>($1, $3); }
    | expr OP_MINUS expr { $$ = Node::add<ast::Sub>($1, $3); }
    | expr OP_MULT expr { $$ = Node::add<ast::Mult>($1, $3); }
    | expr OP_DIVF expr { $$ = Node::add<ast::DivF>($1, $3); }
    | expr OP_EQUAL expr { $$ = Node::add<ast::OpEq>($1, $3); }
    | expr OP_GREATER expr { $$ = Node::add<ast::OpGt>($1, $3); }
    | expr OP_GREATEREQ expr { $$ = Node::add<ast::OpGe>($1, $3); }
    | expr OP_LESS expr { $$ = Node::add<ast::OpLt>($1, $3); }
    | expr OP_LESSEQ expr { $$ = Node::add<ast::OpLe>($1, $3); }
    | expr OP_DOT identifier { $$ = Node::add<ast::Dot>($1, $3); }
    | expr OP_LPAREN OP_RPAREN {
        $$ = Node::add<ast::Call>($1, Node::add<ast::FuncArgs>(std::vector<Node::Ptr>{}));
    }
    | expr OP_LPAREN call_args OP_RPAREN {
        auto list = std::dynamic_pointer_cast<ast::ListNode>($3);
        $$ = Node::add<ast::Call>($1, Node::add<ast::FuncArgs>(list->get_items()));
    }
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