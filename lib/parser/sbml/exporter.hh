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
  static void processFunctionDefinition(
    Ast::FunctionDefinition *func, libsbml::FunctionDefinition *sbml_func, Ast::Model &model);

  static void processParameters(Ast::Model &model, libsbml::Model *sbml_model);
  static void processParameter(Ast::Parameter *param, libsbml::Parameter *sbml_param);

  static void processCompartments(Ast::Model &mode, libsbml::Model *sbml_model);
  static void processCompartment(Ast::Compartment *comp, libsbml::Compartment *sbml_comp);

  static void processSpeciesList(Ast::Model &model, libsbml::Model *sbml_model);
  static void processSpecies(Ast::Species *species, libsbml::Species *sbml_species);

  static void processReactions(Ast::Model &model, libsbml::Model *sbml_model);
  static void processReaction(Ast::Reaction *reac, libsbml::Reaction *sbml_reac, Ast::Model &model);
  static void processKineticLaw(Ast::KineticLaw *law, libsbml::KineticLaw *sbml_law, Ast::Model &model);

  static void processInitialValue(Ast::VariableDefinition *var, libsbml::Model *sbml_model, Ast::Model &model);

  static void processRule(Ast::VariableDefinition *var, libsbml::Model *sbml_model, Ast::Model &model);

  static bool hasDefaultUnit(Ast::VariableDefinition *var, Ast::Model &model);
  static std::string getUnitIdentifier(Ast::VariableDefinition *var, Ast::Model &model);

  static libsbml::ASTNode *processExpression(GiNaC::ex, Ast::Model &model);
};


/** Internal used class to assemble libsbml::ASTNode instances representing a GiNaC expression. */
class SBMLExpressionAssembler :
    public GiNaC::visitor, public GiNaC::numeric::visitor, public GiNaC::add::visitor,
    public GiNaC::mul::visitor, public GiNaC::symbol::visitor, public GiNaC::power::visitor,
    public GiNaC::function::visitor, public GiNaC::basic::visitor
{
protected:
  /** A stack of sub-expressions. */
  std::list<libsbml::ASTNode *> _stack;
  /** The Ast::Model, used to identify special symbols. */
  Ast::Model &_model;

protected:
  /** Hidden constructor, use factory method below. */
  SBMLExpressionAssembler(Ast::Model &model);

public:
  /** Handles a constant numeric value. */
  void visit(const GiNaC::numeric &value);
  /** Handles a symbol. */
  void visit(const GiNaC::symbol &symbol);
  /** Handles sums of expressions. */
  void visit(const GiNaC::add &sum);
  /** Handles products of expressions. */
  void visit(const GiNaC::mul &prod);
  /** Handles a power expression x^y. */
  void visit(const GiNaC::power &pow);
  /** Handles function calls. */
  void visit(const GiNaC::function &function);
  /** Throws an exception. */
  void visit(const GiNaC::basic &basic);

public:
  /** Translates a GiNaC expression into an SBML expression. */
  static libsbml::ASTNode *process(GiNaC::ex expression, Ast::Model &model);
};


}
}
}

#endif
