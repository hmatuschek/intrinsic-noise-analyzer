#ifndef __INA_TRAFO_CONSTANTFOLDER_HH__
#define __INA_TRAFO_CONSTANTFOLDER_HH__

#include "ast/ast.hh"
#include "substitution.hh"


namespace Fluc {
namespace Trafo {


/** Simple visitor to collect all substitutions for constant variables or variables having
 * an @c Ast::AssignmentRule.
 * @ingroup trafo */
class ConstSubstitutionCollector
    : public Ast::Visitor, public Ast::VariableDefinition::Visitor
{
protected:
  /** Weak reference to the @c Substitution operator collecting the substitutions. */
  Substitution &_substitutions;

public:
  /** Constructor. */
  ConstSubstitutionCollector(Substitution &substitutions);

  /** Check if the variable is constant or has an @c Ast::AssignmentRule attached. */
  virtual void visit(const Ast::VariableDefinition *var);
};


/**
 * This class collects all constant substitutions, means the initial values of all
 * variables declared as constant or having an @c Ast::AssignmentRule. These variables are
 * collected as a @c Substitution operator that can be applied on a whole model or on single
 * expressions.
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
  ConstantFolder(const Ast::Model &model);

  /** Tiny helper function to fold all constants in the given model. */
  void apply(Ast::Model &model);

  /** Tiny helper function to fold all constants in the given expression. */
  GiNaC::ex apply(GiNaC::ex expr);
};


}
}

#endif
