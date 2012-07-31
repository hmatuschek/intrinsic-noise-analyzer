#ifndef __INA_PARSER_SBMLSH_EXPORTER_HH__
#define __INA_PARSER_SBMLSH_EXPORTER_HH__

#include <ostream>
#include <ast/model.hh>
#include <ast/reaction.hh>


namespace Fluc {
namespace Parser {
namespace Sbmlsh {

/**
 * Internal used class to serialize an @c Ast::Model instance into SBML-SH.
 */
class Writer {
public:
  static void processModel(Ast::Model &model, std::ostream &output);

protected:
  static void processModelHeader(Ast::Model &model, std::ostream &ouput);
  static std::string getBaseUnitIdentifier(Ast::ScaledBaseUnit::BaseUnit);
  static void processUnitDefinitions(Ast::Model &model, std::ostream &output);
  static void processUnitDefinition(Ast::UnitDefinition *unit, std::ostream &output);
  static void processScaledUnit(const Ast::ScaledBaseUnit &unit, std::ostream &output);
  static void processCompartments(Ast::Model &model, std::ostream &output);
  static void processCompartment(Ast::Compartment *comp, std::ostream &output);
  static void processSpeciesList(Ast::Model &model, std::ostream &output);
  static void processSpecies(Ast::Species *species, std::ostream &output);
  static void processParameterList(Ast::Model &model, std::ostream &output);
  static void processParameter(Ast::Parameter *param, std::ostream &output);
  static void processRuleList(Ast::Model &model, std::ostream &output);
  static void processRule(Ast::VariableDefinition *var, std::ostream &output);
  static void processReactionList(Ast::Model &model, std::ostream &output);
  static void processReaction(Ast::Reaction *reac, std::ostream &output);
  static void processKineticLaw(Ast::KineticLaw *law, std::ostream &output);
  static void processEventList(Ast::Model &model, std::ostream &output);
};


}
}
}

#endif // __INA_PARSER_SBMLSH_EXPORTER_HH__
