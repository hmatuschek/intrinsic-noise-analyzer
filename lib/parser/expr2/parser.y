%{
 
/*
 * parser.y file
 * To generate the parser run: "bison Parser.y"
 */
 
#include "astnode.hh"
#include "lexer.hh"
 
int ina_parser_expr_error(yyscan_t scanner, SExpression **expression, const char *msg);
 
%}
 
%code requires {
#include "../expr/ir.hh"
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
 
}
 
%output  "expr_parser.cc"
%defines "expr_parser.hh"
 
%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { iNA::Parser::Expr2::AstNode *expression }
%parse-param { yyscan_t scanner }
 
%union {
    iNA::Parser::Expr2::AstNode *expression;
}
 
%left '+' TOKEN_PLUS
%left '-' TOKEN_MINUS
%left '*' TOKEN_MULTIPLY
%left '/' TOKEN_DIVIDE
%right "**" TOKEN_POWER

%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_PLUS
%token TOKEN_MINUS
%token TOKEN_MULTIPLY
%token TOKEN_DIVIDE
%token TOKEN_POWER
%token <integer_value> TOKEN_INTEGER
%token <double_value>  TOKEN_FLOAT
%token <string> TOKEN_IDENTIFIER

%type <expression> expr
 
%%
 
input
    : expr { expression = $1; }
    ;
 
expr
    : expr TOKEN_PLUS expr { $$ = iNA::Parser::Expr::Node::createAdd($1, $3); }
    | expr TOKEN_MINUS expr { $$ = iNA::Parser::Expr::Node::createSub($1, $3);}
    | expr TOKEN_MULTIPLY expr { $$ = iNA::Parser::Expr::Node::createMul($1, $3); }
    | TOKEN_LPAREN expr TOKEN_RPAREN { $$ = $2; }
    | TOKEN_INTEGER { $$ = iNA::Parser::expr::Node::createValue($1); }
    | TOKEN_FLOAT { $$ = iNA::Parser::expr::Node::createValue($1); }
    ;
 
%%
