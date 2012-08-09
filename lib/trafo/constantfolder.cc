#include "constantfolder.hh"

using namespace Fluc;
using namespace Fluc::Trafo;



/* ********************************************************************************************* *
 * Implementation of ConstSubstitutionCollector visitor.
 * ********************************************************************************************* */
ConstSubstitutionCollector::ConstSubstitutionCollector(Substitution &substitutions, unsigned flags)
  : _substitutions(substitutions), _flags(flags)
{
  // pass...
}


void
ConstSubstitutionCollector::visit(const Ast::VariableDefinition *var)
{
  // Skip non constant variables:
  if ( (! var->isConst()) || (! var->hasValue()) ) return;

  if (Ast::Node::isSpecies(var) && (_flags & FROM_SPECIES) ) {
    _substitutions.add(var->getSymbol(), var->getValue(), false);
  } else if (Ast::Node::isParameter(var) && (_flags & FROM_PARAMTERS)) {
    _substitutions.add(var->getSymbol(), var->getValue(), false);
  } else if (Ast::Node::isCompartment(var) && (_flags & FROM_COMPARTMENTS)) {
    _substitutions.add(var->getSymbol(), var->getValue(), false);
  }
}



/* ********************************************************************************************* *
 * Implementation of ConstantFolder
 * ********************************************************************************************* */
ConstantFolder::ConstantFolder(const Ast::Model &model, unsigned flags)
  : Substitution()
{
  // Apply const subs. collector on model
  ConstSubstitutionCollector collector(*this, flags);
  model.accept(collector);
  // normalize collected substitutions:
  this->normalize();
}

void
ConstantFolder::apply(Ast::Model &model)
{
  // apply substitution operator on model
  model.apply(*this);
}

GiNaC::ex
ConstantFolder::apply(GiNaC::ex expr)
{
  // apply substitutions on expression:
  return expr.subs(this->getTable());
}
