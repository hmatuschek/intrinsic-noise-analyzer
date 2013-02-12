#include "basemodel.hh"
#include <trafo/constantfolder.hh>

using namespace iNA;
using namespace iNA::Models;


BaseModel::BaseModel(const Ast::Model &model)
  : Ast::Model(model),
    species(numSpecies()), reactions(numReactions()),
    propensities(numReactions())
{

  // Iterate over all species in SBML model:
  for (size_t i=0; i<this->numSpecies(); i++)
  {
    this->species[i] = this->getSpecies(i)->getSymbol();
  }

  // Iterate over all reactions in SBML model:
  for (size_t i=0; i<this->numReactions(); i++)
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
