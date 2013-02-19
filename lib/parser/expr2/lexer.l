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
%option bison-bridge
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
 
{WS} {
  /* Skip blanks. */
}

{NUMBER} {
  long value; sscanf(yytext, "%d", &value);
  yylvalue->expression = iNA::Parser::Expr2::AstNode::newInteger(value);
  return TOKEN_INTEGER;
}

{NUMBER}"."{NUMBER}? {
  double value; sscanf(yytext, "%lf", &value);
  yylvalue->expression = iNA::Parser::Expr2::AstNode::newFloat(value);
  return TOKEN_FLOAT;
}

{NUMBER}("."{NUMBER}?)?([eE]([\-])?{NUMBER}) {
    double value; sscanf(yytext, "%lf", &value;);
    yylvalue->expression = iNA::Parser::Expr2::AstNode::newFloat(value);
    return TOKEN_FLOAT;
}

{IDENTIFIER} {
  yylvalue->expression = iNA::Parser::Expr2::AstNode::newSymbol(yytext);
  return TOKEN_IDENTIFIER;
}

{POWER} {
  return TOKEN_POWER;
}

{MULTIPLY} {
  return TOKEN_MULTIPLY;
}

{DIVIDE} {
  return TOKEN_DIVIDE;
}

{PLUS} {
  return TOKEN_PLUS;
}

{MINUS} {
  return TOKEN_MINUS;
}

{LPAREN} {
  return TOKEN_LPAREN;
}

{RPAREN} {
  return TOKEN_RPAREN;
}

. {
  ina_parser_expr_error("Unexpected char.");
}
 


%%
 
/** Signals an lexer error. */
int ina_parser_expr_error(const char *msg) {
    /** @todo Thow lexer exception here! */
    fprintf(stderr,"Error:%s\n",msg); return 0;
}

