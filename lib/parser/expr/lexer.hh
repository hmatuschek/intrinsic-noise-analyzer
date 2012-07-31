#ifndef __FLUC_PARSER_EXPR_LEXER_HH__
#define __FLUC_PARSER_EXPR_LEXER_HH__

#include <utils/lexer.hh>


namespace Fluc {
namespace Parser {
namespace Expr {

/**
 * Defines all token types used by the SBML-SH parser
 */
typedef enum {
  T_WHITESPACE = Utils::Token::FIRST_USER_DEFINED,  ///< A white space char (SPACE, TAB)
  T_IDENTIFIER,       ///< C-style identifier.
  T_INTEGER,          ///< Integers (w/o) sign.
  T_FLOAT,            ///< Floats (w/o) sign.
  T_COMMA,            ///< ','
  T_PLUS,             ///< '+'
  T_MINUS,            ///< '-'
  T_TIMES,            ///< '*'
  T_DIVIVE,           ///< '/'
  T_LPAR,             ///< '('
  T_RPAR             ///< ')'
} TokenId;


}
}
}

#endif // __FLUC_PARSER_EXPR_LEXER_HH__
