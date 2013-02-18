%{
 
/*
 * parser.y file
 * To generate the parser run: "bison Parser.y"
 */
 
#include "lexer.hh"
 
int ina_parser_expr_error(yyscan_t scanner, SExpression **expression, const char *msg);
 
%}
 
%code requires {
 
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif
 
}
 
//%require "2.6"
%output  "expr_parser.cc"
%defines "expr_parser.hh"
 
%skeleton "lalr1.cc"
%define namespace "iNA::Parser::Expr"

%lex-param   { yyscan_t scanner }
%parse-param { SExpression **expression }
%parse-param { yyscan_t scanner }
 
%union {
  int value;
  SExpression *expression;
}
 
%left '+' TOKEN_PLUS
%left '*' TOKEN_MULTIPLY
 
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_PLUS
%token TOKEN_MULTIPLY
%token <value> TOKEN_NUMBER
 
%type <expression> expr
 
%%
 
input
    : expr { *expression = $1; }
    ;
 
expr
    : expr TOKEN_PLUS expr { $$ = createOperation( ePLUS, $1, $3 ); }
    | expr TOKEN_MULTIPLY expr { $$ = createOperation( eMULTIPLY, $1, $3 ); }
    | TOKEN_LPAREN expr TOKEN_RPAREN { $$ = $2; }
    | TOKEN_NUMBER { $$ = createNumber($1); }
    ;
 
%%
