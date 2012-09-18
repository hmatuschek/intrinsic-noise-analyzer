#ifndef __FLUC_AST_MODELCOPYIST_HH__
#define __FLUC_AST_MODELCOPYIST_HH__

#include "model.hh"
#include "reaction.hh"


namespace iNA {
namespace Ast {


/**
 * This class is just a collection of static methods to ease the copying of @c Ast::Model
 * instances. The copying is performed in two stages, in the first stage, a invalid copy of the
 * original model is created, that still refers to the symbols of the original one. In a second
 * stage, these symbols are resolved to these of the copied model.
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
  /** Helper function to copy function definitions. */
  static Ast::FunctionDefinition *copyFunctionDefinition(Ast::FunctionDefinition *node,
                                                         GiNaC::exmap &translation_table);

  /** Helper function to copy unit-definitions. */
  static Ast::UnitDefinition *copyUnitDefinition(Ast::UnitDefinition *node);

  /** Helper function to copy parameter definitions. */
  static Ast::Parameter *copyParameterDefinition(
    Ast::Parameter *node, GiNaC::exmap &translation_table);

  /** This function updates the symbols of the initial value of the paramter definition using
   * the given translation table. */
  static void updateParamter(Ast::Parameter *node, GiNaC::exmap &translation_table);

  /** Helper function to copy compartment definitions. */
  static Ast::Compartment *copyCompartmentDefinition(Ast::Compartment *node,
                                                     GiNaC::exmap &translation_table);
  /** This function updates the value of the compartment definition. */
  static void updateCompartment(Ast::Compartment *node, GiNaC::exmap &translation_table);

  /** Helper function to copy species definitions. */
  static Ast::Species *copySpeciesDefinition(
    Ast::Species *node, GiNaC::exmap &translation_table,
    std::map<Species *, Species *> &species_table, Model *destination);

  /** This function updates the initial value of the species. */
  static void updateSpecies(Ast::Species *node, GiNaC::exmap &translation_table);

  /** Helper function to copy constraints. */
  static Ast::Constraint *copyConstraint(Ast::Constraint *node, GiNaC::exmap &translation_table);

  /** This function updates a constraint (dispatcher). */
  static void updateConstraint(Ast::Constraint *node, GiNaC::exmap &translation_table);

  /** Helper function to copy algebraic constraints. */
  static Ast::AlgebraicConstraint *copyAlgebraicConstraint(
    Ast::AlgebraicConstraint *node, GiNaC::exmap &translation_table);

  /** This function updates an algebraic constraint. */
  static void updateAlgebraicConstraint(
    Ast::AlgebraicConstraint *node, GiNaC::exmap &translation_table);

  /** Helper function to copy reactions. */
  static Ast::Reaction *copyReaction(Ast::Reaction *node, GiNaC::exmap &translation_table,
                                     std::map<Species *, Species *> &species_table);

  /** This function updates a reaction (stoichiometry and kinetic law) */
  static void updateReaction(Ast::Reaction *node, GiNaC::exmap &translation_table);

  /** Helper function to copy kinetic laws. */
  static Ast::KineticLaw *copyKineticLaw(Ast::KineticLaw *node, GiNaC::exmap &translation_table);

  /** This function updates a kinetic law. */
  static void updateKineticLaw(Ast::KineticLaw *node, GiNaC::exmap &translation_table);

  /** Helper function to copy rules. */
  static Ast::Rule *copyRule(Ast::Rule *node, GiNaC::exmap &translation_table);

  /** This function updates a rule definition. */
  static void updateRule(Ast::Rule *node, GiNaC::exmap &translation_table);

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

#endif // MODELCOPYIST_HH
