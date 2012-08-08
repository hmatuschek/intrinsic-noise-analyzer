#include "constantfolder.hh"

using namespace Fluc;
using namespace Fluc::Trafo;



/* ********************************************************************************************* *
 * Implementation of ConstSubstitutionCollector visitor.
 * ********************************************************************************************* */
ConstSubstitutionCollector::ConstSubstitutionCollector(Substitution &substitutions)
  : _substitutions(substitutions)
{
  // pass...
}


void
ConstSubstitutionCollector::visit(const Ast::VariableDefinition *var)
{
  if (var->isConst() && var->hasValue()) {
    // If variable is defined as constant and has an intial value -> add to substitutions:
    _substitutions.add(var->getSymbol(), var->getValue(), false);
  }
}



/* ********************************************************************************************* *
 * Implementation of ConstantFolder
 * ********************************************************************************************* */
ConstantFolder::ConstantFolder(const Ast::Model &model)
  : Substitution()
{
  // Apply const subs. collector on model
  ConstSubstitutionCollector collector(*this);
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
