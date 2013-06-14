#include "hybridmodel.hh"

using namespace iNA;
using namespace Models;

HybridModel::HybridModel(Ast::Model &model, std::vector<std::string> &soiList)
  : Ast::Model(model), ConstantStoichiometryMixin(model), external(),
    soi(*this,soiList)
{

      // Initialize list of external species with signal of interest
      std::set<Ast::Species *> exS = soi.getSpeciesOfInterest();

      // Select external reaction set given the external species exS
      std::set<Ast::Reaction *> exR;
      selectReactions(exS,exR);

      // List external reactions
      std::cerr << "List of external reactions (" << exR.size() << "):" << std::endl;
      for(std::set<Ast::Reaction *>::iterator it = exR.begin(); it!=exR.end(); it++)
      {
        std::cerr<<(*it)->getName()<<std::endl;
      }
      std::cerr << "====" << std::endl << std::endl;

      // Assemble list of external species
      this->assembleExternalSpeciesList(exR,exS);

      // List external species
      std::cerr << "List of external species (" << exS.size() << "):" << std::endl;
      for(std::set<Ast::Species *>::iterator it = exS.begin(); it!=exS.end(); it++)
      {
        std::cerr<<(*it)->getIdentifier()<<std::endl;
      }
      std::cerr << "====" << std::endl << std::endl;

      // Distill external model
      GiNaC::exmap translation_table;
      distill(this,&external,exR,translation_table);

      // List internal species
      std::cerr << "List of internal species (" << this->numSpecies() << "):" << std::endl;
      for(size_t i=0; i< this->numSpecies(); i++)
      {
        std::cerr<<this->getSpecies(i)->getIdentifier()<<std::endl;
      }
      std::cerr << "====" << std::endl << std::endl;

      // List internal reactions
      std::cerr << "List of internal reactions (" << this->numReactions() << "):" << std::endl;
      for(size_t i=0; i< this->numReactions(); i++)
      {
        std::cerr<<this->getReaction(i)->getIdentifier()<<std::endl;
      }
      std::cerr << "====" << std::endl << std::endl;

}

void
HybridModel::selectReactions(std::set<Ast::Species *> &exS, std::set<Ast::Reaction *> &exR)
{

  // Iterate over all species in list
  for(std::set<Ast::Species *>::iterator it=exS.begin();it!=exS.end();++it)
  {
      size_t idx = this->getSpeciesIdx((*it));

      // Iterate over all reactions in list
      for (size_t j=0;j<this->numReactions();j++)
      {
          if(this->stoichiometry(idx,j)!=0 && exR.find(this->getReaction(j))==exR.end())
          {
            exR.insert(this->getReaction(j));
            for(size_t i=0; i<this->numSpecies(); i++)
              if(this->stoichiometry(i,j)!=0) selectReactions(exS,exR);
          }
      }
  }

}


void
HybridModel::assembleExternalSpeciesList(const std::set<Ast::Reaction *> &exR, std::set<Ast::Species *> & exS)
{

  for(std::set<Ast::Reaction *>::iterator it=exR.begin();it!=exR.end();++it)
  {

    size_t idj=this->getReactionIdx((*it));
    for(size_t i=0; i<this->numSpecies(); i++)
    {
      if(this->stoichiometry(i,idj)!=0) exS.insert(this->getSpecies(i));
    }

  }

}

bool
HybridModel::checkConsistency(const std::set<Ast::Reaction *> &exR, std::set<Ast::Species *> & exS)
{

  // First check for emptyness
  if(!exS.size() || !exR.size()) throw("Error: External model is empty.");
  if(exS.size()==this->numSpecies() || exR.size()==this->numReactions()) throw("Error: Internal model is empty.");

  // Now check exogeneity
  for(size_t j=0; j<this->numReactions(); j++)
  {

    Ast::Reaction *reaction = this->getReaction(j);

    // Skip internal reactions
    if(exR.find(reaction)==exR.end()) continue;
    else
    {
      for(size_t i=0; i<this->numSpecies(); i++)
      {
        Ast::Species *species = this->getSpecies(i);

        // Check if external reaction is exogeneous, i.e., kinetic laws depend only on external species
        bool intSpecies = exS.find(species)==exS.end();
        if(intSpecies && reaction->getKineticLaw()->getRateLaw().has(species->getSymbol()))
          return false;
      }
    }
  }

  return true;

}

