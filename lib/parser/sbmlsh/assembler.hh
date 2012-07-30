#ifndef __INA_SBMLSH_ASSEMBLER_HH__
#define __INA_SBMLSH_ASSEMBLER_HH__

#include "ast/unitdefinition.hh"

#include "utils/parser.hh"
#include "utils/lexer.hh"
#include <typeinfo>



namespace Fluc {

namespace Ast {
// Forward declarations...
class Model;
class Scope;
class Reaction;
class KineticLaw;
}


namespace Parser {
namespace Sbmlsh {

/** This class takes the concrete syntax tree (CST) of some parsed SBML-SH code and assembles a
 * @c Ast::Model instance from it. */
class Assembler
{
protected:
  /** Holds a weak reference to the lexer. The lexer instance is needed to access the values of the
   * token. */
  Utils::Lexer &_lexer;

  /** Holds a weak reference to the model being assembled. */
  Ast::Model &_model;

  /** Maps the name of a base unit to the Ast::ScaledBaseUnit::BaseUnit enum. */
  std::map<std::string, Ast::ScaledBaseUnit::BaseUnit> _base_unit_map;

  /** A stack of scopes. */
  std::vector<Ast::Scope *> _scope_stack;


public:
  /** Constructor. */
  Assembler(Ast::Model &model, Utils::Lexer &lexer);

  void process(Utils::ConcreteSyntaxTree &root);

protected:
  void processModel(Utils::ConcreteSyntaxTree &model);
  void processModelDefinition(Utils::ConcreteSyntaxTree &model);
  void processDefaultUnitDefinitions(Utils::ConcreteSyntaxTree &def_units);
  void processUnitDefinition(Utils::ConcreteSyntaxTree &unit);
  void processScaledUnitList(Utils::ConcreteSyntaxTree &unit,
                             std::list<Ast::ScaledBaseUnit> &unit_list);
  void processCompartmentDefinitions(Utils::ConcreteSyntaxTree &comp);
  void processSpeciesDefinition(Utils::ConcreteSyntaxTree &spec);
  void processSpeciesModifierList(Utils::ConcreteSyntaxTree &spec_mod, bool &has_substance_units,
                                  bool &has_boundary_condition, bool &is_constant);
  void processParameterDefinition(Utils::ConcreteSyntaxTree &param);
  void processReactionDefinitions(Utils::ConcreteSyntaxTree &reac);
  Ast::KineticLaw *processKineticLaw(Utils::ConcreteSyntaxTree &law);
  void processLocalParameters(Utils::ConcreteSyntaxTree &params);
  void processReactionEquation(Utils::ConcreteSyntaxTree &law, Ast::Reaction *reaction);
  void processReactants(Utils::ConcreteSyntaxTree &sum, Ast::Reaction *reaction);
  void processProducts(Utils::ConcreteSyntaxTree &sum, Ast::Reaction *reaction);

  GiNaC::ex processExpression(Utils::ConcreteSyntaxTree &expr);
  GiNaC::ex processProductExpression(Utils::ConcreteSyntaxTree &expr);
  GiNaC::ex processAtomicExpression(Utils::ConcreteSyntaxTree &expr);
  GiNaC::ex resolveIdentifier(const std::string &expr);
  double processNumber(Utils::ConcreteSyntaxTree &num);


protected:
  /** Tiny helper function to parse numbers from strings. */
  template <typename T>
  static T toNumber(const std::string &string)
  {
    T value;

    std::stringstream buffer(string);
    if (! (buffer>>value) ) {
      SBMLParserError err;
      err << "Can not parse \"" << string << "\" as " << typeid(T).name();
      throw err;
    }

    return value;
  }
};


}
}
}

#endif // __INA_SBMLSH_ASSEMBLER_HH__
