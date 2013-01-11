#include "referencecounter.hh"
#include <QApplication>
#include <QString>

using namespace iNA;
using namespace iNA::Trafo;


ReferenceCounter::ReferenceCounter(const Ast::VariableDefinition *var)
  : _var(var), _references(0)
{
  // Pass...
}


void
ReferenceCounter::visit(const Ast::Compartment *var)
{
  if (var->hasValue() && var->getValue().has(_var->getSymbol())) {
    _references++;
  }

  if (var->hasRule() && var->getRule()->getRule().has(_var->getSymbol())) {
    _references++;
  }
}

void
ReferenceCounter::visit(const iNA::Ast::Species *var)
{
  if (var->hasValue() && var->getValue().has(_var->getSymbol())) {
    _references++;
  }

  if (var->hasRule() && var->getRule()->getRule().has(_var->getSymbol())) {
    _references++;
  }

  if (var->getCompartment()->getSymbol() == _var->getSymbol()) {
    _references++;
  }
}


void
ReferenceCounter::visit(const iNA::Ast::Parameter *var)
{
  if (var->hasValue() && var->getValue().has(_var->getSymbol())) {
    _references++;
  }

  if (var->hasRule() && var->getRule()->getRule().has(_var->getSymbol())) {
    _references++;
  }
}


void
ReferenceCounter::visit(const iNA::Ast::AlgebraicConstraint *var)
{
  if (var->getConstraint().has(_var->getSymbol())) {
    _references++;
  }
}


void
ReferenceCounter::visit(const iNA::Ast::Reaction *reac)
{
  // First traverse into kinetic law etc...
  reac->traverse(*this);

  // Check reactants:
  for (Ast::Reaction::const_iterator item=reac->reactantsBegin(); item!=reac->reactantsEnd(); item++) {
    if (item->first->getSymbol() == _var->getSymbol()) {
      _references++;
    }
  }

  // Check products:
  for (Ast::Reaction::const_iterator item=reac->productsBegin(); item!=reac->productsEnd(); item++) {
    if (item->first->getSymbol() == _var->getSymbol()) {
      _references++;
    }
  }

  // Check modifier:
  for (Ast::Reaction::const_mod_iterator item=reac->modifiersBegin(); item!=reac->modifiersEnd(); item++) {
    if ((*item)->getSymbol() == _var->getSymbol()) {
      _references++;
    }
  }
}


void
ReferenceCounter::visit(const iNA::Ast::KineticLaw *law)
{
  // Handle local paramters etc...
  law->traverse(*this);

  // Check rate law...
  if (law->getRateLaw().has(_var->getSymbol())) {
    _references++;
  }
}


size_t
ReferenceCounter::references() const {
  return _references;
}

void
ReferenceCounter::reset() {
  _references = 0;
}


size_t
ReferenceCounter::count(const Ast::VariableDefinition *var, const Ast::Model &model)
{
  ReferenceCounter counter(var);
  model.accept(counter);
  return counter.references();
}
