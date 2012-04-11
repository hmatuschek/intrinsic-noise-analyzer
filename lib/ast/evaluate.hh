#ifndef __FLUC_COMPILER_AST_EVALUATE_HH__
#define __FLUC_COMPILER_AST_EVALUATE_HH__

#include "model.hh"
#include "trafo/walker.hh"
#include <ginac/ginac.h>
#include <eigen3/Eigen/Eigen>



namespace Fluc {
namespace Ast {


/**
 * Evaluates an @c GiNaC::ex (expression) instance using the internal value table. This
 * class does not implement the association between symbols and values, this has to be done in a
 * class derived from this one. For example as in @c EvaluateInitialValue or @c EvaluateByIndex.
 *
 * @note This kind of expression-evaluation is quite slow, use the byte-code interpreter instead.
 *       @c Fluc::Intprt::Interpreter for faster evaluation.
 *
 * @ingroup ast
 */
class Evaluate
{
protected:
  /**
   * Associates symbols with values (also expressions).
   */
  GiNaC::exmap value_table;

  /**
   * (Re-) Sets the value for a symbol (variable).
   */
  void setValue(const GiNaC::symbol &sy, const GiNaC::ex &val);


public:
  /**
   * Constructs a new direct evaluation interpreter.
   */
  Evaluate();

  /**
   * Constructs a new evaluator with the given value table.
   */
  Evaluate(const GiNaC::exmap &table);

  /**
   * Evaluates the given expression.
   */
  virtual double evaluate(const GiNaC::ex &expr);
};




/**
 * Evaluates expression while evaluating the value of variables as their initial value.
 *
 * @ingroup ast
 */
class EvaluateInitialValue : public Evaluate, protected Trafo::Walker
{
public:
  /**
   * Constructs a initial value evaluator, by collecting all initial values of all variables in
   * the given module and associating these variables with their initial values.
   */
  EvaluateInitialValue(Module &module);

protected:
  /**
   * Gets the initial value of the variable (if there is one). These values are then used to
   * evaluate the expressions passed to @c evaluate.
   */
  virtual void handleVariableDefinition(Ast::VariableDefinition *node);
};




/**
 * Evaluates an expression by resolving the symbols of the expression using a value vector.
 * (For example the state vector of an ODE). Therefore, the constructor takes a table, that
 * associates symbols or variables with indices, during evaluation, symbols are resolved using
 * the values provides by a vector.
 *
 * @ingroup ast
 */
class EvaluateByIndex: public Evaluate
{
protected:
  /**
   * Holds the index-mapping symbol->index.
   */
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> index_table;

public:
  /**
   * Constructs a new evaluator using the given index-mapping.
   */
  EvaluateByIndex(const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table);

  /**
   * Updated the values for the index variables.
   */
  void setValues(const Eigen::VectorXd &values);
};


/**
 * Implements the direct evaluation scheme of GiNaC expressions.
 *
 * The values of symbols, associated with Species defined in the model are taken from a vector. All
 * other symbols have to be resolved previously.
 *
 * @ingroup ast
 */
class EvaluateModel : public Evaluate
{
protected:
  /**
   * Holds a weak reference to the @c Ast::Model instance, that defines the indices of the
   * species symbols.
   */
  Ast::Model &model;

public:
  /**
   * Constructs a direct interpreter for GiNaC expressions from the given @c Ast::Model.
   */
  EvaluateModel(Ast::Model &model);

  /**
   * Sets the value of the species.
   */
  void setValues(const Eigen::VectorXd &values);
};


}
}

#endif // __FLUC_COMPILER_AST_EVALUATE_HH__
