#ifndef __FLUC_UTILS_PARSER_HH__
#define __FLUC_UTILS_PARSER_HH__

#include "lexer.hh"
#include "exception.hh"


namespace Fluc {
namespace Utils {


/**
 * Implements a concrete syntax tree node.
 *
 * @ingroup utils
 */
class ConcreteSyntaxTree
{
public:
  /**
   * Specifies the type of the base-production.
   */
  typedef enum {
    EMPTY_NODE,       ///< An empty production.
    TOKEN_NODE,       ///< A token-production.
    PRODUCTION_NODE,  ///< A list-production.
    ALTERNATIVE_NODE  ///< An alternative-production.
  } Type;


protected:
  /**
   * Holds the type of the node.
   */
  Type   type;

  /**
   * If the node is a token-production, this index is the token-index. If the node is an
   * alternative-production, the index is the index of the alternative.
   */
  size_t index;

  /**
   * Holds the vector of child-productions.
   */
  std::vector<ConcreteSyntaxTree> children;


public:
  /**
   * Constructor of an empty-node.
   */
  ConcreteSyntaxTree();

  /**
   * Returns the type of the node.
   */
  Type getType() const;

  /**
   * Returns the index of the token (if the node is a token-node).
   */
  size_t getTokenIdx() const;

  /**
   * Returns the index of the alternative (if the node is an alternative-node).
   */
  size_t getAltIdx() const;

  /**
   * (Re-) Sets the index of the matched alternative.
   */
  void setAltIdx(size_t idx);

  /**
   * Returns true if a optional-production matched.
   */
  bool matched() const;

  /**
   * Sets whether an optional-production matched.
   */
  void setMatched(bool matched=true);

  /**
   * Returns the first child-node. If the node is an optional- or an alternative-node, this
   * method returns the matched node.
   */
  ConcreteSyntaxTree &operator* ();

  /**
   * Returns the i-th child node.
   */
  ConcreteSyntaxTree &operator[] (size_t idx);


public:
  /**
   * Configures the given node as an empty node.
   */
  static void asEmptyNode(ConcreteSyntaxTree &node);

  /**
   * Configures the given node as a token node.
   */
  static void asTokenNode(ConcreteSyntaxTree &node, size_t index);

  /**
   * Configures the given node as list node.
   */
  static void asProdNode(ConcreteSyntaxTree &node, size_t n_children);

  /**
   * Configures the given node as an alternative node.
   */
  static void asAltNode(ConcreteSyntaxTree &node);

  /**
   * Configures the given node as an optional node.
   */
  static void asOptNode(ConcreteSyntaxTree &node);
};



/**
 * Baseclass for all productions.
 *
 * @ingroup utils
 */
class Production
{
protected:
  /**
   * List of all child productions.
   */
  std::list<Production *> elements;

protected:
  /**
   * Hidden constructor.
   */
  Production();

public:
  /**
   * Constructs a list-production.
   */
  Production(const std::list<Production *> &elements);

  /**
   * Constructor with var-args.
   */
  Production(size_t num_prod, ...);

  /**
   * Destructor.
   */
  virtual ~Production();

  /**
   * Performs parsing.
   */
  virtual void parse(Lexer &lexer, ConcreteSyntaxTree &element);

  /**
   * Assembles the CST.
   */
  virtual void notify(Lexer &lexer, ConcreteSyntaxTree &element);
};



/**
 * A production of a single token.
 *
 * @ingroup utils
 */
class TokenProduction: public Production
{
public:
  /**
   * Holds the token id.
   */
  unsigned id;

public:
  /**
   * Constructor, takes the token-id to match.
   */
  TokenProduction(unsigned id);

  virtual void parse(Lexer &lexer, ConcreteSyntaxTree &element);

  virtual void notify(Lexer &lexer, ConcreteSyntaxTree &element);
};



/**
 * A production of alternatives.
 *
 * @ingroup utils
 */
class AltProduction: public Production
{
protected:
  /**
   * Holds the vector of alternatives.
   */
  std::vector<Production *> alternatives;

protected:
  /**
   * Hidden constructor.
   */
  AltProduction();

public:
  /**
   * Constructor with var-args.
   */
  AltProduction(size_t num_prod, ...);

  /**
   * Constructs an alternative production from given alternatives.
   */
  AltProduction(const std::vector<Production *> &alternatives);

  virtual void parse(Lexer &lexer, ConcreteSyntaxTree &element);

  virtual void notify(Lexer &lexer, ConcreteSyntaxTree &element);
};



/**
 * The empty production.
 *
 * @ingroup utils
 */
class EmptyProduction : public Production
{
public:
  /**
   * Constructor.
   */
  EmptyProduction();

  virtual void parse(Lexer &lexer, ConcreteSyntaxTree &element);

  virtual void notify(Lexer &lexer, ConcreteSyntaxTree &element);
};



/**
 * An optional production.
 *
 * @ingroup utils
 */
class OptionalProduction : public Production
{
protected:
  /**
   * Holds the optional production.
   */
  Production *production;

public:
  /**
   * Constructor.
   */
  OptionalProduction(Production *prod);

  virtual void parse(Lexer &lexer, ConcreteSyntaxTree &element);

  virtual void notify(Lexer &lexer, ConcreteSyntaxTree &element);
};


}
}

#endif // PARSER_HH
