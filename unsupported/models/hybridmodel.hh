#ifndef __INA_HYBRIDMODEL_HH__
#define __INA_HYBRIDMODEL_HH__

#include "../ast/ast.hh"
#include <ginac/ginac.h>

#include "basemodel.hh"
#include "constantstoichiometrymixin.hh"

#include "modelcopyist.hh"


namespace iNA {
namespace Models {

/**
 * Base model for all stochastic simulators.
 *
 * @ingroup ssa
 */
class HybridModel
        : BaseModel

{

  /**

  **/

  HybridModel(const Ast::Model &model, Ast::Species *SpeciesOfI)
      : BaseModel(model), ConstantStoichiometryMixin((BaseModel &)(*this))
  {

        GiNaC::exmap translation_table;

        std::vector<Ast::Species *> exS;
        std::vector<Ast::Reaction *> exR;
        exS.push_back(SpeciesOfI);

        int idx = model.getSpeciesIdx(SpeciesOfI);

        // Collect all species which are changed with idx
        for(size_t k=0;k<this->numSpecies();k++) {

            if(idx==k) continue;

            bool d =0;
            for (size_t i=0;i<this->numReactions();i++)
            {
                // The following is true if species k is changed along with idx in reaction i
                bool g = (this->stoichiometry(k,i)!=0 && this->stoichiometry(idx,i)!=0 );
                d = (d || g);

                if(g) exR.push_back(model->getReaction(i));
            }

            // Following is true if species k is changed along with idx in any reaction
            if (d==0) exS.push_back(model->getSpecies(k));

        }

        // Distill exogeneous model
        Ast::Model *copy = new Ast::Model(model);
        distill(model,copy,exS,exR,translation_table);

        // Remove exogeneous reactions from original model
        for(size_t i =0; i<exR.size(); i++) model.remDefinition(exR(i));

  }

};


// Distills a model with list of species and reactions

void distill(Ast::Model * src, Ast::Model * exogeneous,
             const std::vector<Ast::Species *> exS, const std::vector<Ast::Reaction *> exR,
             GiNaC::exmap translation_table)
{

    // Table of species copies:
    std::map<Species *, Species *> species_table;

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
        exogeneous->addDefinition(ModelCopyist::copyFunctionDefinition(
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
        exogeneous->addDefinition(ModelCopyist::copyUnitDefinition(
                              static_cast<Ast::UnitDefinition *>(*iter)));
      }
    }

    // Copy all parameter definitions:
    for (size_t i=0; i<src->numParameters(); i++) {
      exogeneous->addDefinition(ModelCopyist::copyParameterDefinition(
                            static_cast<Ast::Parameter *>(src->getParameter(i)),
                            translation_table));
    }

    // Copy all compartments:
    for (size_t i=0; i<src->numCompartments(); i++) {
      exogeneous->addDefinition(ModelCopyist::copyCompartmentDefinition(
                            static_cast<Ast::Compartment *>(src->getCompartment(i)),
                            translation_table));
    }

    // Copy all species definition:
    for (size_t i=0; i<exS.size(); i++) {
      exogeneous->addDefinition(ModelCopyist::copySpeciesDefinition(
                            static_cast<Ast::Species *>(exS[i]),
                            translation_table, species_table, exogeneous));
    }

    // Copy reactions:
    for (size_t i=0; i<exR.size(); i++) {
      dest->addDefinition(ModelCopyist::copyReaction(
                            static_cast<Ast::Reaction *>(exR(i)),
                            translation_table, species_table));
    }

    /*
     * Now, update all expressions by resolving the "old" symbols to their copy...
     */

    // Update all parameter definitions:
    for (size_t i=0; i<exogeneous->numParameters(); i++) {
      ModelCopyist::updateParameter(exogeneous->getParameter(i), translation_table);
    }
    // Update all compartments:
    for (size_t i=0; i<exogeneous->numCompartments(); i++) {
      ModelCopyist::updateCompartment(exogeneous->getCompartment(i), translation_table);
    }
    // Update all species definition:
    for (size_t i=0; i<exogeneous->numSpecies(); i++) {
      ModelCopyist::updateSpecies(exogeneous->getSpecies(i), translation_table);
    }
    // Update all reactions:
    for (size_t i=0; i<exogeneous->numReactions(); i++) {
      ModelCopyist::updateReaction(exogeneous->getReaction(i), translation_table);
    }

}

}
}

#endif
