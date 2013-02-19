%{
 
/*
 * parser.y file
 * To generate the parser run: "bison Parser.y"
 */
 
#include "astnode.hh"
#define YYSTYPE iNA::Parser::Expr2::AstNode*

extern int ina_parser_expr_lex(YYSTYPE *lvalp,
                               ina_parser_expr_scan_t yyscanner);
%}
 
%code requires {
#include "astnode.hh"
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

}
 
%output  "internal_parser.cc"
%defines "internal_parser.hh"
%name-prefix="ina_parser_expr_"

%define api.pure
%lex-param   { yyscan_t scanner }
%parse-param { iNA::Parser::Expr2::AstNode *expression }
%parse-param { yyscan_t scanner }
  
%left _INA_PARSER_EXPR_TOKEN_PLUS
%left _INA_PARSER_EXPR_TOKEN_MINUS
%left _INA_PARSER_EXPR_TOKEN_MULTIPLY
%left _INA_PARSER_EXPR_TOKEN_DIVIDE
%right _INA_PARSER_EXPR_TOKEN_POWER

%token _INA_PARSER_EXPR_TOKEN_LPAREN
%token _INA_PARSER_EXPR_TOKEN_RPAREN
%token _INA_PARSER_EXPR_TOKEN_PLUS
%token _INA_PARSER_EXPR_TOKEN_MINUS
%token _INA_PARSER_EXPR_TOKEN_MULTIPLY
%token _INA_PARSER_EXPR_TOKEN_DIVIDE
%token _INA_PARSER_EXPR_TOKEN_POWER
%type <expression> expr
%token <expression> _INA_PARSER_EXPR_TOKEN_INTEGER
%token <expression> _INA_PARSER_EXPR_TOKEN_FLOAT
%token <expression> _INA_PARSER_EXPR_TOKEN_IDENTIFIER
 
%%
 
input
    : expr { expression = $1; }
    ;
 
expr
    : expr _INA_PARSER_EXPR_TOKEN_PLUS expr { $$ = iNA::Parser::Expr2::AstNode::newAdd($1, $3); }
    | expr _INA_PARSER_EXPR_TOKEN_MINUS expr { $$ = iNA::Parser::Expr2::AstNode::createSub($1, $3);}
    | expr _INA_PARSER_EXPR_TOKEN_MULTIPLY expr { $$ = iNA::Parser::Expr2::AstNode::createMul($1, $3); }
    | expr _INA_PARSER_EXPR_TOKEN_DIVIDE expr { $$ = iNA::Parser::Expr2::AstNode::createDiv($1, $3); }
    | expr _INA_PARSER_EXPR_TOKEN_POWER expr { $$ = iNA::Parser::Expr2::AstNode::createPow($1, $3); }
    | _INA_PARSER_EXPR_TOKEN_LPAREN expr _INA_PARSER_EXPR_TOKEN_RPAREN { $$ = $2; }
    | _INA_PARSER_EXPR_TOKEN_INTEGER { $$ = $1; }
    | _INA_PARSER_EXPR_TOKEN_FLOAT { $$ = $1; }
    ;
 
%%
