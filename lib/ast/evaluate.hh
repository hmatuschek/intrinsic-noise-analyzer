#ifndef __FLUC_COMPILER_AST_EVALUATE_HH__
#define __FLUC_COMPILER_AST_EVALUATE_HH__

#include "model.hh"
#include <ginac/ginac.h>
#include <eigen3/Eigen/Eigen>



namespace iNA {
namespace Ast {


/**
 * @deprecated
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
