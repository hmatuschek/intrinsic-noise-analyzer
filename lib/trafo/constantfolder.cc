#include "constantfolder.hh"

using namespace iNA;
using namespace iNA::Trafo;



/* ********************************************************************************************* *
 * Implementation of ConstSubstitutionCollector visitor.
 * ********************************************************************************************* */
SubstitutionCollector::SubstitutionCollector(Substitution &substitutions, unsigned flags, const excludeType &excludes)
    : _substitutions(substitutions), _flags(flags) , _excludes(excludes)
{
  // pass...
}


void
SubstitutionCollector::visit(const Ast::VariableDefinition *var)
{

  // Skip to exclude
  if(_excludes.find(var->getSymbol())!=_excludes.end()) return;

  // Skip variables without assigned values:
  if (! var->hasValue()) return;

  // Skip non-constant variables if not filtered:
  if ( (! var->isConst()) && (!(_flags & Filter::NON_CONST)) ) return;

  if (Ast::Node::isSpecies(var) && (_flags & Filter::SPECIES) ) {
    _substitutions.add(var->getSymbol(), var->getValue(), false);
  } else if (Ast::Node::isParameter(var) && (_flags & Filter::PARAMETERS)) {
    _substitutions.add(var->getSymbol(), var->getValue(), false);
  } else if (Ast::Node::isCompartment(var) && (_flags & Filter::COMPARTMENTS)) {
    _substitutions.add(var->getSymbol(), var->getValue(), false);
  }
}


void
SubstitutionCollector::collect(Ast::Model *model, Substitution &substitutions, unsigned flags,
                               const excludeType &excludes, bool normalize)
{
  // Configure and apply collector on model:
  SubstitutionCollector collector(substitutions, flags, excludes);
  model->accept(collector);
  // Normalize substitutions if required:
  if (normalize) {
    substitutions.normalize();
  }
}



/* ********************************************************************************************* *
 * Implementation of InitialValueFolder
 * ********************************************************************************************* */
InitialValueFolder::InitialValueFolder(const Ast::Model &model, unsigned flags, const excludeType &excludes)
  : Substitution()
{
  // Apply const subs. collector on model
  SubstitutionCollector collector(*this, flags, excludes);
  model.accept(collector);
  // normalize collected substitutions:
  this->normalize();
}

void
InitialValueFolder::apply(Ast::Model &model)
{
  // apply substitution operator on model
  model.apply(*this);
}

GiNaC::ex
InitialValueFolder::apply(const GiNaC::ex & expr)
{
  // apply substitutions on expression:
  return expr.subs(this->getTable());
}

double
InitialValueFolder::evaluate(const GiNaC::ex &expr)
{
  // Apply substitutions on expression:
  GiNaC::ex res = GiNaC::evalf(apply(expr));
  // Assert that the result is a numeric value:
  if (! GiNaC::is_a<GiNaC::numeric>(res)) {
    SymbolError err;
    err << "Cannot evaluate expression: " << expr << ": It is not reduced to a value!"
        << " Minimal expression: " << res;
    throw err;
  }

  return GiNaC::ex_to<GiNaC::numeric>(res).to_double();
}

Eigen::MatrixXex
InitialValueFolder::apply(const Eigen::MatrixXex &vecIn)
{
  Eigen::MatrixXex vecOut;
  vecOut.resize(vecIn.rows(),vecIn.cols());

  for (int i=0; i<vecIn.rows(); i++)
    for (int j=0; j<vecIn.cols(); j++)
      vecOut(i,j)=vecIn(i,j).subs(this->getTable());

  return vecOut;
}


/* ********************************************************************************************* *
 * Implementation of ConstantFolder
 * ********************************************************************************************* */
ConstantFolder::ConstantFolder(const Ast::Model &model, unsigned flags, const excludeType &excludes)
  : InitialValueFolder(model, flags, excludes)
{
  // Done...
}

