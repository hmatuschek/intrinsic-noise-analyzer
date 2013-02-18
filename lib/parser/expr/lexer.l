%{
 
/*
 * lexer.l file
 * Defines the lexer for the expression parser. 
 */
 
#include <stdio.h>
#include <math.h>

%}
 
%option outfile="expr_lexer.cc" header-file="expr_lexer.hh"
%option warn nodefault
 
%option reentrant 
%option noyywrap 
%option never-interactive 
%option nounistd
%option bison-bridge
%option prefix="ina_parser_expr_"

 
LPAREN      "("
RPAREN      ")"
PLUS        "+"
MULTIPLY    "*"
 
NUMBER      [0-9]+
WS          [ \r\n\t]*
 
%%
 
{WS}            { /* Skip blanks. */ }
{NUMBER}        { sscanf(yytext, "%d", &yylval->value); return TOKEN_NUMBER; }
 
{MULTIPLY}      { return TOKEN_MULTIPLY; }
{PLUS}          { return TOKEN_PLUS; }
{LPAREN}        { return TOKEN_LPAREN; }
{RPAREN}        { return TOKEN_RPAREN; }
.               {  }
 
%%
 
/** Signals an lexer error. */
int ina_parser_expr_error(const char *msg) {
    /** @todo Thow lexer exception here! */
    fprintf(stderr,"Error:%s\n",msg); return 0;
}

