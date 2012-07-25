#ifndef __FLUC_SBMLSH_PARSER_HH__
#define __FLUC_SBMLSH_PARSER_HH__

#include "productions.hh"


namespace Fluc {

namespace Ast {
// Forward declaration.
class Model;
}


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
 *   ("s" | "t" | "v" | "a" | "l" | "e" | "c") "=" Identifier;
 *
 * UnitDefinitions =
 *   "@units" EOL UnitDefinition {EOL UnitDefinition};
 *
 * UnitDefinition =
 *   Identifier "=" ScaledUnit {";" ScaledUnit};
 *
 * ScaledUnit =
 *   ("mole" | "litre" | "second" | ...) [":" ("e" | "m" | "s") "=" NUMBER];
 *
 * CompartmentDefinitions =
 *   "@compartments" EOL CompartmentDefinitionList;
 *
 * CompartmentDefinitionList =
 *   Identifier ["=" NUMBER] [QuotedString] [EOL CompartmentDefinitionList];
 *
 * SpeciesDefinitions =
 *   "@species" EOL SpeciesDefinitionList;
 *
 * SpeciesDefinitionList =
 *   ID ":" (("[" ID "]") | ID) "=" Number [SpeciesModifierList] [QuotedString] [EOL SpeciesDefinitionList];
 *
 * SpeciesModifierList =
 *   ("s" | "b" | "c") [SpeciesModifierList];
 *
 * ParameterDefinitions =
 *   "@parameters" EOL ParameterDefinitionList;
 *
 * ParameterDefinitionList =
 *   Identifier "=" Number ["v"] [QuotedString] [EOL ParameterDefinition];
 *
 * RuleDefinitions =
 *   "@rules" EOL RuleDefinitionList;
 *
 * RuleDefinitionList =
 *   ["@rate" ":"] Identifier "=" Expression [EOL RuleDefinitionList];
 *
 * ReactionDefinitions =
 *   "@reactions" EOL ReactionDefinitionList;
 *
 * ReactionDefinitionList =
 *   ("@r" | "@rr") Identifier [QuotedString] EOL
 *   ReactionEquation ((":" Identifier) | (EOL KineticLaw))
 *   [EOL ReactionDefinitionList];
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
 *   Identifier "=" ConditionExpression [";" Number] ":" AssignmentList [QuotedString];
 *
 * ConditionExpression =
 *   Identifier ("==" | "!=" | ">" | ">=" | "<" | "<=") Number;
 *
 * AssignmentList =
 *   Assignment [";" Assignment];
 *
 * Assignment =
 *   Identifier "=" Number;
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
  Lexer lexer;
  Utils::Production *grammar;

public:
  Parser(std::istream &input);

  void parse(Ast::Model &model);
  Ast::Model *parse();
};


}
}
#endif
