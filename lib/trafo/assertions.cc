#include "assertions.hh"
#include "exception.hh"

using namespace Fluc;
using namespace Fluc::Trafo;



/* ********************************************************************************************* *
 * Implements no rate rule assertions:
 * ********************************************************************************************* */
void
NoRateRuleAssertion::visit(const Ast::VariableDefinition *var)
{
  if (var->hasRule() && Ast::Node::isRateRule(var->getRule())) {
    SBMLFeatureNotSupported err;
    err << "Rate-rules for variables not supported yet: "
        << " Variable " << var->getIdentifier() << " has a rate-rule.";
    throw err;
  }
}

void
NoRateRuleAssertion::apply(const Ast::Model &model) {
  NoRateRuleAssertion assertion; model.accept(assertion);
}

NoRateRuleMixin::NoRateRuleMixin(const Ast::Model &model)
{
  NoRateRuleAssertion::apply(model);
}



/* ********************************************************************************************* *
 * Implements no constant species assertions:
 * ********************************************************************************************* */
void
NoConstSpeciesAssertion::visit(const Ast::Species *var)
{
  if (var->isConst()) {
    SBMLFeatureNotSupported err;
    err << "This implementation can not handle constant species yet: "
        << " Species " << var->getIdentifier() << " is defined as constant.";
    throw err;
  }
}

void
NoConstSpeciesAssertion::apply(const Ast::Model &model)
{
  NoConstSpeciesAssertion assertion; model.accept(assertion);
}

NoConstSpeciesMixin::NoConstSpeciesMixin(const Ast::Model &model)
{
  NoConstSpeciesAssertion::apply(model);
}



/* ********************************************************************************************* *
 * Implements no assignment rule assertions:
 * ********************************************************************************************* */
void
NoAssignmentRuleAssertion::visit(const Ast::VariableDefinition *var)
{
  if ( var->hasRule() && Ast::Node::isAssignmentRule(var->getRule()) ) {
    SBMLFeatureNotSupported err;
    err << "Assignment rules for variables not supported yet: "
        << " Variable " << var->getIdentifier() << " has an assignment rule.";
    throw err;
  }
}

void
NoAssignmentRuleAssertion::apply(const Ast::Model &model)
{
  NoAssignmentRuleAssertion assertion; model.accept(assertion);
}

NoAssignmentRuleMixin::NoAssignmentRuleMixin(const Ast::Model &model)
{
  NoAssignmentRuleAssertion::apply(model);
}



/* ********************************************************************************************* *
 * Implements no algebraic constraint assertions:
 * ********************************************************************************************* */
void
NoAlgebraicConstraintAssertion::visit(const Ast::Constraint *constraint)
{
  if (Ast::Node::isAlgebraicConstraint(constraint)) {
    SBMLFeatureNotSupported err;
    err << "This implementation can not yet handle algebraic rules applied to the model.";
    throw err;
  }
}

void
NoAlgebraicConstraintAssertion::apply(const Ast::Model &model){
  NoAlgebraicConstraintAssertion assertion; model.accept(assertion);
}

NoAlgebraicConstraintMixin::NoAlgebraicConstraintMixin(const Ast::Model &model)
{
  NoAlgebraicConstraintAssertion::apply(model);
}



/* ********************************************************************************************* *
 * Implements no revsersible reaction assertions:
 * ********************************************************************************************* */
void
NoReversibleReactionAssertion::visit(const Ast::Reaction *reac)
{
  if (reac->isReversible()) {
    SBMLFeatureNotSupported err;
    err << "Reaction "
        << reac->getIdentifier() << " is defined reversible and could not be converted to irreversible.";
    throw err;
  }
}

void
NoReversibleReactionAssertion::apply(const Ast::Model &model)
{
  NoReversibleReactionAssertion assertion; model.accept(assertion);
}

NoReversibleReactionMixin::NoReversibleReactionMixin(const Ast::Model &model)
{
  NoReversibleReactionAssertion::apply(model);
}



/* ********************************************************************************************* *
 * Implements constant parameter assertions:
 * ********************************************************************************************* */
void
ConstParameterAssertion::visit(const Ast::Parameter *param)
{
  // Check if parameter is constant:
  if (! param->isConst())
  {
    SBMLFeatureNotSupported err;
    err << "This implementation can only handle constant paramters: "
        << "Parameter " << param->getIdentifier() << " is defined as non-constant.";
    throw err;
  }

  // Check if parameter has initial value:
  if (! param->hasValue())
  {
    SBMLFeatureNotSupported err;
    err << "Parameter " << param->getIdentifier() << " has no initial value assigned!";
    throw err;
  }
}

void
ConstParameterAssertion::apply(const Ast::Model &model)
{
  ConstParameterAssertion assertion; model.accept(assertion);
}

ConstParamteterMixin::ConstParamteterMixin(const Ast::Model &model)
{
  ConstParameterAssertion::apply(model);
}



/* ********************************************************************************************* *
 * Implements constant compartment assertions:
 * ********************************************************************************************* */
void
ConstCompartmentAssertion::visit(const Ast::Compartment *comp) {
  if (! comp->isConst())
  {
    SBMLFeatureNotSupported err;
    err << "This implementation can not handle non-constant compartments yet.";
    throw err;
  }
}

void
ConstCompartmentAssertion::apply(const Ast::Model &model)
{
  ConstCompartmentAssertion assertion; model.accept(assertion);
}
