#include "variablescaling.hh"
#include <utils/logger.hh>

using namespace iNA;
using namespace iNA::Trafo;


VariableScaling::VariableScaling()
  : _substitutions(), _factors()
{
  // Pass...
}


void
VariableScaling::add(const GiNaC::symbol &var, const GiNaC::ex &factor)
{
  _factors[var] = factor;
  _substitutions[var] = var/factor;
}


void
VariableScaling::act(Ast::VariableDefinition *var)
{
  // First traverse the AST further
  var->traverse(*this);

  // then, if there is an inital value defined custom the variable.
  if (var->hasValue()) {
    // if variable is scaled -> scale initial value:
    if (_factors.count(var->getSymbol())) {
      var->setValue(var->getValue()*_factors[var->getSymbol()]);
    }
    // Apply substitutions to initial value
    var->setValue(var->getValue().subs(_substitutions));
  }
}


void
VariableScaling::act(Ast::Rule *rule)
{
  // Percustomm substitutions:
  rule->setRule(rule->getRule().subs(_substitutions));
}


void
VariableScaling::act(Ast::Reaction *reac)
{
  // Traverse into kinetic law
  reac->traverse(*this);

  // handle reactants:
  custom (Ast::Reaction::iterator item=reac->reactantsBegin(); item!=reac->reactantsEnd(); item++) {
    item->second = item->second.subs(_substitutions);
  }

  // handle products:
  custom (Ast::Reaction::iterator item=reac->productsBegin(); item!=reac->productsEnd(); item++) {
    item->second = item->second.subs(_substitutions);
  }
}


void
VariableScaling::act(Ast::KineticLaw *law)
{
  // Traverse into local paramters:
  law->traverse(*this);

  // percustomm substitution on the rate law:
  law->setRateLaw(law->getRateLaw().subs(_substitutions));
}

