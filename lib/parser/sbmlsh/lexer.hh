#ifndef __FLUC_SBMLSH_LEXER_HH__
#define __FLUC_SBMLSH_LEXER_HH__

#include "utils/lexer.hh"
#include "utils/tokenrules.hh"


namespace Fluc {
namespace Parser {
namespace Sbmlsh {


/**
 * Defines all token types used by the SBML-SH parser
 *
 * @ingroup sbmlsh
 */
typedef enum {
  T_WHITESPACE = Utils::Token::FIRST_USER_DEFINED,  ///< A white space char (SPACE, TAB)
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

  T_IDENTIFIER,       ///< C-style identifier.

  T_QUOTED_STRING,    ///< Quoted string, w/o escapeing.

  T_INTEGER,          ///< Integers (w/o) sign.
  T_FLOAT,            ///< Floats (w/o) sign.

  T_COLON,            ///< ':'
  T_SEMICOLON,        ///< ';'
  T_COMMA,            ///< ','

  T_ASSIGN,           ///< '='
  T_EQUAL,            ///< '=='
  T_NEQUAL,           ///< '!='
  T_LESSTHAN,         ///< '<'
  T_LESSEQUAL,        ///< '<='
  T_GREATERTHAN,      ///< '>'
  T_GREATEREQUAL,     ///< '>='

  T_PLUS,             ///< '+'
  T_MINUS,            ///< '-'
  T_TIMES,            ///< '*'
  T_DIVIVE,           ///< '/'
  T_RARROW,           ///< '->'

  T_LPAR,             ///< '('
  T_RPAR,             ///< ')'
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
