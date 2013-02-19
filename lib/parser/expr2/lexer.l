%{
 
/*
 * lexer.l file
 *  Defines the lexer for the expression parser.
 *
 * Assembled from the templates available at:
 *   https://github.com/bingmann/flex-bison-cpp-example/
 */
 
#include <stdio.h>
#include <math.h>
#include "astnode.hh"
#include "expr_parser.hh"
%}
 
%option outfile="expr_lexer.cc" header-file="expr_lexer.hh"
%option warn nodefault

%option noyywrap 
%option never-interactive 
%option bisonbridge
%option nounistd
%option prefix="ina_parser_expr_"


LPAREN      "("
RPAREN      ")"
PLUS        "+"
MINUS       "-"
POWER       "**"
MULTIPLY    "*"
DIVIDE      "/"

IDENTIFIER  [a-zA-Z_][a-zA-Z0-9_]*
NUMBER      [0-9]+
WS          [ \r\n\t]*
 
%%
 
{WS}            { /* Skip blanks. */ }
{NUMBER}        { sscanf(yytext, "%d", &yylval->integer_value); return TOKEN_INTEGER; }
{NUMBER}"."{NUMBER}? { sscanf(yytext, "%lf", &yylval->double_value); return TOKEN_FLOAT; }
{NUMBER}("."{NUMBER}?)?([eE]([\-])?{NUMBER}) {
                  sscanf(yytext, "%lf", &yylval->double_value;); return TOKEN_FLOAT; }
{IDENTIFIER}    { yylval->string = yytext; return TOKEN_IDENTIFIER; }
{POWER}         { return TOKEN_POWER; }
{MULTIPLY}      { return TOKEN_MULTIPLY; }
{DIVIDE}        { return TOKEN_DIVIDE; }
{PLUS}          { return TOKEN_PLUS; }
{MINUS}         { return TOKEN_MINUS; }
{LPAREN}        { return TOKEN_LPAREN; }
{RPAREN}        { return TOKEN_RPAREN; }

 
%%
 
/** Signals an lexer error. */
int ina_parser_expr_error(const char *msg) {
    /** @todo Thow lexer exception here! */
    fprintf(stderr,"Error:%s\n",msg); return 0;
}

