#ifndef __INA_HYBRIDMODEL_HH__
#define __INA_HYBRIDMODEL_HH__

#include "ast/ast.hh"
#include <ginac/ginac.h>

#include "models/basemodel.hh"
#include "models/constantstoichiometrymixin.hh"

#include "ast/modelcopyist.hh"


namespace iNA {
namespace Models {

/**
 * An atempt to hybrid simulation.
 *
 * @ingroup ssa
 */
class HybridModel
        : BaseModel, ConstantStoichiometryMixin

{

  /**
    Constructor
  **/

  HybridModel(Ast::Model &model, std::set<Ast::Species *>exS)
      : BaseModel(model), ConstantStoichiometryMixin((BaseModel &)(*this))
  {


        GiNaC::exmap translation_table;

        // Select external reaction set given the external species exS
        std::set<Ast::Reaction *> exR;
        selectReactions(exS,exR);

        // Distill external model
        Ast::Model *copy = new Ast::Model(model);
        distill(&model,copy,exR,translation_table);

        // Remove exogeneous reactions from original model
        for(std::set<Ast::Reaction *>::iterator it = exR.begin(); it!=exR.end(); it++) model.remDefinition((*it));

  }


  /**
    Select all reactions that change species along with those in list @param exS
  **/
  void selectReactions(std::set<Ast::Species *> &exS, std::set<Ast::Reaction *> &exR)
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


  /**
    Assembles the list of species changed in the reactions exR
  **/
  void assembleExternalSpeciesList(const std::set<Ast::Reaction *> &exR, std::set<Ast::Species *> & exS)
  {

    for(std::set<Ast::Reaction *>::iterator it=exR.begin();it!=exR.end();++it)
    {

      size_t idj=this->getReactionIdx((*it));
      for(size_t i=0; i<this->numSpecies(); i++)
      {
        if(this->stoichiometry(i,idj)!=0) exS.insert(this->getSpecies(i));
      }

    }

    // Check for consistency
    if(!checkConsistency(exR,exS)) throw("Model not consistent with an exogeneous model");

  }


  /**
   Check if external process is exogeneous.
   @todo Check for emptiness!
  */

  bool checkConsistency(const std::set<Ast::Reaction *> &exR, std::set<Ast::Species *> & exS)
  {


    for(size_t j=0; j<this->numReactions(); j++)
    {

      Ast::Reaction *reaction = this->getReaction(j);

      // Skip internal reactions
      if(exR.find(reaction)!=exR.end()) continue;
      else
      {
        for(size_t i=0; i<this->numSpecies(); i++)
        {
          Ast::Species *species = this->getSpecies(i);

          // Check if external reaction is exogeneous, i.e., kinetic laws depend only on external species
          if(exS.find(species)!=exS.end() && reaction->getKineticLaw()->getRateLaw().has(species->getSymbol()))
            return false;
        }
      }
    }

    return true;

  }




  /**
   Distills a model with list of species and reactions.
   TODO: avoid unnecessary parameters and things.
  */
  void distill(const Ast::Model * src, Ast::Model *exogeneous,
               const std::set<Ast::Reaction *> exR,
               GiNaC::exmap translation_table)
  {


      // Assemble list of external species
      std::set<Ast::Species *> exS;
      this->assembleExternalSpeciesList(exR,exS);


      // Table of species copies:
      std::map<Ast::Species *, Ast::Species *> species_table;

      // Set identifier of model:
      exogeneous->setIdentifier(src->getIdentifier());

      // Set name:
      exogeneous->setName(src->getName());

      // add time symbol to translation table:
      translation_table[src->getTime()] = exogeneous->getTime();

      // Sorry, iteration is quiet in-efficient, however:

      // Copy function definitions:
      for (Ast::Model::const_iterator iter = src->begin(); iter != src->end(); iter++) {
        if (Ast::Node::isFunctionDefinition(*iter)) {
          exogeneous->addDefinition(Ast::ModelCopyist::copyFunctionDefinition(
                                static_cast<Ast::FunctionDefinition *>(*iter), translation_table));
        }
      }

      // Copy default units:
      exogeneous->setSubstanceUnit(src->getSubstanceUnit().asScaledBaseUnit(), false);
      exogeneous->setVolumeUnit(src->getVolumeUnit().asScaledBaseUnit(), false);
      exogeneous->setAreaUnit(src->getAreaUnit().asScaledBaseUnit(), false);
      exogeneous->setLengthUnit(src->getLengthUnit().asScaledBaseUnit(), false);
      exogeneous->setTimeUnit(src->getTimeUnit().asScaledBaseUnit(), false);

      // Copy user defined "specialized" units:
      for (Ast::Model::const_iterator iter = src->begin(); iter != src->end(); iter++) {
        if (Ast::Node::isUnitDefinition(*iter)) {
          exogeneous->addDefinition(Ast::ModelCopyist::copyUnitDefinition(
                                static_cast<Ast::UnitDefinition *>(*iter)));
        }
      }

      // Copy all parameter definitions:
      for (size_t i=0; i<src->numParameters(); i++) {
        exogeneous->addDefinition(Ast::ModelCopyist::copyParameterDefinition(
                              static_cast<Ast::Parameter *>(src->getParameter(i)),
                              translation_table));
      }

      // Copy all compartments:
      for (size_t i=0; i<src->numCompartments(); i++) {
        exogeneous->addDefinition(Ast::ModelCopyist::copyCompartmentDefinition(
                              static_cast<Ast::Compartment *>(src->getCompartment(i)),
                              translation_table));
      }

      // Copy all species definition:
      for (std::set<Ast::Species *>::iterator it=exS.begin(); it!=exS.end(); it++) {
        exogeneous->addDefinition(Ast::ModelCopyist::copySpeciesDefinition(
                                  static_cast<Ast::Species *>((*it)),
                                  translation_table, species_table, exogeneous));
      }

      // Copy reactions:
      for (std::set<Ast::Reaction *>::iterator it=exR.begin(); it!=exR.end(); it++) {
        exogeneous->addDefinition(Ast::ModelCopyist::copyReaction(
                              static_cast<Ast::Reaction *>((*it)),
                              translation_table, species_table));
      }

      /*
       * Now, update all expressions by resolving the "old" symbols to their copy...
       */

      // Update all parameter definitions:
      for (size_t i=0; i<exogeneous->numParameters(); i++) {
        Ast::ModelCopyist::updateParameter(exogeneous->getParameter(i), translation_table);
      }
      // Update all compartments:
      for (size_t i=0; i<exogeneous->numCompartments(); i++) {
        Ast::ModelCopyist::updateCompartment(exogeneous->getCompartment(i), translation_table);
      }
      // Update all species definition:
      for (size_t i=0; i<exogeneous->numSpecies(); i++) {
        Ast::ModelCopyist::updateSpecies(exogeneous->getSpecies(i), translation_table);
      }
      // Update all reactions:
      for (size_t i=0; i<exogeneous->numReactions(); i++) {
        Ast::ModelCopyist::updateReaction(exogeneous->getReaction(i), translation_table);
      }

  }



};


}
}

#endif
