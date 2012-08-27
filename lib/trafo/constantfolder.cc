#include "constantfolder.hh"

using namespace Fluc;
using namespace Fluc::Trafo;



/* ********************************************************************************************* *
 * Implementation of ConstSubstitutionCollector visitor.
 * ********************************************************************************************* */
ConstSubstitutionCollector::ConstSubstitutionCollector(Substitution &substitutions, unsigned flags, const excludeType &excludes)
    : _substitutions(substitutions), _flags(flags) , _excludes(excludes)
{
  // pass...
}


void
ConstSubstitutionCollector::visit(const Ast::VariableDefinition *var)
{

  // Skip to exclude
  if(_excludes.find(var->getSymbol())!=_excludes.end()) return;

  // Skip non constant variables:
  if ( (! var->isConst()) || (! var->hasValue()) ) return;

  if (Ast::Node::isSpecies(var) && (_flags & Filter::SPECIES) ) {
    _substitutions.add(var->getSymbol(), var->getValue(), false);
  } else if (Ast::Node::isParameter(var) && (_flags & Filter::PARAMETERS)) {
    _substitutions.add(var->getSymbol(), var->getValue(), false);
  } else if (Ast::Node::isCompartment(var) && (_flags & Filter::COMPARTMENTS)) {
    _substitutions.add(var->getSymbol(), var->getValue(), false);
  }
}



/* ********************************************************************************************* *
 * Implementation of ConstantFolder
 * ********************************************************************************************* */
ConstantFolder::ConstantFolder(const Ast::Model &model, unsigned flags, const excludeType &excludes)
  : Substitution()
{
  // Apply const subs. collector on model
  ConstSubstitutionCollector collector(*this, flags, excludes);
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

void
ConstantFolder::apply(Eigen::VectorXex &expr)
{
  // apply substitutions to vector:
    for(int i=0;i<expr.size();i++)
        expr(i)=expr(i).subs(this->getTable());
}

