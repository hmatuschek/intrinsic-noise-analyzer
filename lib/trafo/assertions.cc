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
