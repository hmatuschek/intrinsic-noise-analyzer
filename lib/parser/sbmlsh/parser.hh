#ifndef __FLUC_SBMLSH_PARSER_HH__
#define __FLUC_SBMLSH_PARSER_HH__

#include "productions.hh"


namespace Fluc {

namespace Ast {
// Forward declaration.
class Model;
}


namespace Parser {
namespace Sbmlsh {

/**
 * ...
 *
 * SBML ShortHanded EBNF grammar:
 *
 * Model =
 *   ModelDefinition
 *   [EOL UnitDefinitions]
 *   [EOL CompartmentDefinitions]
 *   [EOL SpeciesDefinitiony]
 *   [EOL ParameterDefinitions]
 *   [EOL RuleDefinitions]
 *   [EOL ReactionDefinitions]
 *   [EOL EventDefinitions]
 *   [EOL] EndOfStream;
 *
 * ModelDefinition =
 *   "@model" ":" "3.3.1" "=" Identifier [QuotedString] [EOL DefaultUnitDefinitions];
 *
 * DefaultUnitDefinitions =
 *   DefaultUnitDefinition {DefaultUnitDefinition};
 *
 * DefaultUnitDefinition =
 *   ("s" | "t" | "v" | "a" | "l" | "e" | "c") "=" BaseUnitIdentifier;
 *
 * UnitDefinitions =
 *   "@units" EOL UnitDefinition {EOL UnitDefinition};
 *
 * UnitDefinition =
 *   Identifier "=" ScaledUnit {";" ScaledUnit} [QuotedString];
 *
 * ScaledUnit =
 *   BaseUnitIdentifier [":" ScaledUnitModifierList];
 *
 * BaseUnitIdentifier =
 *  ("mole" | "litre" | "second" | ...)
 *
 * ScaledUnitModifierList =
 *   ("e" | "m" | "s" | "o") "=" NUMBER ["," ScaledUnitModifierList];
 *
 * CompartmentDefinitions =
 *   "@compartments" EOL CompartmentDefinitionList;
 *
 * CompartmentDefinitionList =
 *   Identifier ["<" Identifier] ["=" Expression] [QuotedString] [EOL CompartmentDefinitionList];
 *
 * SpeciesDefinitions =
 *   "@species" EOL SpeciesDefinitionList;
 *
 * SpeciesDefinitionList =
 *   ID ":" (("[" ID "]") | ID) "=" Expression [SpeciesModifierList] [QuotedString] [EOL SpeciesDefinitionList];
 *
 * SpeciesModifierList =
 *   ("s" | "b" | "c") [SpeciesModifierList];
 *
 * ParameterDefinitions =
 *   "@parameters" EOL ParameterDefinitionList;
 *
 * ParameterDefinitionList =
 *   Identifier "=" Expression ["v"] [QuotedString] [EOL ParameterDefinition];
 *
 * RuleDefinitions =
 *   "@rules" EOL RuleDefinitionList;
 *
 * RuleDefinitionList =
 *   [("@rate"|"@assign")] ":" Identifier "=" Expression [EOL RuleDefinitionList];
 *
 * ReactionDefinitions =
 *   "@reactions" EOL ReactionDefinitionList;
 *
 * ReactionDefinitionList =
 *   ("@r" | "@rr") "=" Identifier [QuotedString] EOL
 *   ReactionEquation [":" ReactionModifierList]
 *   EOL KineticLaw
 *   [EOL ReactionDefinitionList];
 *
 * ReactionModifierList =
 *   Identifier ["," ReactionModifierList];
 *
 * ReactionEquation =
 *   [StoichiometrySum] "->" [StoichiometrySum];
 *
 * StochiometrySum =
 *   [Integer] Identifier ["+" StochiometrySum];
 *
 * KineticLaw =
 *   Expression [":" LocalParameterDefinitions];
 *
 * LocalParameterDefinitions =
 *   LocalParameterDefinition {"," LocalParameterDefinition};
 *
 * EventDefinitions =
 *   "@events" EOL EventDefinition {EOL EventDefinition};
 *
 * EventDefinition =
 *   Identifier "=" ConditionExpression [";" Expression] ":" AssignmentList [QuotedString];
 *
 * ConditionExpression =
 *   Identifier ("==" | "!=" | ">" | ">=" | "<" | "<=") Expression;
 *
 * AssignmentList =
 *   Assignment [";" Assignment];
 *
 * Assignment =
 *   Identifier "=" Expression;
 *
 * Expression =
 *   ProductExpression | (ProductExpression "+" Expression);
 *
 * ProductExpression =
 *   AtomicExpresion | (AtomicExpression "*" ProductExpression);
 *
 * AtomicExpression =
 *   Identifier | Number | ("(" Expression ")");
 *
 * @ingroup sbmlsh
 */
class Parser
{
protected:
  /** Hold the lexer. */
  Lexer lexer;
  /** Holds the root of the grammar. */
  Utils::Production *grammar;

public:
  /** Constructor. */
  Parser(std::istream &input);

  /** Parses the input stream, given to the constructor and updates the given @c Ast::Model
   * instance. */
  void parse(Ast::Model &model);

  /** Allocates and assembles a new @c Ast::Model instance from the input stream given to the
   * constructor. */
  Ast::Model *parse();
};

}
}
}
#endif
