#ifndef __FLUC_COMPILER_AST_FUNCTIONDEFINITION_HH__
#define __FLUC_COMPILER_AST_FUNCTIONDEFINITION_HH__

#include <string>
#include <vector>
#include <ginac/ginac.h>

#include "definition.hh"
#include "scope.hh"
#include "variabledefinition.hh"


namespace iNA {
namespace Ast {


/**
 * A specialized variant of the @c VariableDefinition class representing a function argument
 *
 * @ingroup ast
 */
class FunctionArgument : public VariableDefinition
{
public:
  /** Visitor class custom function arguments. */
  class Visitor { public: virtual void visit(const FunctionArgument *arg) = 0; };
  /** Operator class custom function arguments. */
  class Operator { public: virtual void act(FunctionArgument *arg) = 0; };

public:
  /** Constructs a new functoion argument.
   * @param id Specifies the identifier of the argument. */
  FunctionArgument(const std::string &id);

  /** Handles a visitor custom the function argument. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator on the function argument. */
  virtual void apply(Ast::Operator &op);
};



/**
 * Represents a function definition. A function definition is quiet simple structured. The body
 * consists just of a single expression which can only refer to arguments and not to parameters.
 *
 * @ingroup ast
 */
class FunctionDefinition : public Definition, public Scope
{
public:
  /** Visitor class custom function definitions. */
  class Visitor { public: virtual void visit(const FunctionDefinition *func) = 0; };
  /** Operator class custom function definitions. */
  class Operator { public: virtual void act(FunctionDefinition *func) = 0; };

  /** Defines the iterator type over function argument definitions. */
  typedef std::vector<GiNaC::symbol>::iterator ArgIterator;


protected:
  /**
   * Holds the function argument definitions.
   */
  std::vector<GiNaC::symbol> arguments;

  /**
   * Holds the function body.
   */
  GiNaC::ex function_body;


public:
  /**
   * Constructs a new function definition.
   *
   * @param id Specifies the name/identifier of the function.
   * @param argdef Specifies the argument definitions of the function. The ownership of the
   *        definitions are taken by the function definition.
   * @param body Specifies the function body (simply an expression). The ownership of the expression
   *        is taken by the function definition.
   */
  FunctionDefinition(const std::string &id, std::vector<VariableDefinition *> &argdef, GiNaC::ex body);

  /**
   * Destructor, does nothing (yet).
   */
  virtual ~FunctionDefinition();

  /**
   * Returns the number of arguments.
   */
  size_t getNumArgs();

  /**
   * Returns the i-th argument definition.
   *
   * @param i Specifies the index of the argument definition.
   */
  GiNaC::symbol getArgByIdx(size_t i);

  /**
   * Retunrs an iterator over the argument definitions pointing to the first argument.
   */
  ArgIterator argsBegin();

  /**
   * Returns an iterator over the argument definitino pointing right after the last argument.
   */
  ArgIterator argsEnd();

  /**
   * Returns a weak reference to the function body.
   */
  GiNaC::ex getBody();

  /**
   * (Re-) Sets the function body.
   */
  void setBody(GiNaC::ex body);

  /**
   * Retunrs an expression that is the function body, where the function arguments are substituted
   * with the given vector of expressions.
   */
  GiNaC::ex inlineFunction(GiNaC::exvector &args);

  /**
   * Dumps a string representation of the function definition into the given stream.
   */
  virtual void dump(std::ostream &str);

  /** Handles a visitor custom function definitions. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator on the function definition. */
  virtual void apply(Ast::Operator &op);

  /** customwards the visitor to function arguments. */
  virtual void traverse(Ast::Visitor &visitor) const;

  /** Applies the operator on function arguments. */
  virtual void traverse(Ast::Operator &op);
};

}
}

#endif
