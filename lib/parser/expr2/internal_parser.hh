/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_INA_PARSER_EXPR_INTERNAL_PARSER_HH_INCLUDED
# define YY_INA_PARSER_EXPR_INTERNAL_PARSER_HH_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ina_parser_expr_debug;
#endif
/* "%code requires" blocks.  */
/* Line 2058 of yacc.c  */
#line 15 "parser.y"

#include "astnode.hh"
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif



/* Line 2058 of yacc.c  */
#line 56 "internal_parser.hh"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     _INA_PARSER_EXPR_TOKEN_PLUS = 258,
     _INA_PARSER_EXPR_TOKEN_MINUS = 259,
     _INA_PARSER_EXPR_TOKEN_MULTIPLY = 260,
     _INA_PARSER_EXPR_TOKEN_DIVIDE = 261,
     _INA_PARSER_EXPR_TOKEN_POWER = 262,
     _INA_PARSER_EXPR_TOKEN_LPAREN = 263,
     _INA_PARSER_EXPR_TOKEN_RPAREN = 264,
     _INA_PARSER_EXPR_TOKEN_INTEGER = 265,
     _INA_PARSER_EXPR_TOKEN_FLOAT = 266,
     _INA_PARSER_EXPR_TOKEN_IDENTIFIER = 267
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int ina_parser_expr_parse (void *YYPARSE_PARAM);
#else
int ina_parser_expr_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int ina_parser_expr_parse (iNA::Parser::Expr2::AstNode *expression, yyscan_t scanner);
#else
int ina_parser_expr_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_INA_PARSER_EXPR_INTERNAL_PARSER_HH_INCLUDED  */
