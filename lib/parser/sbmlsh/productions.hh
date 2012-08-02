#ifndef __INA_PARSER_SBMLSH_PRODUCTIONS_HH__
#define __INA_PARSER_SBMLSH_PRODUCTIONS_HH__

#include "lexer.hh"
#include <parser/production.hh>
#include <parser/lexer.hh>

namespace Fluc {
namespace Parser {
namespace Sbmlsh {


/**
 * Helper production to unify integers and floats to numbers with signs.
 *
 * Number =
 *   ["-"] (INTEGER | FLOAT );
 */
class NumberProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor, use factory method @c get to get an instance. */
  NumberProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static NumberProduction *instance;
};


/**
 * Helper production to handle multiple new-lines as one.
 *
 * EndOfLine =
 *   EOL [EndOfLine];
 */
class EndOfLineProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  EndOfLineProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static EndOfLineProduction *instance;
};


/**
 * DefaultUnitIdentifier =
 *   ("s" | "t" | "v" | "a" | "l" | "e" | "c");
 *
 * @todo 'e' & 'c' are no default unit identifiers!
 */
class DefaultUnitIdentifierProduction : public Fluc::Parser::TokenProduction
{
protected:
  /** Hidden constructor. */
  DefaultUnitIdentifierProduction();

public:
  /** Parses identifier. */
  virtual void parse(Fluc::Parser::Lexer &lexer, Fluc::Parser::ConcreteSyntaxTree &element);

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton-instance. */
  static DefaultUnitIdentifierProduction *instance;
};


/**
 * DefaultUnitDefinition =
 *   DefaultUnitIdentifier "=" Identifier;
 */
class DefaultUnitDefinitionProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  DefaultUnitDefinitionProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static DefaultUnitDefinitionProduction *instance;
};


/**
 * DefaultUnitDefinitions =
 *   DefaultUnitDefinition {DefaultUnitDefinitions};
 */
class DefaultUnitDefinitionsProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  DefaultUnitDefinitionsProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static DefaultUnitDefinitionsProduction *instance;
};


/**
 * ModelDefinition =
 *   "@model" ":" "3.3.1" "=" Identifier [QuotedString] [EOL DefaultUnitDefinitions];
 */
class ModelDefinitionProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  ModelDefinitionProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static ModelDefinitionProduction *instance;
};


/**
 * ScaledUnitIdentifier =
 *   ("ampere" | "becquerel" | ...);
*/
class ScaledUnitIdentifierProduction : public Fluc::Parser::TokenProduction
{
protected:
  /** Holds all valid unit identifiers.
   * \todo Try to make this set static. */
  std::set<std::string> valid_units;

protected:
  /** Hidden constructor. */
  ScaledUnitIdentifierProduction();

public:
  /** Checks if unit identifier is valid. */
  virtual void parse(Fluc::Parser::Lexer &lexer, Fluc::Parser::ConcreteSyntaxTree &element);

  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static ScaledUnitIdentifierProduction *instance;
};


/**
 * ScaledUnitModifier =
 *   ("e" | "m" | "s");
 */
class ScaledUnitModifierProduction : public Fluc::Parser::TokenProduction
{
protected:
  /** Hidden constructor. */
  ScaledUnitModifierProduction();

public:
  /** Checks modifier. */
  virtual void parse(Fluc::Parser::Lexer &lexer, Fluc::Parser::ConcreteSyntaxTree &element);

  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static ScaledUnitModifierProduction *instance;
};


/**
 * ScaledUnitModifierList =
 *   ScaledUnitModifier "=" NUMBER [',' ScaledUnitModifierList];
 */
class ScaledUnitModifierListProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  ScaledUnitModifierListProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static ScaledUnitModifierListProduction *instance;
};


/**
 * ScaledUnitList =
 *   ScaledUnitIdentifier [":" ScaledUnitModifier "=" NUMBER];
 */
class ScaledUnitListProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  ScaledUnitListProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static ScaledUnitListProduction *instance;
};


/**
 * UnitDefinitionList =
 *   Identifier "=" ScaledUnitList [EOL UnitDefinitionList]
 */
class UnitDefinitionListProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  UnitDefinitionListProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static UnitDefinitionListProduction *instance;
};


/**
 * UnitDefinitions =
 *   "@units" EOL UnitDefinitionList;
 */
class UnitDefinitionsProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  UnitDefinitionsProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static UnitDefinitionsProduction *instance;
};


/**
 * CompartmentDefinitionList =
 *   Identifier ["=" NUMBER] [QuotedString] [EOL CompartmentDefinitionList];
 */
class CompartmentDefinitionListProduction : public Fluc::Parser::Production
{
protected:
  /** hidden constructor. */
  CompartmentDefinitionListProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static CompartmentDefinitionListProduction *instance;
};


/**
 * CompartmentDefinitions =
 *   "@compartments" EOL CompartmentDefinitionList;
 */
class CompartmentDefinitionsProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  CompartmentDefinitionsProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static CompartmentDefinitionsProduction *instance;
};


/**
 * SpeciesModifier =
 *   ("s" | "b" | "c")
 */
class SpeciesModifierProduction : public Fluc::Parser::TokenProduction
{
protected:
  /** Hidden constructor. */
  SpeciesModifierProduction();

public:
  /** Checks species modifier. */
  virtual void parse(Fluc::Parser::Lexer &lexer, Fluc::Parser::ConcreteSyntaxTree &element);

  /** factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static SpeciesModifierProduction *instance;
};


/**
 * SpeciesModifierList =
 *   SpeciesModifier [SpeciesModifierList];
 */
class SpeciesModifierListProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  SpeciesModifierListProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static SpeciesModifierListProduction *instance;
};



/**
 * SpeciesDefinitionList =
 *   ID ":" (("[" ID "]") | ID) "=" Number [SpeciesModifierList] [QuotedString] [EOL SpeciesDefinitionList];
 */
class SpeciesDefinitionListProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  SpeciesDefinitionListProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static SpeciesDefinitionListProduction *instance;
};


/**
 * SpeciesDefinitions =
 *   "@species" EOL SpeciesDefinitionList;
 */
class SpeciesDefinitionsProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  SpeciesDefinitionsProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static SpeciesDefinitionsProduction *instance;
};


/**
 * ParameterModifier =
 *   "v";
 */
class ParameterModifierProduction : public Fluc::Parser::TokenProduction
{
protected:
  /** Hidden constructor. */
  ParameterModifierProduction();

public:
  /** Checks modifier. */
  virtual void parse(Fluc::Parser::Lexer &lexer, Fluc::Parser::ConcreteSyntaxTree &element);

  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static ParameterModifierProduction *instance;
};


/**
 * ParameterDefinitionList =
 *   Identifier "=" Number [ParameterModifier] [QuotedString] [EOL ParameterDefinition];
 */
class ParameterDefinitionListProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  ParameterDefinitionListProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static ParameterDefinitionListProduction *instance;
};


/**
 * ParameterDefinitions =
 *   "@parameters" EOL ParameterDefinitionList;
 */
class ParameterDefinitionsProduction : Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  ParameterDefinitionsProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static ParameterDefinitionsProduction *instance;
};


/**
 * RuleDefinitionList =
 *   [("@rate"|"@assign")] ":" Identifier "=" Expression [EOL RuleDefinitionList];
 */
class RuleDefinitionListProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  RuleDefinitionListProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static RuleDefinitionListProduction *instance;
};


/**
 * RuleDefinitions =
 *   "@rules" EOL RuleDefinitionList;
 */
class RuleDefinitionsProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  RuleDefinitionsProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static RuleDefinitionsProduction *instance;
};


/**
 * StochiometrySum =
 *   [Integer] Identifier ["+" StochiometrySum];
 */
class StoichiometrySumProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  StoichiometrySumProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** singleton instance. */
  static StoichiometrySumProduction *instance;
};


/**
 * ReactionEquation =
 *   (StoichiometrySum "->" [StoichiometrySum]) | ( "->" StoichiometrySum);
 */
class ReactionEquationProduction : public Fluc::Parser::AltProduction
{
protected:
  /** Hidden constructor. */
  ReactionEquationProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static ReactionEquationProduction *instance;
};


/**
 * LocalParameterList =
 *   Identifier "=" Expression ["," LocalParameterList];
 */
