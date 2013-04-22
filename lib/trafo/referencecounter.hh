#ifndef __INA_TRAFO_REFERENCECOUNTER_HH__
#define __INA_TRAFO_REFERENCECOUNTER_HH__

#include "../ast/ast.hh"

namespace iNA {
namespace Trafo {

/** This class terverses the @c Ast::Model and collects all references to a defined variable.
 * @ingroup trafo */
class ReferenceCounter
    : public iNA::Ast::Visitor, public iNA::Ast::Compartment::Visitor,
    public iNA::Ast::Species::Visitor, public iNA::Ast::Parameter::Visitor,
    public iNA::Ast::AlgebraicConstraint::Visitor, public iNA::Ast::Reaction::Visitor,
    public iNA::Ast::KineticLaw::Visitor
{
public:
  /** Constructor, specifies the variable to be searched for. */
  explicit ReferenceCounter(const iNA::Ast::VariableDefinition *var);
  /** Processes the volume expression of the compartment.*/
  void visit(const iNA::Ast::Compartment *var);
  /** Processes the species initial value and optional rule expression. */
  void visit(const iNA::Ast::Species *var);
  /** Processes the parameter intitial value and rule expression. */
  void visit(const iNA::Ast::Parameter *var);
  /** Processes global algebraic constraint expressions. */
  void visit(const iNA::Ast::AlgebraicConstraint *var);
  /** Process reaction. */
  void visit(const iNA::Ast::Reaction *reac);
  /** Process kinetic law expression and local parameters. */
  void visit(const iNA::Ast::KineticLaw *law);

  /** Returns number of references found. */
  size_t references() const;
  /** Resets the counter. */
  void reset();

  /** Counts the reference of the given variable found the the given model, not counting the
   * variable itself. */
  static size_t count(const Ast::VariableDefinition *var, const Ast::Model &model);

private:
  /** Counts the references of this variable. */
  const iNA::Ast::VariableDefinition *_var;
  /** Collects the references found. */
  size_t _references;
};

}
}


#endif
