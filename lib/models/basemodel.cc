#include "basemodel.hh"

using namespace Fluc;
using namespace Fluc::Models;


BaseModel::BaseModel(libsbml::Model *sbml_model)
    : Ast::Model(sbml_model), Convert2IrreversibleMixin((Ast::Model &)(*this)),
    species(numSpecies()), reactions(numReactions()),
    propensities(numReactions())
{
  // Collect all constants and assignment rules, that are needed to be substituted
  // before evaluation:
  Ast::Trafo::SubstitutionCollector collector(this->constant_substitution_table);
  collector.handleModule(this);

  // Iterate over all species in SBML model:
  for (size_t i=0; i<this->numSpecies(); i++)
  {
    this->species[i] = this->getSpecies(i)->getSymbol();
  }

  // Iterate over all reactions in SBML model:
  for (size_t i=0; i<this->numReactions(); i++)
  {
    Ast::Reaction *reac = this->getReaction(i);
    this->reactions[i]    = reac;
    this->propensities[i] = reac->getKineticLaw()->getRateLaw();
  }
}


BaseModel::BaseModel(const Ast::Model &model)
  : Ast::Model(model), Convert2IrreversibleMixin((Ast::Model &)(*this)),
    species(numSpecies()), reactions(numReactions()),
    propensities(numReactions())
{
  // Collect all constants and assignment rules, that are needed to be substituted
  // before evaluation:
  Ast::Trafo::SubstitutionCollector collector(this->constant_substitution_table);
  collector.handleModule(this);

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


GiNaC::ex
BaseModel::foldConstants(GiNaC::ex expression)
{
  return expression.subs(this->constant_substitution_table);
}

