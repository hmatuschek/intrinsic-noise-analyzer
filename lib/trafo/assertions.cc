#include "assertions.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Trafo;



/* ********************************************************************************************* *
 * Implements no rate rule assertions:
 * ********************************************************************************************* */
void
NoRateRuleAssertion::visit(const Ast::VariableDefinition *var)
{
  apply(var);
}

void
NoRateRuleAssertion::apply(const Ast::VariableDefinition *var) throw (SBMLFeatureNotSupported)
{
  if (var->hasRule() && Ast::Node::isRateRule(var->getRule())) {
    SBMLFeatureNotSupported err;
    err << "Rate-rules custom variables not supported yet: "
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
NoConstSpeciesAssertion::visit(const Ast::Species *var) {
  apply(var);
}

void
NoConstSpeciesAssertion::apply(const Ast::Species *var) throw (SBMLFeatureNotSupported)
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
  apply(var);
}

void
NoAssignmentRuleAssertion::apply(const Ast::VariableDefinition *var)  throw (SBMLFeatureNotSupported)
{
  if ( var->hasRule() && Ast::Node::isAssignmentRule(var->getRule()) ) {
    SBMLFeatureNotSupported err;
    err << "Assignment rules custom variables not supported yet: "
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
 * Implements no revsersible reaction assertions:
 * ********************************************************************************************* */
void
NoReversibleReactionAssertion::visit(const Ast::Reaction *reac)
{
  apply(reac);
}

void
NoReversibleReactionAssertion::apply(const Ast::Reaction *reac) throw (SBMLFeatureNotSupported)
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
  apply(param);
}

void
ConstParameterAssertion::apply(const Ast::Parameter *param) throw (SBMLFeatureNotSupported)
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
  apply(comp);
}

void
ConstCompartmentAssertion::apply(const Ast::Compartment *comp) throw (SBMLFeatureNotSupported)
{
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



/* ********************************************************************************************* *
 * Implements constant stoichiometry assertions:
 * ********************************************************************************************* */
void
ConstStoichiometryAssertion::visit(const Ast::Reaction *reac)
{
  apply(reac);
}

void
ConstStoichiometryAssertion::apply(const Ast::Reaction *reac) throw (SBMLFeatureNotSupported)
{
  // Check reactants:
  custom (Ast::Reaction::const_iterator item=reac->reactantsBegin(); item!=reac->reactantsEnd(); item++) {
    SBMLFeatureNotSupported err;
    err << "Stoichiometry expression custom reactant " << item->first->getIdentifier()
        << " in reaction " << reac->getIdentifier() << " is not a constant! "
        << "It is: " << item->second;
    throw err;
  }

  // Check products:
  custom (Ast::Reaction::const_iterator item=reac->productsBegin(); item!=reac->productsEnd(); item++) {
    SBMLFeatureNotSupported err;
    err << "Stoichiometry expression custom product " << item->first->getIdentifier()
        << " in reaction " << reac->getIdentifier() << " is not a constant! "
        << "It is: " << item->second;
    throw err;
  }
}

void
ConstStoichiometryAssertion::apply(const Ast::Model &model)
{
  ConstStoichiometryAssertion assertion; model.accept(assertion);
}



/* ********************************************************************************************* *
 * Implements no explicit time dependence assertions:
 * ********************************************************************************************* */
NoExplicitTimeDependenceAssertion::NoExplicitTimeDependenceAssertion(GiNaC::symbol time_symbol)
  : _time_symbol(time_symbol)
{
  // pass...
}

void
NoExplicitTimeDependenceAssertion::visit(const Ast::VariableDefinition *var)
{
  // Process rule if there is one:
  if (var->hasRule()) var->getRule()->accept(*this);

  // Check if initial value is explicit time-dependent:
  if ((var->hasValue()) && (var->getValue().has(_time_symbol))) {
    SBMLFeatureNotSupported err;
    err << "Initial value of variable " << var->getIdentifier() << " is explicit time-dependent.";
    throw err;
  }
}

void
NoExplicitTimeDependenceAssertion::visit(const Ast::Rule *rule)
{
  // If the rule expression is explicitly time dependent:
  if (rule->getRule().has(_time_symbol)) {
    SBMLFeatureNotSupported err;
    err << "Rule " << rule->getRule() << " is explicitly time dependent.";
    throw err;
  }
}

void
NoExplicitTimeDependenceAssertion::visit(const Ast::KineticLaw *law)
{
  // First process local paramter definitions:
  law->traverse(*this);

  // check if rate law is explicit time-dependent:
  if (law->getRateLaw().has(_time_symbol)) {
    SBMLFeatureNotSupported err;
    err << "Kinetic law " << law->getRateLaw() << " is explicit time dependent.";
    throw err;
  }
}

void
NoExplicitTimeDependenceAssertion::apply(const Ast::Model &model)
{
  NoExplicitTimeDependenceAssertion assertion(model.getTime());
  model.accept(assertion);
}


/* ********************************************************************************************* *
 * Implements reasonable model asserion:
 * ********************************************************************************************* */
ReasonableModelAssertion::ReasonableModelAssertion(GiNaC::symbol time_symbol)
  : NoExplicitTimeDependenceAssertion(time_symbol)
{
  // pass...
}

void
ReasonableModelAssertion::visit(const Ast::VariableDefinition *var)
{
  // Apply assertions:
  NoRateRuleAssertion::apply(var);
  NoAssignmentRuleAssertion::apply(var);
  NoExplicitTimeDependenceAssertion::visit(var);

  // If there is a rule define -> check:
  if (var->hasRule()) { var->getRule()->accept(*((NoExplicitTimeDependenceAssertion *)this)); }
}

void
ReasonableModelAssertion::visit(const Ast::Parameter *param)
{
  // Apply assertions:
  ConstParameterAssertion::apply(param);
  // Check base class:
  ReasonableModelAssertion::visit((Ast::VariableDefinition *)(param));
}

void
ReasonableModelAssertion::visit(const Ast::Rule *rule)
{
  // apply assertions:
  NoExplicitTimeDependenceAssertion::visit(rule);
}

void
ReasonableModelAssertion::visit(const Ast::KineticLaw *law)
{
  // Apply assertions:
  NoExplicitTimeDependenceAssertion::visit(law);
}

void
ReasonableModelAssertion::apply(const Ast::Model &model)
{
  ReasonableModelAssertion assertion(model.getTime());
  model.accept((NoExplicitTimeDependenceAssertion &)assertion);
}



/* ********************************************************************************************* *
 * Implements linear assignment rule assertion:
 * ********************************************************************************************* */
LinearAssignmentRuleAssertion::LinearAssignmentRuleAssertion(const Ast::Model &model)
  : _species_symbols(model.numSpecies(),1), _link_vector(model.numSpecies(),1)
{
  // Collect list of species symbols:
  custom (size_t i=0; i<model.numSpecies(); i++) {
    _species_symbols(i,0) = model.getSpecies(i)->getSymbol();
  }
}

void
LinearAssignmentRuleAssertion::visit(const Ast::AssignmentRule *rule)
{
  GiNaC::ex remain = rule->getRule();

  // Try to transcustomm the rule expression into a polynomial w.r.t. species variables:
  custom (size_t i=0; i<_species_symbols.rows(); i++)
  {
    // Check degree of each variable, and
    if (1 < rule->getRule().degree(_species_symbols(i,0))) {
      SBMLFeatureNotSupported err;
      err << "Non-linear assignement rule: " << rule->getRule()
          << ": is non-linear in " << _species_symbols(i,0);
      throw err;
    }

    // get coeff
    GiNaC::ex c = rule->getRule().coeff(_species_symbols(i,0), 1);

    // check if coeff contains any symbols:
    custom (size_t j=0; j<_species_symbols(i,0); j++) {
      if (c.has(_species_symbols(j,0))) {
        SBMLFeatureNotSupported err;
        err << "Non-linear assignement rule: " << rule->getRule()
            << ": is non-linear in " << _species_symbols(i,0)
            << " or " << _species_symbols(j,0);
        throw err;
      }
    }

    // Update remainder:
    remain -= c*_species_symbols(i,0);
  }

  // Check if reaminder is 0:
  if (! remain.is_zero()) {
    SBMLFeatureNotSupported err;
    err << "Assignment rule : " << rule->getRule()
        << ": is not a linear combination of species: Remainder " << remain;
    throw err;
  }
}

void
LinearAssignmentRuleAssertion::apply(const Ast::Model &model)
{
  LinearAssignmentRuleAssertion assertion(model);
  model.accept(assertion);
}
