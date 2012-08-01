#ifndef __INA_SBMLSH_ASSEMBLER_HH__
#define __INA_SBMLSH_ASSEMBLER_HH__

#include "ast/unitdefinition.hh"

#include "utils/parser.hh"
#include "utils/lexer.hh"
#include "parser/expr/assembler.hh"
#include <typeinfo>



namespace Fluc {

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
class Assembler : public Expr::Assembler
{
protected:
  /** Holds a weak reference to the model being assembled. */
  Ast::Model &_model;

  /** Maps the name of a base unit to the Ast::ScaledBaseUnit::BaseUnit enum. */
  std::map<std::string, Ast::ScaledBaseUnit::BaseUnit> _base_unit_map;


public:
  /** Constructor. */
  Assembler(Ast::Model &model, Utils::Lexer &lexer);

  /** Entry point. */
  void process(Utils::ConcreteSyntaxTree &root);

protected:
  /** Dispatcher, assembles the @c Ast::Model instance. */
  void processModel(Utils::ConcreteSyntaxTree &model);
  /** Handles model header. */
  void processModelDefinition(Utils::ConcreteSyntaxTree &model);
  /** Handles default unit definitions. */
  void processDefaultUnitDefinitions(Utils::ConcreteSyntaxTree &def_units);
  /** Handles a unit definition. */
  void processUnitDefinition(Utils::ConcreteSyntaxTree &unit);
  /** Handles a list of scaled base unit. */
  void processScaledUnitList(Utils::ConcreteSyntaxTree &unit,
                             std::list<Ast::ScaledBaseUnit> &unit_list);
  /** Handles a list of modifiers of base units. */
  void processScaledUnitModifierList(Utils::ConcreteSyntaxTree &sulist,
                                     double &multiplier, int &scale, int &exponent);
  void processCompartmentDefinitions(Utils::ConcreteSyntaxTree &comp);
  void processSpeciesDefinition(Utils::ConcreteSyntaxTree &spec);
  void processSpeciesModifierList(Utils::ConcreteSyntaxTree &spec_mod, bool &has_substance_units,
                                  bool &has_boundary_condition, bool &is_constant);
  void processParameterDefinition(Utils::ConcreteSyntaxTree &param);
  void processRuleDefinitionList(Utils::ConcreteSyntaxTree &rules);
  void processReactionDefinitions(Utils::ConcreteSyntaxTree &reac);
  void processReactionModifierList(Utils::ConcreteSyntaxTree &lst, std::list<Ast::Species *> &mods);
  Ast::KineticLaw *processKineticLaw(Utils::ConcreteSyntaxTree &law);
  void processLocalParameters(Utils::ConcreteSyntaxTree &params);
  void processReactionEquation(Utils::ConcreteSyntaxTree &law, Ast::Reaction *reaction);
  void processReactants(Utils::ConcreteSyntaxTree &sum, Ast::Reaction *reaction);
  void processProducts(Utils::ConcreteSyntaxTree &sum, Ast::Reaction *reaction);
};


}
}
}

#endif // __INA_SBMLSH_ASSEMBLER_HH__
