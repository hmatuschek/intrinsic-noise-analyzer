#ifndef __FLUC_UTILS_TOKENRULES_HH__
#define __FLUC_UTILS_TOKENRULES_HH__

#include "lexer.hh"

namespace Fluc {
namespace Utils {


/**
 * A token-rule, that parses white-spaces SPACE and TAB.
 * @ingroup parser
 */
class WhiteSpaceTokenRule : public TokenRule
{
public:
  /**
   * Constructor, takes the token-id of white-spaces.
   */
  WhiteSpaceTokenRule(unsigned id);
};


/**
 * A token-rule that parses EOLs ("NL", "CR", "NL CR", "CR NL").
 * @ingroup parser
 */
class EOLTokenRule : public TokenRule
{
public:
  /**
   * Constructor, takes the token-id for EOLs.
   */
  EOLTokenRule(unsigned id);
};


/**
 * A token rule to parse C-style identifier.
 * @ingroup parser
 */
class IdentifierTokenRule : public TokenRule
{
public:
  /**
   * Constructor, takes the token-id for identifier.
   */
  IdentifierTokenRule(unsigned id);
};


/**
 * A token rule to parse quoted strings (without escapes).
 * @ingroup parser
 */
class StringTokenRule : public TokenRule
{
public:
  /**
   * Constructor, takes the token-id for strings.
   */
  StringTokenRule(unsigned id);
};


/**
 * Simplifies token rule to parse keywords.
 * @ingroup parser
 */
class KeyWordTokenRule : public TokenRule
{
public:
  /**
   * Constructor, takes the token-id for the given keyword.
   */
  KeyWordTokenRule(unsigned id, const std::string &keyword);
};


/**
 * A token rule to parse non-negative integers.
 * @ingroup parser
 */
class IntegerTokenRule : public TokenRule
{
public:
  /**
   * Constructor, takes the token-id for integer-numbers.
   */
  IntegerTokenRule(unsigned id);
};


/**
 * A token rule to parse non-negative floating-point numbers (incl. exp.).
 *
 * \dot
 * digraph DFA {
 *   rankdir=LR;
 *   graph [fontsize=8];
 *
 *   si  [shape=box, label="init"];
 *   s3  [shape=circle, label="state 1"];
 *   sff [shape=doublecircle, label="final 1"];
 *   sei [shape=circle, label="state 2"];
 *   se1 [shape=circle, label="state 3"];
 *   sef [shape=doublecircle, label="final 2"];
 *
 *   si  -> s3  [label="0-9"];
 *   s3  -> s3  [label="0-9"];
 *   s3  -> sff [label="."];
 *   s3  -> sei [label="e"];
 *   sff -> sff [label="0-9"];
 *   sff -> sei [label="e"];
 *   sei -> se1 [label="-"];
 *   sei -> sef [label="0-9"];
 *   se1 -> sef [label="0-9"];
 *   sef -> sef [label="0-9"];
 * }
 * \enddot
 *
 * @ingroup parser
 */
class FloatTokenRule : public TokenRule
{
public:
  /**
   * Constructor, takes the token-id for floating-point numbers.
   */
  FloatTokenRule(unsigned id);
};


}
}
#endif // TOKENRULES_HH
