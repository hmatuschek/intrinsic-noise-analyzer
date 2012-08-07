#ifndef __FLUC_TRAFO_ASSERTIONS_HH__
#define __FLUC_TRAFO_ASSERTIONS_HH__

#include "ast/ast.hh"
#include "ginacsupportforeigen.hh"


namespace Fluc {
namespace Trafo {

/** This visitor traverses a given model and throws a @c SBMLFeatureNotSupported exception if
 * is a variable has a @c RateRule.
 * @ingroup trafo */
class NoRateRuleAssertion
    : public Ast::Visitor, public Ast::VariableDefinition::Visitor
{
public:
  /** Checks if the variable has a @c RateRule. */
  virtual void visit(const Ast::VariableDefinition *var);

public:
  /** Applies the assertion on the given model.
   * @throws SBMLFeatureNotSupported If there is a rate-rule for any variable. */
  static void apply(const Ast::Model &model);
};


/** This class implements the @c NoRateRuleAssertion as a model mixin.
 * @ingroup trafo */
class NoRateRuleMixin
{
public:
  /** Constructor, throws a @c SBMLFeatureNotSupported exception if there is a @c RateRule. */
  NoRateRuleMixin(const Ast::Model &model);
};


/** This class implements an assertion that checks if there are any species defined as constant.
 * @ingroup trafo */
class NoConstSpeciesAssertion :
    public Ast::Visitor, public Ast::Species::Visitor
{
public:
  /** This method throws a @c SBMLFeatureNotSupported exception if there is a species defined as
   * constant. */
  virtual void visit(const Ast::Species *var);

public:
  /** This method applies the assertion on the model. */
  static void apply(const Ast::Model &model);
};


/** This class implements the @c NoConstSpeciesAssertion as a model mixin.
 * @ingroup trafo */
class NoConstSpeciesMixin
{
public:
  /** Constructor, checks if there are any species defined as constant. */
  NoConstSpeciesMixin(const Ast::Model &model);
};


/** This class checks if there is an @c Ast::AssignmentRule defined for any variable.
 * @ingroup trafo */
class NoAssignmentRuleAssertion :
    public Ast::Visitor, public Ast::VariableDefinition::Visitor
{
public:
  /** Checks if there is an @c AssignmentRule for the variable. */
  virtual void visit(const Ast::VariableDefinition *var);

public:
  /** Applies this assertion on the given model. */
  static void apply(const Ast::Model &model);
};


/** This class implements the @c NoAssignmentRuleAssertion as a model mixin.
 * @ingroup trafo */
class NoAssignmentRuleMixin
{
public:
  /** Constructor, throws a @ SBMLFeatureNotSupported exception if there is an @c AssginmentRule
   * defined for any variable. */
  NoAssignmentRuleMixin(const Ast::Model &model);
};


/** This class checks if there is no algebraic constraint defined.
 * @ingroup trafo */
class NoAlgebraicConstraintAssertion :
    public Ast::Visitor, public Ast::Constraint::Visitor
{
public:
  /** Checks if the constraint is an @c AlgebraicConstraint. */
  virtual void visit(const Ast::Constraint *constraint);

public:
  /** Performs the assertion on the given model. */
  static void apply(const Ast::Model &model);
};


/** This class implements the @c NoAlgebraicConstraintAssertion as a model mixin.
 * @ingroup trafo */
class NoAlgebraicConstraintMixin
{
public:
  /** Checks if the given model has an algebraic constraint. */
  NoAlgebraicConstraintMixin(const Ast::Model &model);
};


/** This class checks if there are reversible reactions.
 * @ingroup trafo */
class NoReversibleReactionAssertion :
    public Ast::Visitor, public Ast::Reaction::Visitor
{
public:
  /** This method checks if the reaction is reversible. */
  virtual void visit(const Ast::Reaction *reac);

public:
  /** This method checks if the given model has a reversible reaction. */
  static void apply(const Ast::Model &model);
};


/** This class implements the @c NoReversibleReactionAssertion as a model mixin.
 * @ingroup trafo */
class NoReversibleReactionMixin
{
public:
  /** Constructor, throws a @c SBMLFeatureNotSupported exception if there is a reaction defined as
   * reversible. */
  NoReversibleReactionMixin(const Ast::Model &model);
};


/** This class checks if all paramters are defined constant.
 * @ingroup trafo */
class ConstParameterAssertion :
    public Ast::Visitor, public Ast::Parameter::Visitor
{
public:
  /** Checks if the paramter is defined as constant. */
  virtual void visit(const Ast::Parameter *param);

public:
  /** Applies the constraint on the given model. */
  static void apply(const Ast::Model &model);
};


/** This class implements the @c ConstParameterAssertion as a model mixin.
 * @ingroup trafo */
class ConstParamteterMixin
{
public:
  /** Throws an exception if there is a paramter defined, that is not constant. */
  ConstParamteterMixin(const Ast::Model &model);
};


/** This class checks if all compartments are defined as constant.
 * @ingroup trafo */
class ConstCompartmentAssertion :
    public Ast::Visitor, public Ast::Compartment::Visitor
{
public:
  /** Checks if the given compartment is constant. */
  virtual void visit(const Ast::Compartment *comp);

public:
  /** Applies the assertion on the given model. */
  static void apply(const Ast::Model &model);
};


/** This class checks if the stoichiomerty expressions are all constant.
 * @ingroup trafo */
class ConstStoichiometryAssertion :
    public Ast::Visitor, public Ast::Reaction::Visitor
{
public:
  /** Checks if the stoichiometry expressions of the given reaction are constant. */
  virtual void visit(const Ast::Reaction *reac);

public:
  /** Applies the assertion on the given model. */
  static void apply(const Ast::Model &model);
};


/** This class checks if any expression is explict time-dependent.
 * @ingroup trafo */
class NoExplicitTimeDependenceAssertion :
    public Ast::Visitor, Ast::VariableDefinition::Visitor, Ast::Rule::Visitor, Ast::KineticLaw::Visitor
{
protected:
  /** Holds a reference to the global time-symbol. */
  GiNaC::symbol _time_symbol;

public:
  /** Constructor.
   * @param time_symbol Specifies the global time symbol, usually the one returned by
   *        @c Ast::Model::getTime. */
  NoExplicitTimeDependenceAssertion(GiNaC::symbol time_symbol);

public:
  /** Checks if the initial value of the expression is explicitly time dependent.
   * Also checks the assigned rule if there is one. */
  virtual void visit(const Ast::VariableDefinition *var);
  /** Checks if the rule is explicitly time-dependent. */
  virtual void visit(const Ast::Rule *rule);
  /** Checks if the rate law expression is explicitly time dependent. Also checks the local
   * paramters. */
  virtual void visit(const Ast::KineticLaw *law);

public:
  /** Applies the assertion on the given model. */
  static void apply(const Ast::Model &model);
};


/** This class unifies some assertions that are required for a "reasonable" model.
 * @ingroup trafo */
class ReasonableModelAssertion
    : public NoExplicitTimeDependenceAssertion,
    public NoAlgebraicConstraintAssertion, public ConstParameterAssertion,
    public NoAssignmentRuleAssertion, public NoRateRuleAssertion
{
public:
  /** Constructor. */
  ReasonableModelAssertion(GiNaC::symbol time_symbol);

public:
  /** Applies the @c NoRateRuleAssertion, @c NoAssignmentRuleAssertion and
   * @c NoExplicitTimeDependenceAssertion on the variable. */
  virtual void visit(const Ast::VariableDefinition *var);

  /** Applies the @c NoAlgebraicConstraintAssertion on the given constraint. */
  virtual void visit(const Ast::Constraint *constraint);

  /** Applies the @c ConstParamterAssertion on the parameter. */
  virtual void visit(const Ast::Parameter *param);

  /** Applies the @c NoExplicitTimeDependenceAssertion on the given rule. */
  virtual void visit(const Ast::Rule *rule);

  /** Applies the @c NoExplicitTimeDependenceAssertion on the given kinetic law. */
  virtual void visit(const Ast::KineticLaw *law);

public:
  /** Applies the assertions on the given model. */
  static void apply(const Ast::Model &model);
};


/** This class ensures, that all assignment rules are linear functions of the species.
 * I.e.: S3 := S1 + S2, but not S3 = S1**2 + S2!
 * @ingroup trafo */
class LinearAssignmentRuleAssertion :
    public Ast::Visitor, public Ast::AssignmentRule::Visitor
{
protected:
  /** List of all species symbols. */
  GiNaC::matrix _species_symbols;
  /** Vector of expressions, assembles assingment rule as scalar product of this vector and
   * the vector of species symbols. */
  GiNaC::matrix _link_vector;

public:
  /** Constructor. Assembles  @c _species_list from given model. */
  LinearAssignmentRuleAssertion(const Ast::Model &model);

public:
  /** Checks if the given @c AssignmentRule is linear in the species of the model. */
  virtual void visit(const Ast::AssignmentRule *rule);

public:
  /** Applies the constraint on the given model. */
  static void apply(const Ast::Model &model);
};


}
}

#endif
