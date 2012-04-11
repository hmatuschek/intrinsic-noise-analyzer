#ifndef __FLUC_SBMLSH_LEXER_HH__
#define __FLUC_SBMLSH_LEXER_HH__

#include "utils/lexer.hh"
#include "utils/tokenrules.hh"


namespace Fluc {
namespace Sbmlsh {


/**
 * Defines all token types used by the SBML-SH parser
 */
typedef enum {
  T_WHITESPACE = Utils::Token::FIRST_USER_DEFINED,
  T_END_OF_LINE,

  T_MODEL_KW,
  T_UNITS_KW,
  T_COMPARTMENTS_KW,
  T_SPECIES_KW,
  T_PARAMETERS_KW,
  T_RULES_KW,
  T_RATE_KW,
  T_REACTIONS_KW,
  T_R_KW,
  T_RR_KW,
  T_EVENTS_KW,

  T_IDENTIFIER,

  T_QUOTED_STRING,

  T_INTEGER,
  T_FLOAT,

  T_COLON,
  T_SEMICOLON,
  T_COMMA,

  T_ASSIGN,
  T_EQUAL,
  T_NEQUAL,
  T_LESSTHAN,
  T_LESSEQUAL,
  T_GREATERTHAN,
  T_GREATEREQUAL,

  T_PLUS,
  T_MINUS,
  T_TIMES,
  T_DIVIVE,
  T_RARROW,

  T_LPAR,
  T_RPAR,
  T_LBRAC,
  T_RBRAC,

  T_VERSION_NUMBER
} TokenId;



/**
 * A helper token-rule to parse version strings "?.?.?".
 */
class VersionNumberTokenRule : public Utils::TokenRule
{
public:
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
  Lexer(std::istream &input);
};


}
}

#endif // LEXER_HH