void
HybridModel::distill(Ast::Model * src, Ast::Model *external,
             std::set<Ast::Reaction *> exR,
             GiNaC::exmap &translation_table)
{


    // Assemble list of external species
    std::set<Ast::Species *> exS;
    this->assembleExternalSpeciesList(exR,exS);

    // ...and check for consistency first
    if(!checkConsistency(exR,exS))
    {
      iNA::InternalError err("Model does not define an external process!");
      throw err;
    }

    // Table of species copies:
    std::map<Ast::Species *, Ast::Species *> species_table;

    // Set identifier of model:
    external->setIdentifier(src->getIdentifier());

    // Set name:
    external->setName(src->getName());

    // add time symbol to translation table:
    translation_table[src->getTime()] = external->getTime();

    // Sorry, iteration is quiet in-efficient, however:

    // Copy function definitions:
    for (Ast::Model::iterator iter = src->begin(); iter != src->end(); iter++) {
      if (Ast::Node::isFunctionDefinition(*iter)) {
        external->addDefinition(Ast::ModelCopyist::copyFunctionDefinition(
                              static_cast<Ast::FunctionDefinition *>(*iter), translation_table));
      }
    }

    // Copy default units:
    external->setSubstanceUnit(src->getSubstanceUnit().asScaledBaseUnit(), false);
    external->setVolumeUnit(src->getVolumeUnit().asScaledBaseUnit(), false);
    external->setAreaUnit(src->getAreaUnit().asScaledBaseUnit(), false);
    external->setLengthUnit(src->getLengthUnit().asScaledBaseUnit(), false);
    external->setTimeUnit(src->getTimeUnit().asScaledBaseUnit(), false);

    // Copy user defined "specialized" units:
    for (Ast::Model::iterator iter = src->begin(); iter != src->end(); iter++) {
      if (Ast::Node::isUnitDefinition(*iter)) {
        external->addDefinition(Ast::ModelCopyist::copyUnitDefinition(
                              static_cast<Ast::UnitDefinition *>(*iter)));
      }
    }

    // Copy all parameter definitions:
    for (size_t i=0; i<src->numParameters(); i++) {
      external->addDefinition(Ast::ModelCopyist::copyParameterDefinition(
                            static_cast<Ast::Parameter *>(src->getParameter(i)),
                            translation_table));
    }

    // Copy all compartments:
    for (size_t i=0; i<src->numCompartments(); i++) {
      external->addDefinition(Ast::ModelCopyist::copyCompartmentDefinition(
                            static_cast<Ast::Compartment *>(src->getCompartment(i)),
                            translation_table));
    }

    // Register external species definition:
    for (std::set<Ast::Species *>::iterator it=exS.begin(); it!=exS.end(); it++) {
      external->addDefinition((*it));
    }

    // Copy external reactions:
    for (std::set<Ast::Reaction *>::iterator it=exR.begin(); it!=exR.end(); it++) {
      external->addDefinition(Ast::ModelCopyist::copyReaction(
                            static_cast<Ast::Reaction *>((*it)),
                            translation_table, species_table));
    }

    /*
     * Now, update all expressions by resolving the "old" symbols to their copy...
     */

    // Update all parameter definitions:
    for (size_t i=0; i<external->numParameters(); i++) {
      Ast::ModelCopyist::updateParameter(external->getParameter(i), translation_table);
    }
    // Update all compartments:
    for (size_t i=0; i<external->numCompartments(); i++) {
      Ast::ModelCopyist::updateCompartment(external->getCompartment(i), translation_table);
    }
    // Update all species definition:
    for (size_t i=0; i<external->numSpecies(); i++) {
      Ast::ModelCopyist::updateSpecies(external->getSpecies(i), translation_table);
    }
    // Update all reactions:
    for (size_t i=0; i<external->numReactions(); i++) {
      Ast::ModelCopyist::updateReaction(external->getReaction(i), translation_table);
    }

    /*
     * Now, clean up internal model
     */

    // Remove exogeneous reactions from original model
    for(std::set<Ast::Reaction *>::iterator it = exR.begin(); it!=exR.end(); it++)
    {
      Ast::Reaction *reaction = ((*it));
      src->remDefinition(reaction);
      delete reaction; reaction=0;
    }

    // Remove external species from internal model but not their references
    for(std::set<Ast::Species *>::iterator it = exS.begin(); it!=exS.end(); it++)
    {
      Ast::Species *species = ((*it));
      src->remDefinition(species);
    }

    // Return empty lists
    exR.empty();

}


Ast::Model &
HybridModel::getExternalModel()
{
  return external;
}

const SignalOfInterest &
HybridModel::getSOI() const
{
  return soi;
}

Ast::Parameter *
HybridModel::SpeciestoParameter(Ast::Species *node, GiNaC::exmap &translation_table,
                                std::map<Ast::Species *, Ast::Species *> &species_table)
{
  // Get compartment associated to the copied species:
  //Ast::Compartment *comp = destination->getCompartment(node->getCompartment()->getIdentifier());

  // Copy species:
  Ast::Parameter *param = new Ast::Parameter(node->getIdentifier(),
                                             node->getValue().subs(translation_table),
                                             Ast::Unit::dimensionless(),true);

  // Store species in translation_table
  translation_table[node->getSymbol()] = param->getSymbol();
  //species_table[node] = param;

  // Done.
  return param;
}
