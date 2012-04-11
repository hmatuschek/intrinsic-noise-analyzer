#ifndef __FLUC_COMPILER_AST_NODE_HH__
#define __FLUC_COMPILER_AST_NODE_HH__

#include <map>
#include <iostream>


namespace Fluc {
namespace Ast {


/**
 * Baseclass for all AST nodes.
 *
 * Does almost nothing, except holding the location in the sources and type of the node.
 *
 * @ingroup ast
 */
class Node
{
public:
  /**
   * Specifies all node types.
   */
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
  virtual NodeType getNodeType();

  /**
   * A pure-virtual method that has to be implemented by sub-sclasses.
   *
   * This method shoud dump a simple string representation of the instance into the given stream.
   */
  virtual void dump(std::ostream &str) = 0;


  /*
   * A collection of helper functions to test for AST node types.
   */
public:
  /**
   * Retunrs true, if the given node is a constraint.
   */
  static bool isConstraint(Node *node);

  /**
   * Returns true, if the given node is an algebraic constraint.
   */
  static bool isAlgebraicConstraint(Node *node);

  /**
   * Retunrs true, if given node is a definition, ie. a FunctionDefinition, etc.
   */
  static bool isDefinition(Node *node);

  /**
   * Retunrs true, if the given node is a function definition.
   */
  static bool isFunctionDefinition(Node *node);

  /**
   * Returns true if the given node is a variable definition.
   */
  static bool isVariableDefinition(Node *node);

  /**
   * Returns true, if the given node is a species definition.
   */
  static bool isSpecies(Node *node);

  /**
   * Returns true, if the given node is a parameter definition.
   */
  static bool isParameter(Node *node);

  /**
   * Returns true, if the given node is a compartment definition.
   */
  static bool isCompartment(Node *node);

  /**
   * Returns true if the given node is a reaction definition.
   */
  static bool isReactionDefinition(Node *node);

  /**
   * Returns true, if the given node is a unit-definition.
   */
  static bool isUnitDefinition(Node *node);

  /**
   * Retunrs true, if the given node is a kinetic law.
   */
  static bool isKineticLaw(Node *node);

  /**
   * Retunrs true, if the given node is a rule for a variable.
   */
  static bool isRule(Node *node);

  /**
   * Returns true, if the given node is an assignment rule for a variable.
   */
  static bool isAssignmentRule(Node *node);

  /**
   * Returns true, if the given node is a rate rule for a variable.
   */
  static bool isRateRule(Node *node);
};


}
}

#endif
