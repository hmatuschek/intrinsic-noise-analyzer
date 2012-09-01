#include "referencecounter.hh"
#include <QApplication>
#include <QString>

using namespace iNA;


ReferenceCounter::ReferenceCounter(iNA::Ast::VariableDefinition *var, QObject *parent)
  : QObject(parent), _var(var)
{
  // Pass...
}


void
ReferenceCounter::visit(const iNA::Ast::Compartment *var)
{
  QString name = var->getIdentifier().c_str();
  if (var->hasName()) { name = QString("%1 (id: %2)").arg(var->getName().c_str(), name); }

  if (var->hasValue() && var->getValue().has(_var->getSymbol())) {
    _references.push_back(tr("in initial value of compartment %1").arg(name));
  }

  if (var->hasRule() && var->getRule()->getRule().has(_var->getSymbol())) {
    _references.push_back(tr("in rule of compartment %1").arg(name));
  }
}

void
ReferenceCounter::visit(const iNA::Ast::Species *var)
{
  QString name = var->getIdentifier().c_str();
  if (var->hasName()) { name = QString("%1 (id: %2)").arg(var->getName().c_str(), name); }

  if (var->hasValue() && var->getValue().has(_var->getSymbol())) {
    _references.push_back(tr("in initial value of species %1").arg(name));
  }

  if (var->hasRule() && var->getRule()->getRule().has(_var->getSymbol())) {
    _references.push_back(tr("in rule of species %1").arg(name));
  }

  if (var->getCompartment()->getSymbol() == _var->getSymbol()) {
    _references.push_back(tr("as compartment of species %1").arg(name));
  }
}


void
ReferenceCounter::visit(const iNA::Ast::Parameter *var)
{
  QString name = var->getIdentifier().c_str();
  if (var->hasName()) { name = QString("%1 (id: %2)").arg(var->getName().c_str(), name); }

  if (var->hasValue() && var->getValue().has(_var->getSymbol())) {
    _references.push_back(tr("in initial value of parameter %1").arg(name));
  }

  if (var->hasRule() && var->getRule()->getRule().has(_var->getSymbol())) {
    _references.push_back(tr("in rule of parameter %1").arg(name));
  }
}


void
ReferenceCounter::visit(const iNA::Ast::AlgebraicConstraint *var)
{
  if (var->getConstraint().has(_var->getSymbol())) {
    _references.push_back(tr("in an algebraic constraint"));
  }
}


void
ReferenceCounter::visit(const iNA::Ast::Reaction *reac)
{
  // First traverse into kinetic law etc...
  reac->traverse(*this);

  QString name = reac->getIdentifier().c_str();
  if (reac->hasName()) { name = QString("%1 (id: %2)").arg(reac->getName().c_str(), name); }

  // Check reactants:
  for (Ast::Reaction::const_iterator item=reac->reacBegin(); item!=reac->reacEnd(); item++) {
    if (item->first->getSymbol() == _var->getSymbol()) {
      _references.push_back(tr("as reactant in reaction %1").arg(name));
    }
  }

  // Check products:
  for (Ast::Reaction::const_iterator item=reac->prodBegin(); item!=reac->prodEnd(); item++) {
    if (item->first->getSymbol() == _var->getSymbol()) {
      _references.push_back(tr("as product in reaction %1").arg(name));
    }
  }

  // Check modifier:
  for (Ast::Reaction::const_mod_iterator item=reac->modBegin(); item!=reac->modEnd(); item++) {
    if ((*item)->getSymbol() == _var->getSymbol()) {
      _references.push_back(tr("as modifier in reaction %1").arg(name));
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
    _references.push_back(tr("in a kinetic law"));
  }
}


const QList<QString> &
ReferenceCounter::references() const {
  return _references;
}
