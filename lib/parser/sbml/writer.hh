#ifndef __INA_PARSER_SBML_WRITER_HH__
#define __INA_PARSER_SBML_WRITER_HH__

#include "../../ast/ast.hh"
#include <sbml/SBMLTypes.h>


namespace iNA {
namespace Parser {
namespace Sbml {

/**
 * Internal class used to serialize an @c Ast::Model instance into SBML.
 */
class Writer {
public:
  /** Assembles a libsbml model from the given Ast::Model. */
  static LIBSBML_CPP_NAMESPACE_QUALIFIER Model *processModel(
    Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER SBMLDocument *sbml_doc);

protected:
  /** Translates the list unit definitions. */
  static void processUnitDefinitions(Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model);
  /** Translates a single unit definition. */
  static void processUnitDefinition(const Ast::Unit &unit, LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *sbml_unit_def);
  /** Translates a ScaledBaseUnit. */
  static void processUnit(const Ast::ScaledBaseUnit &unit, LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *sbml_unit_def);
  /** Translates the list of function definitions. */
  static void processFunctionDefinitions(Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model);
  /** Translates a single function definition. */
  static void processFunctionDefinition(
    Ast::FunctionDefinition *func, LIBSBML_CPP_NAMESPACE_QUALIFIER FunctionDefinition *sbml_func, Ast::Model &model);
  /** Translates the list of parameters. */
  static void processParameters(Ast::Model &model, std::map<Ast::Unit, std::string> &units,
                                LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model);
  /** Translates a single parameter definition. */
  static void processParameter(Ast::Parameter *param, std::map<Ast::Unit, std::string> &units,
                               LIBSBML_CPP_NAMESPACE_QUALIFIER Parameter *sbml_param,
                               LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model);
  /** Translates the list of compartment definitions. */
  static void processCompartments(Ast::Model &mode, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model);
  /** Translates a single compartment definition. */
  static void processCompartment(Ast::Compartment *comp, LIBSBML_CPP_NAMESPACE_QUALIFIER Compartment *sbml_comp);
  /** Translates the list of species definitions. */
  static void processSpeciesList(Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model);
  /** Translates a single species definition. */
  static void processSpecies(Ast::Species *species, LIBSBML_CPP_NAMESPACE_QUALIFIER Species *sbml_species);
  /** Translates the list of reactions. */
  static void processReactions(Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model,
                               std::map<Ast::Unit, std::string> &units);
  /** Translates a single reaction. */
  static void processReaction(Ast::Reaction *reac, LIBSBML_CPP_NAMESPACE_QUALIFIER Reaction *sbml_reac,
                              Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model,
                              std::map<Ast::Unit, std::string> &units);
  /** Translates a kinetic law. */
  static void processKineticLaw(Ast::KineticLaw *law, LIBSBML_CPP_NAMESPACE_QUALIFIER KineticLaw *sbml_law,
                                LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model,
                                std::map<Ast::Unit, std::string> &units, Ast::Model &model);
  /** Creates a initial value definition for a variable. */
  static void processInitialValue(Ast::VariableDefinition *var, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model, Ast::Model &model);
  /** Creates a rule for a variable. */
  static void processRule(Ast::VariableDefinition *var, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model, Ast::Model &model);
  /** Helper function to test if a variable has its default unit. */
  static bool hasDefaultUnit(Ast::Parameter *var);
  /** Returns the identifier of the unit definition of a variable. */
  static std::string getUnitIdentifier(Ast::Parameter *var, std::map<Ast::Unit, std::string> units,
                                       LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model);
  /** Translates a GiNaC expression. */
  static LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *processExpression(GiNaC::ex, Ast::Model &model, Ast::Scope &scope);
};


/** Internal used class to assemble ASTNode instances representing a GiNaC expression. */
class SBMLExpressionAssembler :
    public GiNaC::visitor, public GiNaC::numeric::visitor, public GiNaC::add::visitor,
    public GiNaC::mul::visitor, public GiNaC::symbol::visitor, public GiNaC::power::visitor,
    public GiNaC::function::visitor, public GiNaC::basic::visitor
{
protected:
  /** Hidden constructor, use factory method below. */
  SBMLExpressionAssembler(Ast::Model &model, Ast::Scope &scope);

  /** (Re-) Sets the current variable scope of the writer. All symboles are resolved using this
   * scope. */
  void setScope(Ast::Scope &scope);

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
  static LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *process(GiNaC::ex expression, Ast::Model &model, Ast::Scope &scope);

protected:
  /** A stack of sub-expressions. */
  std::list<LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *> _stack;
  /** The Ast::Model, used to identify special symbols. */
  Ast::Model &_model;
  /** Holds a weak reference to the current scope used to resolve variables. */
  Ast::Scope &_scope;
};


}
}
}

#endif