class LocalParameterListProduction : public Fluc::Parser::Production
{
protected:
  /**
   * Hidden constructor.
   */
  LocalParameterListProduction();

public:
  /**
   * Factory method.
   */
  static Fluc::Parser::Production *get();

private:
  /**
   * Singleton instance.
   */
  static LocalParameterListProduction *instance;
};


/**
 * KineticLaw =
 *   Expression [":" LocalParameterDefinitionList];
 */
class KineticLawProduction : public Fluc::Parser::Production
{
protected:
  /**
   * Hidden constructor.
   */
  KineticLawProduction();

public:
  /**
   * Factory method.
   */
  static Fluc::Parser::Production *get();

private:
  static KineticLawProduction *instance;
};


/**
 * ReactionModifierList =
 *   Identifier [',' ReactionModifierList];
 */
class ReactionModifierListProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor.*/
  ReactionModifierListProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

protected:
  /** Singleton instance. */
  static ReactionModifierListProduction *instance;
};


/**
 * ReactionDefinitionList =
 *   ("@r" | "@rr") Identifier [QuotedString] EOL
 *   ReactionEquation [":" ReactionModifierList]
 *   EOL KineticLaw
 *   [EOL ReactionDefinitionList];
 */
class ReactionDefinitionListProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  ReactionDefinitionListProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static ReactionDefinitionListProduction *instance;
};


/**
 * ReactionDefinitions =
 *   "@reactions" EOL ReactionDefinitionList;
 */
class ReactionDefinitionsProduction : public Fluc::Parser::Production
{
protected:
  /** Hidden constructor. */
  ReactionDefinitionsProduction();

public:
  /** Factory method. */
  static Fluc::Parser::Production *get();

private:
  /** Singleton instance. */
  static ReactionDefinitionsProduction *instance;
};



/**
 * AssignmentList =
 *   Identifier "=" Number [";" AssignmentList];
 */
class AssignmentListProduction : public Fluc::Parser::Production
{
protected:
  /**
   * Hidden constructor.
   */
  AssignmentListProduction();

public:
  /**
   * Factory method.
   */
  static Fluc::Parser::Production *get();

private:
  /**
   * Singleton instance.
   */
  static AssignmentListProduction *instance;
};


/**
 * ConditionExpression =
 *   Identifier ("==" | "!=" | ">" | ">=" | "<" | "<=") Number;
 */
class ConditionExpressionProduction : public Fluc::Parser::Production
{
protected:
  /**
   * Hidden constructor.
   */
  ConditionExpressionProduction();

public:
  /**
   * Factory method.
   */
  static Fluc::Parser::Production *get();

private:
  /**
   * Singleton instance.
   */
  static ConditionExpressionProduction *instance;
};


/**
 * EventDefinitionList =
 *   Identifier "=" ConditionExpression [";" Number] ":" AssignmentList [QuotedString]
 *   [EOL EventDefinitionList];
 */
class EventDefinitionListProduction : public Fluc::Parser::Production
{
protected:
  /**
   * Hidden constructor.
   */
  EventDefinitionListProduction();

public:
  /**
   * Factory method.
   */
  static Fluc::Parser::Production *get();

private:
  /**
   * Singleton instance.
   */
  static EventDefinitionListProduction *instance;
};


/**
 * EventDefinitions =
 *   "@events" EOL EventDefinitionList;
 */
class EventDefinitionsProduction : public Fluc::Parser::Production
{
protected:
  /**
   * Hidden constructor.
   */
  EventDefinitionsProduction();

public:
  /**
   * Factory method.
   */
  static Fluc::Parser::Production *get();

private:
  /**
   * singleton instance.
   */
  static EventDefinitionsProduction *instance;
};


/**
 * Model =
 *   ModelDefinition
 *   [EOL UnitDefinitions]
 *   [EOL CompartmentDefinitions]
 *   [EOL SpeciesDefinitiony]
 *   [EOL ParameterDefinitions]
 *   [EOL RuleDefinitions]
 *   [EOL ReactionDefinitions]
 *   [EOL EventDefinitions];
 */
class ModelProduction : public Fluc::Parser::Production
{
protected:
  /**
   * Hidden constructor.
   */
  ModelProduction();

public:
  /**
   * Factory method.
   */
  static Fluc::Parser::Production *get();

private:
  /**
   * Singleton instance.
   */
  static ModelProduction *instance;
};


}
}
}

#endif // __INA_PARSER_SBMLSH_PRODUCTIONS_HH__
