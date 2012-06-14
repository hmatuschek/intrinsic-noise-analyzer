#ifndef __FLUC_AST_TRAFO_MODELCOPYIST_HH__
#define __FLUC_AST_TRAFO_MODELCOPYIST_HH__

#include "ast/model.hh"
#include "ast/reaction.hh"


namespace Fluc {
namespace Ast {
namespace Trafo {


/**
 * This class is just a collection of static methods to ease the copying of @c Ast::Model
 * instances.
 *
 * @ingroup ast
 */
class ModelCopyist
{
public:
  /**
   * Simply copies the model @c src and stores the copy in @c dest.
   */
  static void copy(const Ast::Model *src, Ast::Model *dest);

  /**
   * Copies the model @c src into @c dest but populates the given translation table
   * mapping "old" symbols to their copies.
   */
  static void copy(const Ast::Model *src, Ast::Model *dest, GiNaC::exmap &translation_table);

  /**
   * Function to copy a single reaction of a model, that refers to the original species and global
   * parameters etc, while owning a fresh copy of the kinetic law (with its own local parameters).
   */
  static Ast::Reaction *dupReaction(Reaction *node);

  /**
   * Function to copy a single reaction of a model, that refers to the original species and global
   * parameters etc, while owning a fresh copy of the kinetic law (with its own local parameters).
   *
   * The kinetic law of the reaction may have locally defined parameters. The translation between
   * "old local parameters" to the new (copied) parameters is retunred in param_table.
   */
  static Ast::Reaction *dupReaction(Reaction *node, GiNaC::exmap &param_table);

  /**
   * Function to copy a single kinetic law instance of a reaction of a model, that refers to the
   * original species and global parameters etc, while owning a its own local parameters.
   */
  static Ast::KineticLaw *dupKineticLaw(Ast::KineticLaw *law, GiNaC::exmap &param_table);


protected:
  /**
   * Helper function to copy function definitions.
   */
  static Ast::FunctionDefinition *copyFunctionDefinition(Ast::FunctionDefinition *node,
                                                         GiNaC::exmap &translation_table);

  /**
   * Helper function to copy unit-definitions.
   */
  static Ast::UnitDefinition *copyUnitDefinition(Ast::UnitDefinition *node);

  /**
   * Helper function to copy parameter definitions.
   */
  static Ast::Parameter *copyParameterDefinition(Ast::Parameter *node,
                                                 GiNaC::exmap &translation_table);

  /**
   * Helper function to copy compartment definitions.
   */
  static Ast::Compartment *copyCompartmentDefinition(Ast::Compartment *node,
                                                     GiNaC::exmap &translation_table);

  /**
   * Helper function to copy species definitions.
   */
  static Ast::Species *copySpeciesDefinition(Ast::Species *node, GiNaC::exmap &translation_table,
                                             std::map<Species *, Species *> &species_table,
                                             Model *destination);

  /**
   * Helper function to copy constraints.
   */
  static Ast::Constraint *copyConstraint(Ast::Constraint *node, GiNaC::exmap &translation_table);

  /**
   * Helper function to copy algebraic constraints.
   */
  static Ast::AlgebraicConstraint *copyAlgebraicConstraint(Ast::AlgebraicConstraint *node,
                                                           GiNaC::exmap &translation_table);

  /**
   * Helper function to copy reactions.
   */
  static Ast::Reaction *copyReaction(Ast::Reaction *node, GiNaC::exmap &translation_table,
                                     std::map<Species *, Species *> &species_table);

  /**
   * Helper function to copy kinetic laws.
   */
  static Ast::KineticLaw *copyKineticLaw(Ast::KineticLaw *node, GiNaC::exmap &translation_table);

  /**
   * Helper function to copy rules.
   */
  static Ast::Rule *copyRule(Ast::Rule *node, GiNaC::exmap &translation_table);

  /**
   * Helper function to copy assignment rules.
   */
  static Ast::AssignmentRule *copyAssignmentRule(Ast::AssignmentRule *node,
                                                 GiNaC::exmap &translation_table);

  /**
   * Helper function to copy rate-rules.
   */
  static Ast::RateRule *copyRateRule(Ast::RateRule *node, GiNaC::exmap &translation_table);
};


}
}
}

#endif // MODELCOPYIST_HH
