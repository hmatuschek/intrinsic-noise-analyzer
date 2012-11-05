#ifndef __FLUC_COMPILER_AST_NODE_HH__
#define __FLUC_COMPILER_AST_NODE_HH__

#include <map>
#include <iostream>
#include "visitor.hh"


namespace iNA {
namespace Ast {


/**
 * Baseclass custom all AST nodes.
 *
 * Does almost nothing, except holding the location in the sources and type of the node.
 *
 * @ingroup ast
 */
class Node
{
public:
  /** Specifies all node types. */
  typedef enum {
    ALGEBRAIC_CONSTRAINT,   ///< Node is an algebraic constraint.
    UNIT_DEFINITION,        ///< Node is an unit-defintion.
    FUNCTION_DEFINITION,    ///< Node is a function definition.
    FUNCTION_ARGUMENT,      ///< Node is a function argument variable.
    SPECIES_DEFINITION,     ///< Node is a species definition.
    COMPARTMENT_DEFINITION, ///< Node is a compartment definition.
    PARAMETER_DEFINITION,   ///< Node is a parameter definition.
    REACTION_DEFINITION,    ///< Node is a reaction definition.
    KINETIC_LAW,            ///< Node is a kinetic law.
    ASSIGNMENT_RULE,        ///< Node is an assignment rule.
    RATE_RULE               ///< Node is a rate rule.
  } NodeType;

  /** Visitor class custom all nodes. */
  class Visitor { public: virtual void visit(const Node *node) = 0; };
  /** Operator class custom all nodes. */
  class Operator { public: virtual void act(Node *node) = 0; };

protected:
  /**
   * Holds the type of the node.
   */
  NodeType node_type;


protected:
  /**
   * Protected constructor, avoids direct instantiation of a Node.
   *
   * @param node_type Specifies the node type.
   */
  Node(NodeType type);


public:
  /**
   * Destructor, does nothin (yet).
   */
  virtual ~Node();

  /**
   * Retuns the type of the node.
   */
  virtual NodeType getNodeType() const;

  /** A pure-virtual method that has to be implemented by sub-sclasses.
   * This method shoud dump a simple string representation of the instance into the given stream. */
  virtual void dump(std::ostream &str) = 0;

  /** Handles a vistor. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Handles an operator. */
  virtual void apply(Ast::Operator &op);

  /** Does nothing, there is nothing to traverse. */
  virtual void traverse(Ast::Visitor &visitor) const;

  /** Does nothon, there is nothing to traverse. */
  virtual void traverse(Ast::Operator &op);

public:
  /**
   * Retunrs true, if the given node is a constraint.
   */
  static bool isConstraint(const Node *node);

  /**
   * Returns true, if the given node is an algebraic constraint.
   */
  static bool isAlgebraicConstraint(const Node *node);

  /**
   * Retunrs true, if given node is a definition, ie. a FunctionDefinition, etc.
   */
  static bool isDefinition(const Node *node);

  /**
   * Retunrs true, if the given node is a function definition.
   */
  static bool isFunctionDefinition(const Node *node);

  /**
   * Returns true if the given node is a variable definition.
   */
  static bool isVariableDefinition(const Node *node);

  /**
   * Returns true, if the given node is a species definition.
   */
  static bool isSpecies(const Node *node);

  /**
   * Returns true, if the given node is a parameter definition.
   */
  static bool isParameter(const Node *node);

  /**
   * Returns true, if the given node is a compartment definition.
   */
  static bool isCompartment(const Node *node);

  /**
   * Returns true if the given node is a reaction definition.
   */
  static bool isReactionDefinition(const Node *node);

  /**
   * Returns true, if the given node is a unit-definition.
   */
  static bool isUnitDefinition(const Node *node);

  /**
   * Retunrs true, if the given node is a kinetic law.
   */
  static bool isKineticLaw(const Node *node);

  /**
   * Retunrs true, if the given node is a rule custom a variable.
   */
  static bool isRule(const Node *node);

  /**
   * Returns true, if the given node is an assignment rule custom a variable.
   */
  static bool isAssignmentRule(const Node *node);

  /**
   * Returns true, if the given node is a rate rule custom a variable.
   */
  static bool isRateRule(const Node *node);
};


}
}

#endif
