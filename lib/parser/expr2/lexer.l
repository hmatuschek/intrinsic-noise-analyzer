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
#include "internal_parser.hh"
// Forwad declaration:
int ina_parser_expr_error(const char *msg);
#define YYSTYPE iNA::Parser::Expr2::AstNode*
%}
 
%option outfile="internal_lexer.cc" header-file="internal_lexer.hh"
%option reentrant
%option never-interactive 
%option bison-bridge
%option nounistd
%option nodefault
%option noinput
%option nounput
%option noyywrap
%option warn
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
  long value; sscanf(yytext, "%ld", &value);
  (*yylval_param) = iNA::Parser::Expr2::AstNode::newInteger(value);
  return _INA_PARSER_EXPR_TOKEN_INTEGER;
}

{NUMBER}"."{NUMBER}? {
  double value; sscanf(yytext, "%lf", &value);
  (*yylval_param) = iNA::Parser::Expr2::AstNode::newFloat(value);
  return _INA_PARSER_EXPR_TOKEN_FLOAT;
}

{NUMBER}("."{NUMBER}?)?([eE]([\-])?{NUMBER}) {
    double value; sscanf(yytext, "%lf", &value);
    (*yylval_param) = iNA::Parser::Expr2::AstNode::newFloat(value);
    return _INA_PARSER_EXPR_TOKEN_FLOAT;
}

{IDENTIFIER} {
  (*yylval_param) = iNA::Parser::Expr2::AstNode::newSymbol(yytext);
  return _INA_PARSER_EXPR_TOKEN_IDENTIFIER;
}

{POWER} {
  return _INA_PARSER_EXPR_TOKEN_POWER;
}

{MULTIPLY} {
  return _INA_PARSER_EXPR_TOKEN_MULTIPLY;
}

{DIVIDE} {
  return _INA_PARSER_EXPR_TOKEN_DIVIDE;
}

{PLUS} {
  return _INA_PARSER_EXPR_TOKEN_PLUS;
}

{MINUS} {
  return _INA_PARSER_EXPR_TOKEN_MINUS;
}

{LPAREN} {
  return _INA_PARSER_EXPR_TOKEN_LPAREN;
}

{RPAREN} {
  return _INA_PARSER_EXPR_TOKEN_RPAREN;
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

