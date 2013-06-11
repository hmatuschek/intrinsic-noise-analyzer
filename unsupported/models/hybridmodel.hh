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
 * Builds an hybrid model.
 *
 * @ingroup unsupported
 */
class HybridModel
        : public Ast::Model,
          private ConstantStoichiometryMixin

{
protected:

  Ast::Model external;

public:
  /**
    Constructor.
    @param model Ast::Model to hybridize.
    @param specList Minimal list of external species.
  **/
  HybridModel(Ast::Model &model, std::set<std::string> specList);

  /**
   * Returns the external model
   */
  Model &getExternalModel();

protected:

  /**
    Selects all reactions that change species along with those in list @param exS
  **/
  void selectReactions(std::set<Ast::Species *> &exS, std::set<Ast::Reaction *> &exR);

  /**
    Assembles the list of species changed in the reactions exR
  **/
  void assembleExternalSpeciesList(const std::set<Ast::Reaction *> &exR, std::set<Ast::Species *> & exS);

  /**
   Check exogeneity of external process used in distll.
  */
  bool checkConsistency(const std::set<Ast::Reaction *> &exR, std::set<Ast::Species *> & exS);

  /**
   Distills a model with list of species and reactions. Returns empty exR.
   TODO: avoid unnecessary parameters and things.
  */
  void distill(Ast::Model * src, Ast::Model *external, std::set<Ast::Reaction *> exR, GiNaC::exmap &translation_table);

  /**
   Returns species definition as parameter.
  */
  Ast::Parameter * SpeciestoParameter(Ast::Species *node, GiNaC::exmap &translation_table,
                                  std::map<Ast::Species *, Ast::Species *> &species_table);

};


}
}

#endif
