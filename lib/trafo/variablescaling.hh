#ifndef __INA_TRAFO_VARIABLESCALING_HH__
#define __INA_TRAFO_VARIABLESCALING_HH__

#include <ast/ast.hh>

namespace iNA {
namespace Trafo {

/** This simple operator scales variables within a @c Ast::Model and maintains an consistent model.
 * This operator is particulary useful when changeing units of variables without modifying the
 * model at all. The scaleing \f$x\rightarrow x' : x' = s\cdot x\f$ such that all occurrences of x
 * are replaced by x/f and the initial value of x, x(0) is replaced with f*x(0).
 * @ingroup trafo */
class VariableScaling
    : public Ast::Operator, Ast::VariableDefinition::Operator, public Ast::Rule::Operator,
    public Ast::Reaction::Operator, public Ast::KineticLaw::Operator
{
protected:
  /** Holds the substitutions for the variables. */
  GiNaC::exmap _substitutions;
  /** Holds the factors for each variable. */
  std::map<GiNaC::symbol, GiNaC::ex, GiNaC::ex_is_less> _factors;

public:
  /** Empty constructor. */
  VariableScaling();

  /** Adds a scaling of the given variable symbol by the given factor. */
  void add(const GiNaC::symbol &var, const GiNaC::ex &factor);

public:
  /** Performs substitutions on the intial value of the variable, if defined.
   * Variables are @c Ast::Parameter, @c Ast::Compartment and @c Ast::Species definitions. */
  virtual void act(Ast::VariableDefinition *var);

  /** Performs substitutions on the rule expression. */
  virtual void act(Ast::Rule *rule);

  /** Performs substitutions on all stoichiometry expressions. */
  virtual void act(Ast::Reaction *reac);

  /** Performs substitutions on @c Ast::KineticLaw. */
  virtual void act(Ast::KineticLaw *law);
};

}
}

#endif // __INA_TRAFO_VARIABLESCALING_HH__
