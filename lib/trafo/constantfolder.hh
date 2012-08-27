#ifndef __INA_TRAFO_CONSTANTFOLDER_HH__
#define __INA_TRAFO_CONSTANTFOLDER_HH__

#include "ast/ast.hh"
#include "filterflags.hh"
#include "substitution.hh"
#include "ginacsupportforeigen.hh"


namespace Fluc {
namespace Trafo {


typedef std::set<GiNaC::ex, GiNaC::ex_is_less> excludeType;

/** Simple visitor to collect all substitutions for constant variables.
 * It is possible to specify which classes of variables are processed. This allows for example to
 * skip constant folding of compartments altough they are defined as constant (SSE).
 * @ingroup trafo */
class ConstSubstitutionCollector
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

  /** Constructor. */
  ConstSubstitutionCollector(Substitution &substitutions, unsigned flags=Filter::ALL, const excludeType &excludes=excludeType());

  /** Check if the variable is constant. */
  virtual void visit(const Ast::VariableDefinition *var);
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
    : public Substitution
{
public:
  /** Constructor, collects all substitutions of constant variables and assignment rules. */
    ConstantFolder(const Ast::Model &model, unsigned flags = Filter::ALL, const excludeType &excludes=excludeType());

  /** Tiny helper function to fold all constants in the given model. */
  void apply(Ast::Model &model);

  /** Tiny helper function to fold all constants in the given expression. */
  GiNaC::ex apply(GiNaC::ex expr);

  /** Tiny helper function to fold all constants in the given vector expression. */
  void apply(Eigen::VectorXex &expr);

};


}
}

#endif
