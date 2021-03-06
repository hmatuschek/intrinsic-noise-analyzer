#ifndef __INA_SBMLSH_ASSEMBLER_HH__
#define __INA_SBMLSH_ASSEMBLER_HH__

#include <typeinfo>

#include "../../ast/unitdefinition.hh"
#include "../../trafo/variablescaling.hh"
#include "../lexer.hh"
#include "../production.hh"
#include "../expr/assembler.hh"



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

/** This class takes the concrete syntax tree (CST) of some parsed SBML-sh code and assembles a
 * @c Ast::Model instance from it. */
class Assembler
{
public:
  /** Constructor. */
  Assembler(Ast::Model &model, iNA::Parser::Lexer &lexer);

  /** Entry point. */
  void process(iNA::Parser::ConcreteSyntaxTree &root);

protected:
  /** Dispatcher, assembles the @c Ast::Model instance. */
  void processModel(iNA::Parser::ConcreteSyntaxTree &model);
  /** Links the model, parses all expressions of initial values. */
  void linkModel(ConcreteSyntaxTree &model);
  /** Handles model header. */
  void processModelDefinition(iNA::Parser::ConcreteSyntaxTree &model);
  /** Handles default unit definitions. */
  void processDefaultUnitDefinitions(iNA::Parser::ConcreteSyntaxTree &def_units);
  /** Handles a unit definition. */
  void processUnitDefinition(iNA::Parser::ConcreteSyntaxTree &unit);
  /** Handles a list of scaled base unit. */
  void processScaledUnitList(iNA::Parser::ConcreteSyntaxTree &unit_prod, Ast::Unit &unit);
  /** Handles a list of modifiers of base units. */
  void processScaledUnitModifierList(iNA::Parser::ConcreteSyntaxTree &sulist,
                                     double &multiplier, int &scale, int &exponent);
  /** Handles a list of compartment definitions. */
  void processCompartmentDefinitions(iNA::Parser::ConcreteSyntaxTree &comp);
  /** Parses the initial values of compartments. */
  void linkCompartmentDefinitions(iNA::Parser::ConcreteSyntaxTree &comp);
  /** Handles a list species definition. */
  void processSpeciesDefinition(iNA::Parser::ConcreteSyntaxTree &spec,
                                bool &speciesHasSubstanceUnits);
  /** Parses the initial value of species definitions. */
  void linkSpeciesDefinition(iNA::Parser::ConcreteSyntaxTree &spec, Trafo::VariableScaling &scaleing);
  /** Handles a list of species modifiers. */
  void processSpeciesModifierList(iNA::Parser::ConcreteSyntaxTree &spec_mod, bool &has_substance_units,
                                  bool &has_boundary_condition, bool &is_constant);
  /** Handles a list of parameter definitions. */
  void processParameterDefinition(iNA::Parser::ConcreteSyntaxTree &param);
  /** Parses the initial value of parameter definitions. */
  void linkParameterDefinition(iNA::Parser::ConcreteSyntaxTree &param);
  /** Handles a list of rule definitions. */
  void processRuleDefinitionList(iNA::Parser::ConcreteSyntaxTree &rules);
  /** Handles a list of reaction definitions. */
  void processReactionDefinitions(iNA::Parser::ConcreteSyntaxTree &reac);
  /** Links the initial values of all reactions. */
  void linkReactionDefinitions(iNA::Parser::ConcreteSyntaxTree &reac);
  /** Handles the list of reaction modifiers. */
  void processReactionModifierList(iNA::Parser::ConcreteSyntaxTree &lst, std::list<Ast::Species *> &mods);
  /** Handels the kinetic law of a reaction. */
  Ast::KineticLaw *processKineticLaw(iNA::Parser::ConcreteSyntaxTree &law);
  /** Parses the initial values of all parameters defined in the kinetic law. */
  void linkKineticLaw(Ast::KineticLaw *kinetic_law, iNA::Parser::ConcreteSyntaxTree &law);
  /** Handles the list of local parameters of a kinetic law. */
  void processLocalParameters(Ast::KineticLaw *scope, iNA::Parser::ConcreteSyntaxTree &params);
  /** Parses the initial value of the local paramters. */
  void linkLocalParameters(Ast::KineticLaw *scope, iNA::Parser::ConcreteSyntaxTree &params);
  /** Handles a reaction equation. */
  void processReactionEquation(iNA::Parser::ConcreteSyntaxTree &law, Ast::Reaction *reaction);
  /** Handles the reactants of a reaction. */
  void processReactants(iNA::Parser::ConcreteSyntaxTree &sum, Ast::Reaction *reaction);
  /** Handles the products of a reaction. */
  void processProducts(iNA::Parser::ConcreteSyntaxTree &sum, Ast::Reaction *reaction);
  /** Removes the quotes. */
  void unquote(std::string &name);
  /** Defines a unit in the unit table. */
  void defineUnit(const std::string &id, const Ast::Unit &unit);

protected:
  /** Holds a weak reference to the Lexer instance. */
  iNA::Parser::Lexer &_lexer;

  /** Holds a weak reference to the model being assembled. */
  Ast::Model &_model;

  /** Maps the name of a base unit to the Ast::ScaledBaseUnit::BaseUnit enum. */
  std::map<std::string, Ast::Unit::BaseUnit> _base_unit_map;

  /** Maps a unit identifier to a unit. */
  std::map<std::string, Ast::Unit> _units;
};


}
}
}

#endif // __INA_SBMLSH_ASSEMBLER_HH__
