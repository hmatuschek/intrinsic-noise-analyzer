#ifndef __FLUC_SBMLSH_LEXER_HH__
#define __FLUC_SBMLSH_LEXER_HH__

#include "../expr/lexer.hh"


namespace iNA {
namespace Parser {
namespace Sbmlsh {


/**
 * Defines all token types used by the SBML-sh parser
 *
 * @todo Implement ignored "comment" token for SBML-sh lexer.
 *
 * @ingroup sbmlsh
 */
typedef enum {
  /* Reproduce lexer items of expression parser. */
  T_WHITESPACE = Expr::T_WHITESPACE,  ///< A white space char (SPACE, TAB)
  T_IDENTIFIER = Expr::T_IDENTIFIER,  ///< C-style identifier.
  T_INTEGER = Expr::T_INTEGER,        ///< Integers (w/o) sign.
  T_FLOAT = Expr::T_FLOAT,            ///< Floats (w/o) sign.
  T_COMMA = Expr::T_COMMA,            ///< ','
  T_PLUS = Expr::T_PLUS,              ///< '+'
  T_MINUS = Expr::T_MINUS,            ///< '-'
  T_TIMES = Expr::T_TIMES,            ///< '*'
  T_POWER = Expr::T_POWER,            ///< '**', '^'
  T_DIVIVE = Expr::T_DIVIDE,          ///< '/'
  T_LPAR = Expr::T_LPAR,              ///< '('
  T_RPAR = Expr::T_RPAR,              ///< ')'

  /* New token ids for the SBML-sh parser. */
  T_END_OF_LINE = Expr::T_FIRST_NON_EXPRESSION_TOKEN,  ///< Any combination of NEWLINE, LINEFEET

  T_MODEL_KW,         ///< '@model'
  T_UNITS_KW,         ///< '@units'
  T_COMPARTMENTS_KW,  ///< '@compartments'
  T_SPECIES_KW,       ///< '@Species'
  T_PARAMETERS_KW,    ///< '@parameters'
  T_RULES_KW,         ///< '@rules'
  T_ASSIGN_KW,        ///< '@assign'
  T_RATE_KW,          ///< '@rate'
  T_REACTIONS_KW,     ///< '@reactions'
  T_R_KW,             ///< '@r'
  T_RR_KW,            ///< '@rr'
  T_EVENTS_KW,        ///< '@events'

  T_QUOTED_STRING,    ///< Quoted string, w/o escapeing.

  T_COLON,            ///< ':'
  T_SEMICOLON,        ///< ';'

  T_ASSIGN,           ///< '='
  T_EQUAL,            ///< '=='
  T_NEQUAL,           ///< '!='
  T_LESSTHAN,         ///< '<'
  T_LESSEQUAL,        ///< '<='
  T_GREATERTHAN,      ///< '>'
  T_GREATEREQUAL,     ///< '>='

  T_RARROW,           ///< '->'

  T_LBRAC,            ///< '['
  T_RBRAC,            ///< ']'

  T_VERSION_NUMBER,   ///< 'INT.INT.INT'
  T_COMMENT           ///< Line comment "#...";
} TokenId;



/**
 * A helper token-rule to parse version strings "?.?.?", where "?" is a place holder for an positive
 * integer.
 *
 * @ingroup sbmlsh
 */
class VersionNumberTokenRule : public iNA::Parser::TokenRule
{
public:
  /** Constructor. */
  VersionNumberTokenRule();
};


/** Helper token rule to parse comments. */
class LineCommentTokenRule : public iNA::Parser::TokenRule
{
public:
  /** Constructor. */
  LineCommentTokenRule();
};


/**
 * A lexer for the SBML-sh grammar.
 *
 * @todo Turn this into a Production to get the actual version numbers.
 * @ingroup sbmlsh
 */
class Lexer: public iNA::Parser::Lexer
{
public:
  /** Constructor.
   * @param input Specifies the input stream to lex. */
  Lexer(std::istream &input);
};


}
}
}

#endif // LEXER_HH
