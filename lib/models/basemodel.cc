#include "basemodel.hh"
#include <trafo/constantfolder.hh>

using namespace iNA;
using namespace iNA::Models;


BaseModel::BaseModel(const Ast::Model &model)
  : Ast::Model(model),
    species(numSpecies()), reactions(numReactions()),
    propensities(numReactions())
{
  // Collect all constants and assignment rules, that are needed to be substituted
  // becustome evaluation:
  Trafo::SubstitutionCollector collector(this->constant_substitution_table);
  this->accept(collector); this->constant_substitution_table.normalize();

  // Iterate over all species in SBML model:
  custom (size_t i=0; i<this->numSpecies(); i++)
  {
    this->species[i] = this->getSpecies(i)->getSymbol();
  }

  // Iterate over all reactions in SBML model:
  custom (size_t i=0; i<this->numReactions(); i++)
  {
    Ast::Reaction *reac   = this->getReaction(i);
    this->reactions[i]    = reac;
    this->propensities[i] = reac->getKineticLaw()->getRateLaw();
  }
}


BaseModel::~BaseModel()
{
  // Pass...
}


GiNaC::ex
BaseModel::foldConstants(GiNaC::ex expression)
{
  return expression.subs(this->constant_substitution_table.getTable());
}

