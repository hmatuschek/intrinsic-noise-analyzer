#ifndef __INA_PARSER_SBMLSH_WRITER_HH__
#define __INA_PARSER_SBMLSH_WRITER_HH__

#include <ostream>
#include <ast/model.hh>
#include <ast/reaction.hh>


namespace iNA {
namespace Parser {
namespace Sbmlsh {

/**
 * Internal used class to serialize an @c Ast::Model instance into SBML-SH.
 */
class Writer {
public:
  /** Serialises the given model into the given stream. */
  static void processModel(Ast::Model &model, std::ostream &output);

protected:
  /** Assembles the model header. */
  static void processModelHeader(Ast::Model &model, std::ostream &ouput);
  /** Translates the base unit identifier. */
  static std::string getBaseUnitIdentifier(Ast::ScaledBaseUnit::BaseUnit);
  /** Serializes the list of unit definitions. */
  static void processUnitDefinitions(Ast::Model &model, std::ostream &output);
  /** Serializes a unit definition. */
  static void processUnitDefinition(Ast::UnitDefinition *unit, std::ostream &output);
  /** Serializes a single scaled base unit. */
  static void processScaledUnit(const Ast::ScaledBaseUnit &unit, std::ostream &output);
  /** Serializes the list of compartments. */
  static void processCompartments(Ast::Model &model, std::ostream &output);
  /** Serializes a single compartement. */
  static void processCompartment(Ast::Compartment *comp, const Ast::Model &model, std::ostream &output);
  /** Serializes the list of species definitions. */
  static void processSpeciesList(Ast::Model &model, std::ostream &output);
  /** Serializes a single species definition. */
  static void processSpecies(Ast::Species *species, const Ast::Model &model, std::ostream &output);
  /** Serializes the list of parameter definitions. */
  static void processParameterList(Ast::Model &model, std::ostream &output);
  /** Serializes a single parameter definition. */
  static void processParameter(Ast::Parameter *param, const Ast::Model &model, std::ostream &output);
  /** Serializes the list of rules. */
  static void processRuleList(Ast::Model &model, std::ostream &output);
  /** Serializes a single rule. */
  static void processRule(Ast::VariableDefinition *var, const Ast::Model &model, std::ostream &output);
  /** Serializes the list of reactions. */
  static void processReactionList(Ast::Model &model, std::ostream &output);
  /** Serializes a single reaction. */
  static void processReaction(Ast::Reaction *reac, std::ostream &output);
  /** Serializes the kinetic law of a reaction. */
  static void processKineticLaw(Ast::KineticLaw *law, std::ostream &output);
  /** Serializes the list of events. */
  static void processEventList(Ast::Model &model, std::ostream &output);
};


}
}
}

#endif // __INA_PARSER_SBMLSH_WRITER_HH__
