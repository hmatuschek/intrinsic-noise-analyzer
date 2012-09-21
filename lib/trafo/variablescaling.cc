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
  Utils::Message msg = LOG_MESSAGE(Utils::Message::DEBUG);
  msg << "Scale " << var << " with " << factor;
  Utils::Logger::get().log(msg);
}


void
VariableScaling::act(Ast::VariableDefinition *var)
{
  // First traverse the AST further
  var->traverse(*this);

  Utils::Message msg = LOG_MESSAGE(Utils::Message::DEBUG);
  msg << "Process variable " << var->getIdentifier();
  Utils::Logger::get().log(msg);

  // then, if there is an inital value defined for the variable.
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
  // Perform substitutions:
  rule->setRule(rule->getRule().subs(_substitutions));
}


void
VariableScaling::act(Ast::Reaction *reac)
{
  // Traverse into kinetic law
  reac->traverse(*this);

  // handle reactants:
  for (Ast::Reaction::iterator item=reac->reacBegin(); item!=reac->reacEnd(); item++) {
    item->second = item->second.subs(_substitutions);
  }

  // handle products:
  for (Ast::Reaction::iterator item=reac->prodBegin(); item!=reac->prodEnd(); item++) {
    item->second = item->second.subs(_substitutions);
  }
}


void
VariableScaling::act(Ast::KineticLaw *law)
{
  // Traverse into local paramters:
  law->traverse(*this);

  // perform substitution on the rate law:
  law->setRateLaw(law->getRateLaw().subs(_substitutions));
}

