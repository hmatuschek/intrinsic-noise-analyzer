#ifndef __FLUC_SBMLSH_LEXER_HH__
#define __FLUC_SBMLSH_LEXER_HH__

#include "utils/lexer.hh"
#include "utils/tokenrules.hh"
#include <parser/expr/lexer.hh>


namespace Fluc {
namespace Parser {
namespace Sbmlsh {


/**
 * Defines all token types used by the SBML-SH parser
 *
 * @todo Implement ignored "comment" token for SBML-SH lexer.
 *
 * @ingroup sbmlsh
 */
typedef enum {
  T_WHITESPACE = Expr::T_WHITESPACE,  ///< A white space char (SPACE, TAB)
  T_IDENTIFIER = Expr::T_IDENTIFIER,  ///< C-style identifier.
  T_INTEGER = Expr::T_INTEGER,        ///< Integers (w/o) sign.
  T_FLOAT = Expr::T_FLOAT,            ///< Floats (w/o) sign.
  T_COMMA = Expr::T_COMMA,            ///< ','
  T_PLUS = Expr::T_PLUS,              ///< '+'
  T_MINUS = Expr::T_MINUS,            ///< '-'
  T_TIMES = Expr::T_TIMES,            ///< '*'
  T_POWER = Expr::T_POWER,            ///< '**', '^'
  T_DIVIVE = Expr::T_DIVIVE,          ///< '/'
  T_LPAR = Expr::T_LPAR,              ///< '('
  T_RPAR = Expr::T_RPAR,              ///< ')'

  T_END_OF_LINE,  ///< Any combination of NEWLINE, LINEFEET

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

  T_VERSION_NUMBER    ///< 'INT.INT.INT'
} TokenId;



/**
 * A helper token-rule to parse version strings "?.?.?", where "?" is a place holder for an positive
 * integer.
 *
 * @ingroup sbmlsh
 */
class VersionNumberTokenRule : public Utils::TokenRule
{
public:
  /** Constructor. */
  VersionNumberTokenRule();
};


/**
 * A lexer for the SBML-SH grammar.
 *
 * @ingroup sbmlsh
 */
class Lexer: public Utils::Lexer
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
