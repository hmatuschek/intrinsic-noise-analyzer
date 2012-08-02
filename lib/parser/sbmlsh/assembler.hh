#ifndef __INA_SBMLSH_ASSEMBLER_HH__
#define __INA_SBMLSH_ASSEMBLER_HH__

#include <typeinfo>

#include <ast/unitdefinition.hh>
#include <parser/lexer.hh>
#include <parser/production.hh>
#include <parser/expr/assembler.hh>



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
  Assembler(Ast::Model &model, Fluc::Parser::Lexer &lexer);

  /** Entry point. */
  void process(Fluc::Parser::ConcreteSyntaxTree &root);

protected:
  /** Dispatcher, assembles the @c Ast::Model instance. */
  void processModel(Fluc::Parser::ConcreteSyntaxTree &model);
  /** Handles model header. */
  void processModelDefinition(Fluc::Parser::ConcreteSyntaxTree &model);
  /** Handles default unit definitions. */
  void processDefaultUnitDefinitions(Fluc::Parser::ConcreteSyntaxTree &def_units);
  /** Handles a unit definition. */
  void processUnitDefinition(Fluc::Parser::ConcreteSyntaxTree &unit);
  /** Handles a list of scaled base unit. */
  void processScaledUnitList(Fluc::Parser::ConcreteSyntaxTree &unit,
                             std::list<Ast::ScaledBaseUnit> &unit_list);
  /** Handles a list of modifiers of base units. */
  void processScaledUnitModifierList(Fluc::Parser::ConcreteSyntaxTree &sulist,
                                     double &multiplier, int &scale, int &exponent);
  void processCompartmentDefinitions(Fluc::Parser::ConcreteSyntaxTree &comp);
  void processSpeciesDefinition(Fluc::Parser::ConcreteSyntaxTree &spec);
  void processSpeciesModifierList(Fluc::Parser::ConcreteSyntaxTree &spec_mod, bool &has_substance_units,
                                  bool &has_boundary_condition, bool &is_constant);
  void processParameterDefinition(Fluc::Parser::ConcreteSyntaxTree &param);
  void processRuleDefinitionList(Fluc::Parser::ConcreteSyntaxTree &rules);
  void processReactionDefinitions(Fluc::Parser::ConcreteSyntaxTree &reac);
  void processReactionModifierList(Fluc::Parser::ConcreteSyntaxTree &lst, std::list<Ast::Species *> &mods);
  Ast::KineticLaw *processKineticLaw(Fluc::Parser::ConcreteSyntaxTree &law);
  void processLocalParameters(Fluc::Parser::ConcreteSyntaxTree &params);
  void processReactionEquation(Fluc::Parser::ConcreteSyntaxTree &law, Ast::Reaction *reaction);
  void processReactants(Fluc::Parser::ConcreteSyntaxTree &sum, Ast::Reaction *reaction);
  void processProducts(Fluc::Parser::ConcreteSyntaxTree &sum, Ast::Reaction *reaction);
};


}
}
}

#endif // __INA_SBMLSH_ASSEMBLER_HH__
