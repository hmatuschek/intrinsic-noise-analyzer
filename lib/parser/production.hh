#ifndef __FLUC_PARSER_PRODUCTION_HH__
#define __FLUC_PARSER_PRODUCTION_HH__

#include "lexer.hh"
#include "exception.hh"


namespace iNA {
namespace Parser {


/**
 * Implements a concrete syntax tree (CST) node.
 *
 * The concrete syntax tree describes the path the parser has taken through the grammar. It can
 * be seen as the actual parsing result. Each production of a grammar generates a node of the
 * concrete syntax tree. An assembler is then used to build an abstract syntax tree (AST).
 *
 * @ingroup parser
 */
class ConcreteSyntaxTree
{
public:
  /** Specifies the type of the base-production. Any production is one of these types. */
  typedef enum {
    EMPTY_NODE,       ///< An empty production.
    TOKEN_NODE,       ///< A token-production.
    PRODUCTION_NODE,  ///< A list-production.
    ALTERNATIVE_NODE  ///< An alternative-production.
  } Type;


protected:
  /** Holds the type of the node, see @c Type for the possible node types. */
  Type   type;

  /** If the node is a token-production, this index is the token-index. If the node is an
   * alternative-production, the index is the index of the alternative. */
  size_t index;

  /** Holds the vector of child-productions.
   *
   * If the node is a @c PRODUCTION_NODE, the vector contains the child elements. Note, if the
   * production was an @c OptionalProduction, that generated this node, @c childen[0] contains the
   * child of the optional production if it matched. If the production was an
   * @c AlternativeProduction that generated this node, @c children[0] contains the alternative,
   * that matched. A @c TokenProduction and @c EmptyProduction has no child nodes. */
  std::vector<ConcreteSyntaxTree> children;


public:
  /** Constructor of an empty-node. Use one of the factory method to initialize a CST node. */
  ConcreteSyntaxTree();

  /** Returns the type of the node. */
  Type getType() const;

  /** Returns true, if the node is empty. This call is equivalent to
   * ConcreteSyntaxTree::EMPTY_NODE == getType(). */
  bool isEmpty() const;

  /** Returns the index of the token (if the node is a token-node). */
  size_t getTokenIdx() const;

  /** Returns the index of the alternative (if the node is an alternative-node). */
  size_t getAltIdx() const;

  /** (Re-) Sets the index of the matched alternative. */
  void setAltIdx(size_t idx);

  /** Returns true if a optional-production matched. */
  bool matched() const;

  /** Sets whether an optional-production matched. */
  void setMatched(bool matched=true);

  /** Returns the first child-node. If the node is an optional- or an alternative-node, this
   * method returns the matched node. */
  ConcreteSyntaxTree &operator* ();

  /** Returns the i-th child node. */
  ConcreteSyntaxTree &operator[] (size_t idx);


public:
  /** Configures the given node as an empty node. */
  static void asEmptyNode(ConcreteSyntaxTree &node);

  /** Configures the given node as a token node. */
  static void asTokenNode(ConcreteSyntaxTree &node, size_t index);

  /** Configures the given node as list node. */
  static void asProdNode(ConcreteSyntaxTree &node, size_t n_children);

  /** Configures the given node as an alternative node. */
  static void asAltNode(ConcreteSyntaxTree &node);

  /** Configures the given node as an optional node. */
  static void asOptNode(ConcreteSyntaxTree &node);
};



/**
 * Baseclass for all productions.
 *
 * By default this class implements a list production, where all child productions must match in
 * the given order.
 *
 * EBNF: (Child_1 Child_2 ... Child_n)
 *
 * @ingroup parser
 */
class Production
{
protected:
  /** List of all child productions. */
  std::list<Production *> elements;

protected:
  /** Hidden constructor. */
  Production();

public:
  /** Constructs a list-production called just production. */
  Production(const std::list<Production *> &elements);

  /** Constructor with var-args. */
  Production(size_t num_prod, ...);

  /** Destructor. */
  virtual ~Production();

  /** Performs parsing and assembles the CST. If the production is successful, it initializes
   * the given CST element. */
  virtual void parse(Lexer &lexer, ConcreteSyntaxTree &element);

  /** Assembles the CST.
   * @deprecated I do not know, if this is necessary any more. */
  virtual void notify(Lexer &lexer, ConcreteSyntaxTree &element);
};



/**
 * A production of a single token.
 *
 * @ingroup parser
 */
class TokenProduction: public Production
{
public:
  /** Holds the token id. */
  unsigned _id;

public:
  /** Constructor, takes the token-id to match. */
  TokenProduction(unsigned _id);

  /** Checks if the current token of the lexer matches. If the token matches, the given CST
   * element is initialized as a @c TOKEN_NODE. */
  virtual void parse(Lexer &lexer, ConcreteSyntaxTree &element);

  /** @deprecated */
  virtual void notify(Lexer &lexer, ConcreteSyntaxTree &element);
};



/**
 * A production of alternatives.
 *
 * Exactly one child-production of the alternative must match.
 *
 * EBNF: (Alt_1 | Alt_2 | ... | Alt_n)
 *
 * @ingroup parser
 */
class AltProduction: public Production
{
protected:
  /** Holds the vector of alternatives. */
  std::vector<Production *> alternatives;

protected:
  /** Hidden constructor. */
  AltProduction();

public:
  /** Constructor with var-args. */
  AltProduction(size_t num_prod, ...);

  /** Constructs an alternative production from given alternatives. */
  AltProduction(const std::vector<Production *> &alternatives);

  virtual void parse(Lexer &lexer, ConcreteSyntaxTree &element);

  virtual void notify(Lexer &lexer, ConcreteSyntaxTree &element);
};



/**
 * The empty production, matches nothing.
 *
 * @ingroup parser
 */
class EmptyProduction : public Production
{
public:
  /** Constructor. */
  EmptyProduction();

  virtual void parse(Lexer &lexer, ConcreteSyntaxTree &element);

  virtual void notify(Lexer &lexer, ConcreteSyntaxTree &element);
};



/**
 * An optional production, the child production may match.
 *
 * EBNF: [Child]
 *
 * @ingroup parser
 */
class OptionalProduction : public Production
{
protected:
  /** Holds the optional production. */
  Production *production;

public:
  /** Constructor. */
  OptionalProduction(Production *prod);

  virtual void parse(Lexer &lexer, ConcreteSyntaxTree &element);

  virtual void notify(Lexer &lexer, ConcreteSyntaxTree &element);
};


}
}

#endif
