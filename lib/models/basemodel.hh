#ifndef __FLUC_MODELS_BASEMODEL_HH__
#define __FLUC_MODELS_BASEMODEL_HH__


#include <sbml/SBMLTypes.h>
#include "ast/ast.hh"
#include "convert2irreversiblemixin.hh"

namespace Fluc {
namespace Models {


/**
 * Absolute basic model.
 *
 * Assembles a reference of a SMBL model to a Ast::Module instance.
 *
 * @todo Currently all mixin classes are defined as friend of this class, due to the fact, that
 *       BaseModel is now derived from @c Ast::Model, the may not be necessary anymore.
 *       @c Ast::Model provides add/get/set methods for all its members.
 *
 * @ingroup models
 */
class BaseModel
        : public Ast::Model,
          public Convert2IrreversibleMixin
{
protected:
  /**
   * Maps the index back to a species-symbol.
   *
   * @deprecated Use the @c getSpecies(size_t)->getSymbol() combination.
   */
  std::vector<GiNaC::symbol> species;

  /**
   * Maps an index to a @c Ast::Reaction.
   *
   * @deprecated Use the getReaction(size_t) method instead.
   */
  std::vector<Ast::Reaction *> reactions;

  /**
   * Holds for each reactions the propensity.
   */
  GiNaC::exvector propensities;

  /**
   * Holds the table: constant -> value and assignment rule substitutions.
   */
  GiNaC::exmap constant_substitution_table;


public:
  /**
   * Constructor.
   *
   * Assembles the @c Ast::Module and performs constant folding and assignement-rule inlineing.
   * It also constructs the species <-> index tables.
   */
  BaseModel(libsbml::Model *sbml_model);

  /**
   * Constructor.
   *
   * Constructs a @c Model::BaseModel as copy of the given @c Ast::Model.
   */
  explicit BaseModel(const Ast::Model &model);

  /**
   * Base virtual destructor. Don't mind.
   */
  virtual ~BaseModel();

  /**
   * The ConstantStoichiometryMixin etc may access the base-model interna.
   */
  friend class ConstantStoichiometryMixin;

  /**
   * The ConstCompartmentMixin may access the base-model interna.
   */
  friend class ConstCompartmentMixin;

  /**
   * The BaseUnitMixin may access the base-model interna.
   */
  friend class BaseUnitMixin;

  /**
   * The ExtensiveSpeciesMixin may access the base-model interna.
   */
  friend class ExtensiveSpeciesMixin;

  /**
   * The IntensiveSpeciesMixin may access the base-model interna.
   */
  friend class IntensiveSpeciesMixin;

  /**
   * The ParticleNumbersMixin may access the base-model interna.
   */
  friend class ParticleNumbersMixin;

  /**
   * Also propensityExansion may access the interna of base-model.
   */
  friend class propensityExpansion;

  friend class AssertNoExplicitTimeDependenceMixin;


protected:
  /**
   * Performs all constant and assignment rule substitutions.
   */
  GiNaC::ex foldConstants(GiNaC::ex expression);
};


}
}

#endif
