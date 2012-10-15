#ifndef __INA_SBMLSH_ASSEMBLER_HH__
#define __INA_SBMLSH_ASSEMBLER_HH__

#include <typeinfo>

#include <ast/unitdefinition.hh>
#include <parser/lexer.hh>
#include <parser/production.hh>
#include <parser/expr/assembler.hh>



namespace iNA {

namespace Ast {
// Forward declarations...
class Model;
class Scope;
class Reaction;
class KineticLaw;
class Species;
class VariableDefinition;
}


namespace Parser {
namespace Sbmlsh {

/** This class takes the concrete syntax tree (CST) of some parsed SBML-SH code and assembles a
 * @c Ast::Model instance from it. */
class Assembler : protected Expr::ScopeContext, public Expr::Assembler
{
protected:
  /** Holds a weak reference to the model being assembled. */
  Ast::Model &_model;

  /** Maps the name of a base unit to the Ast::ScaledBaseUnit::BaseUnit enum. */
  std::map<std::string, Ast::ScaledBaseUnit::BaseUnit> _base_unit_map;


public:
  /** Constructor. */
  Assembler(Ast::Model &model, iNA::Parser::Lexer &lexer);

  /** Entry point. */
  void process(iNA::Parser::ConcreteSyntaxTree &root);

protected:
  /** Dispatcher, assembles the @c Ast::Model instance. */
  void processModel(iNA::Parser::ConcreteSyntaxTree &model);
  /** Handles model header. */
  void processModelDefinition(iNA::Parser::ConcreteSyntaxTree &model);
  /** Handles default unit definitions. */
  void processDefaultUnitDefinitions(iNA::Parser::ConcreteSyntaxTree &def_units);
  /** Handles a unit definition. */
  void processUnitDefinition(iNA::Parser::ConcreteSyntaxTree &unit);
  /** Handles a list of scaled base unit. */
  void processScaledUnitList(iNA::Parser::ConcreteSyntaxTree &unit,
                             std::list<Ast::ScaledBaseUnit> &unit_list);
  /** Handles a list of modifiers of base units. */
  void processScaledUnitModifierList(iNA::Parser::ConcreteSyntaxTree &sulist,
                                     double &multiplier, int &scale, int &exponent);
  /** Handles a list of compartment definitions. */
  void processCompartmentDefinitions(iNA::Parser::ConcreteSyntaxTree &comp);
  /** Handles a list species definition. */
  void processSpeciesDefinition(iNA::Parser::ConcreteSyntaxTree &spec);
  /** Handles a list of species modifiers. */
  void processSpeciesModifierList(iNA::Parser::ConcreteSyntaxTree &spec_mod, bool &has_substance_units,
                                  bool &has_boundary_condition, bool &is_constant);
  /** Handles a list of parameter definitions. */
  void processParameterDefinition(iNA::Parser::ConcreteSyntaxTree &param);
  /** Handles a list of rule definitions. */
  void processRuleDefinitionList(iNA::Parser::ConcreteSyntaxTree &rules);
  /** Handles a list of reaction definitions. */
  void processReactionDefinitions(iNA::Parser::ConcreteSyntaxTree &reac);
  /** Handles the list of reaction modifiers. */
  void processReactionModifierList(iNA::Parser::ConcreteSyntaxTree &lst, std::list<Ast::Species *> &mods);
  /** Handels the kinetic law of a reaction. */
  Ast::KineticLaw *processKineticLaw(iNA::Parser::ConcreteSyntaxTree &law);
  /** Handles the list of local parameters of a kinetic law. */
  void processLocalParameters(iNA::Parser::ConcreteSyntaxTree &params);
  /** Handles a reaction equation. */
  void processReactionEquation(iNA::Parser::ConcreteSyntaxTree &law, Ast::Reaction *reaction);
  /** Handles the reactants of a reaction. */
  void processReactants(iNA::Parser::ConcreteSyntaxTree &sum, Ast::Reaction *reaction);
  /** Handles the products of a reaction. */
  void processProducts(iNA::Parser::ConcreteSyntaxTree &sum, Ast::Reaction *reaction);
  /** Removes the quotes. */
  void unquote(std::string &name);
};


}
}
}

#endif // __INA_SBMLSH_ASSEMBLER_HH__
