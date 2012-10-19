#ifndef __INA_TRAFO_CONSTANTFOLDER_HH__
#define __INA_TRAFO_CONSTANTFOLDER_HH__

#include "ast/ast.hh"
#include "filterflags.hh"
#include "substitution.hh"
#include "ginacsupportforeigen.hh"


namespace iNA {
namespace Trafo {


typedef GiNaC::exmap excludeType;

/**
 * Simple visitor to collect all substitutions for variables.
 *
 * It is possible to specify which classes of variables are processed. This allows for example to
 * skip constant folding of compartments altough they are defined as constant (SSE).
 * @ingroup trafo
 */
class SubstitutionCollector
    : public Ast::Visitor, public Ast::VariableDefinition::Visitor
{
protected:
  /** Weak reference to the @c Substitution operator collecting the substitutions. */
  Substitution &_substitutions;

  /** Holds the flags to filter which variables are processed. */
  unsigned _flags;

  /** Holds the variables to be excluded from substitution. */
  excludeType _excludes;

public:
  /** Constructor.
   * @param substitutions Specifies the set of substitutions to be populated by the collector.
   * @param flags Specifies which types of variables are collected. By default all constant
   *        variables are collected. By specifying @c Filter::ALL, it is possible to collect all
   *        initial values of even non-constant variables.
   * @param excludes Specifies a set of variable symbols, that are to be skipped during substitution
   *        collection. */
  SubstitutionCollector(Substitution &substitutions, unsigned flags=Filter::ALL_CONST,
                        const excludeType &excludes=excludeType());

  /** Check if the variable is constant. */
  virtual void visit(const Ast::VariableDefinition *var);

public:
  /** Applies a collector with the given parameters to the given model. */
  static void collect(Ast::Model *model, Substitution &substitutions,
                      unsigned flags=Filter::ALL_CONST, const excludeType &excludes=excludeType(),
                      bool normalize=false);
};


/**
 * This class allows to collect and substitute all initial values defined within a @c Ast::Model.
 * This class is particulary useful for constant folding (see @c ConstantFolder) of for the
 * evaluation of initial values.
 * @ingroup trafo
 */
class InitialValueFolder
    : public Substitution
{
public:
  /** Constructor, collects all initial values of the given model depending on the filter flags.
   * @param model Specifies the model, to collect initial values from.
   * @param flags Specifies which elements of the model to be processed, by default all
   *        variables are taken into account.
   * @param excludes A list of symbols that are not processed. */
  InitialValueFolder(const Ast::Model &model, unsigned flags=Filter::ALL, const excludeType &excludes=excludeType());

  /** Tiny helper function to fold all constants in the given model. */
  void apply(Ast::Model &model);

  /** Tiny helper function to fold all substitutions in the given expression. */
  GiNaC::ex apply(const GiNaC::ex &expr);

  /** Tiny helper function to fold all substitutions in the given expression assuming it is then
   * folded into a double value. */
  double evaluate(const GiNaC::ex &expr);

  /** Tiny helper function to fold all constants in the given vector expression. */
  Eigen::MatrixXex apply(const Eigen::MatrixXex &vecIn);
};


/**
 * This class collects all constant substitutions, means the initial values of all
 * variables declared as constant. These variables are collected as a @c Substitution operator that
 * can be applied on a whole model or on single expressions.
 *
 * @code {.cpp}
 * Fluc::Ast::Model model;
 * // load and process model.
 * // ...
 *
 * // Create Constant folder, this does no substitution, just collect all constant substitutions
 * ConstantFolder folder(model);
 *
 * // If you want to reaplace all constant in the complete model at once
 * model.apply(folder);
 *
 * // Usually you want to perform the substitutions on single expressions
 * GiNaC::ex expression;
 * expression = expression.subs(folder.getTable());
 * // or
 * expression = folder.apply(expression);
 * @endcode
 *
 * @ingroup trafo
 */
class ConstantFolder
    : public InitialValueFolder
{
public:
  /** Constructor, collects all substitutions of constant variables and assignment rules. */
  ConstantFolder(const Ast::Model &model, unsigned flags = Filter::ALL_CONST, const excludeType &excludes=excludeType());
};


}
}

#endif
