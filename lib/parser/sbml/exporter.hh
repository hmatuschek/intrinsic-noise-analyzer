#ifndef __INA_PARSER_SBML_WRITER_HH__
#define __INA_PARSER_SBML_WRITER_HH__

#include <ast/ast.hh>
#include <sbml/SBMLTypes.h>


namespace Fluc {
namespace Parser {
namespace Sbml {

/**
 * Internal class used to serialize an @c Ast::Model instance into SBML.
 */
class Writer {
public:
  static libsbml::Model *processModel(Ast::Model &model, libsbml::SBMLDocument *sbml_doc);

protected:
  static void processUnitDefinitions(Ast::Model &model, libsbml::Model *sbml_model);
  static void processUnitDefinition(const Ast::Unit &unit, libsbml::UnitDefinition *sbml_unit_def);
  static void processUnit(const Ast::ScaledBaseUnit &unit, libsbml::UnitDefinition *sbml_unit_def);

  static void processFunctionDefinitions(Ast::Model &model, libsbml::Model *sbml_model);
  static void processFunctionDefinition(Ast::FunctionDefinition *func, libsbml::FunctionDefinition *sbml_func);

  static void processParameters(Ast::Model &model, libsbml::Model *sbml_model);
  static void processParameter(Ast::Parameter *param, libsbml::Parameter *sbml_param);

  static void processInitialValue(Ast::VariableDefinition *var, libsbml::Model *sbml_model);

  static void processRule(Ast::VariableDefinition *var, libsbml::Model *model);

  static libsbml::ASTNode *processExpression(GiNaC::ex);
};


}
}
}

#endif
