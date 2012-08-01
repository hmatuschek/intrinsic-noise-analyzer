#ifndef __FLUC_PARSER_LEXER_HH__
#define __FLUC_PARSER_LEXER_HH__

#include "automata.hh"
#include <vector>
#include <iostream>
#include <sstream>


namespace Fluc {
namespace Parser {

/**
 * Represents a single (parsed) token.
 *
 * @ingroup parser
 */
class Token
{
public:
  /** Defines the default token-ids. */
  typedef enum {
    END_OF_INPUT = 0,          ///< A token representing the end-of-input.
    FIRST_USER_DEFINED = 256   ///< The first non-reseved token-id.
  } Id;


protected:
  /** Holds the ID of this token. */
  unsigned id;

  /** Holds the line of this token. */
  size_t line;

  /** Holds the value of this token. */
  std::string value;


public:
  /** Constructs an empty token (END_OF_INPUT). */
  Token();

  /** Constructs a token from given identifier, line and value. */
  Token(unsigned id, size_t line, const std::string &value);

  /** Returns the identifier of the token. */
  unsigned getId() const;

  /** Returns the line of the token. */
  unsigned int getLine() const;

  /** Returns the value of the token. */
  const std::string &getValue() const;
};



/**
 * Base class of all token-rules (regex).
 *
 * @ingroup parser
 */
class TokenRule : public DFA<char>
{
protected:
  /** Id of the token. */
  unsigned id;

  /** Holds the current string of this token, will be reset if reset() is called. */
  std::stringstream _current_value;

public:
  /** Constructs a token-rule. */
  TokenRule(unsigned id);

  /** Constructs a token from match. */
  Token getToken(size_t line) const;

  /** Returns the id of the tokens generated by this rule. */
  unsigned getId() const;

  /** Resets this token rule, also clears the allready parsed value. */
  void reset();

  /** Accepts a char from the input, if the char is accepted, it will be added to _current_value. */
  State *accept(const char &value);

  /** Assembles a transition from state A to B on a certain char. */
  void onChar(char value, State *A, State *B);

  /** Adds a transition for anything but the given char. */
  void onNotChar(char c, State *A, State *B);

  /** Adds a transistion on white-space input. */
  void onWhiteSpace(State *A, State *B);

  /** Adds a transition on "[a-z]". */
  void onLowerAlpha(State *A, State *B);

  /** Adds a transition on "[A-Z]". */
  void onUpperAlpha(State *A, State *B);

  /** Adds a transition on "[a-zA-Z]". */
  void onAlpha(State *A, State *B);

  /** Adds a transition on "[0-9]". */
  void onNumber(State *A, State *B);

  /** Adds a transition on "[1-9]". */
  void onPosNumber(State *A, State *B);

  /** Adds a transition on "[0-9a-zA-Z]". */
  void onAlphaNum(State *A, State *B);
};


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


/**
 * Implements a general lexer.
 *
 * @ingroup parser
 */
class Lexer
{
public:
  /** Simple class to hold the lexer state. */
  class State {
  public:
    /** Holds the index of the current token. */
    size_t idx;
    /** Holds the current line number. */
    size_t line;
    /** Holds the current column number. */
    size_t column;

  public:
    /** Constructor. */
    State(size_t idx, size_t line, size_t column)
      : idx(idx), line(line), column(column) { }
  };


protected:
  /** Holds a weak reference to the input stream. */
  std::istream &input;
  /** Holds the vector of parsed tokens. */
  std::vector< Token > history;
  /** Holds stack of states. */
  std::list<State> stack;
  /** The @c NFA to process input. */
  NFA<char> reader;
  /** A set of ignored token. */
  std::set<unsigned> ignored_token;
  /** A set of tokens marking a new-line. */
  std::set<unsigned> new_line_token;
  /** Holds a translation-table Token-ID -> name. Used for debug and exceptions. */
  std::map<unsigned, std::string> token_table;


public:
  /** Constructs a general lexer for the given input. */
  Lexer(std::istream &input);

  /** Adds a rule to the lexer. */
  void addRule(const TokenRule &token);

  /** Returns the next token, reads it from input if needed. */
  const Token &next();

  /** Returns the current token.
   * If the first token was not read from the input yet, it will be read. */
  const Token &current();

  /** Returns the index of the current token. */
  size_t currentIndex() const;

  /** Saves the current state on the stack. */
  void push_state();

  /** Drops the previous state. */
  void drop_state();

  /** Restores the previous state. */
  void restore_state();

  /** Resets the lexer, conserves parsed tokens in history. */
  void reset();

  /** Registers a name for the token-id. */
  void addTokenName(unsigned id, const std::string &name);

  /** Returns the name for the token or "Token(ID)" if no name is registered. */
  std::string getTokenName(unsigned id);

  /** Adds a token to the list of ignored tokens. */
  void addIgnoredToken(unsigned id);

  /** Returns the i-th parsed token. */
  const Token &operator[] (size_t idx) const;


protected:
  /** Parses the next token from input. */
  void parseToken();
};



}
}

#endif
