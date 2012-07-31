#include "productions.hh"
#include <utils/parser.hh>
#include <utils/exception.hh>


using namespace Fluc;
using namespace Fluc::Parser::Sbmlsh;



/* ******************************************************************************************** *
 * Implementation of ModelProduction:
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
 * ******************************************************************************************** */
ModelProduction::ModelProduction()
  : Utils::Production()
{
  ModelProduction::instance = this;

  this->elements.push_back(ModelDefinitionProduction::get());

  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, EndOfLineProduction::get(), UnitDefinitionsProduction::get())));

  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, EndOfLineProduction::get(), CompartmentDefinitionsProduction::get())));

  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, EndOfLineProduction::get(), SpeciesDefinitionsProduction::get())));

  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, EndOfLineProduction::get(), ParameterDefinitionsProduction::get())));

  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, EndOfLineProduction::get(), RuleDefinitionsProduction::get())));

  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, EndOfLineProduction::get(), ReactionDefinitionsProduction::get())));

  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, EndOfLineProduction::get(), EventDefinitionsProduction::get())));

  this->elements.push_back(new Utils::OptionalProduction(EndOfLineProduction::get()));
  this->elements.push_back(new Utils::TokenProduction(Utils::Token::END_OF_INPUT));
}


ModelProduction *ModelProduction::instance = 0;

Utils::Production *
ModelProduction::get()
{
  if (0 == ModelProduction::instance)
    new ModelProduction();

  return ModelProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ModelDefinitionsProduction:
 *
 * ModelDefinition =
 *   "@model" ":" "3.3.1" "=" Identifier [QuotedString] [EOL DefaultUnitDefinitions];
 * ******************************************************************************************** */
ModelDefinitionProduction::ModelDefinitionProduction()
  : Utils::Production()
{
  ModelDefinitionProduction::instance = this;

  // "@model" ":" "3.3.1" "=" Identifier
  this->elements.push_back(new Utils::TokenProduction(T_MODEL_KW));
  this->elements.push_back(new Utils::TokenProduction(T_COLON));
  this->elements.push_back(new Utils::TokenProduction(T_VERSION_NUMBER));
  this->elements.push_back(new Utils::TokenProduction(T_ASSIGN));
  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));

  // [QuotedString]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::TokenProduction(T_QUOTED_STRING)));

  // [EOL DefaultUnitDefinitions]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Production(2, EndOfLineProduction::get(),
                         DefaultUnitDefinitionsProduction::get())));
}

ModelDefinitionProduction *ModelDefinitionProduction::instance = 0;

Utils::Production *
ModelDefinitionProduction::get()
{
  if (0 == ModelDefinitionProduction::instance)
    new ModelDefinitionProduction();

  return ModelDefinitionProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of DefaultUnitDefinitionsProduction:
 *
 * DefaultUnitDefinitions =
 *   DefaultUnitDefinition {DefaultUnitDefinitions};
 * ******************************************************************************************** */
DefaultUnitDefinitionsProduction::DefaultUnitDefinitionsProduction()
  : Utils::Production()
{
  DefaultUnitDefinitionsProduction::instance = this;

  this->elements.push_back(DefaultUnitDefinitionProduction::get());
  this->elements.push_back(new Utils::OptionalProduction(this));
}

DefaultUnitDefinitionsProduction *DefaultUnitDefinitionsProduction::instance = 0;

Utils::Production *
DefaultUnitDefinitionsProduction::get()
{
  if (0 == DefaultUnitDefinitionsProduction::instance)
    new DefaultUnitDefinitionsProduction();

  return DefaultUnitDefinitionsProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of DefaultUnitDefinitionProduction:
 *
 * DefaultUnitDefinition =
 *   DefaultUnitIdentifier "=" Identifier;
 * ******************************************************************************************** */
DefaultUnitDefinitionProduction::DefaultUnitDefinitionProduction()
  : Utils::Production()
{
  DefaultUnitDefinitionProduction::instance = this;

  this->elements.push_back(DefaultUnitIdentifierProduction::get());
  this->elements.push_back(new Utils::TokenProduction(T_ASSIGN));
  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));
}

DefaultUnitDefinitionProduction *DefaultUnitDefinitionProduction::instance = 0;

Utils::Production *
DefaultUnitDefinitionProduction::get()
{
  if (0 == DefaultUnitDefinitionProduction::instance)
    new DefaultUnitDefinitionProduction();

  return DefaultUnitDefinitionProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of DefaultUnitIdentifierProduction:
 *
 * DefaultUnitIdentifier =
 *   ("s" | "t" | "v" | "a" | "l" | "e" | "c");
 * ******************************************************************************************** */
DefaultUnitIdentifierProduction::DefaultUnitIdentifierProduction()
  : Utils::TokenProduction(T_IDENTIFIER)
{
  DefaultUnitIdentifierProduction::instance = this;
}

void
DefaultUnitIdentifierProduction::parse(Utils::Lexer &lexer, Utils::ConcreteSyntaxTree &element)
{
  std::string id = lexer.current().getValue();
  TokenProduction::parse(lexer, element);

  if ("s" != id && "t" != id && "v" != id && "a" != id && "l" != id && "e" != id && "c" != id)
  {
    Utils::SyntaxError err;
    err << "Unexpected identifier " << id << " expected s, t, v, a, l, e or c.";
    throw err;
  }
}

DefaultUnitIdentifierProduction *DefaultUnitIdentifierProduction::instance = 0;

Utils::Production *
DefaultUnitIdentifierProduction::get()
{
  if (0 == DefaultUnitIdentifierProduction::instance)
    new DefaultUnitIdentifierProduction();

  return DefaultUnitIdentifierProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of UnitDefinitionsProduction:
 *
 * UnitDefinitions =
 *   "@units" EOL UnitDefinitionList;
 * ******************************************************************************************** */
UnitDefinitionsProduction::UnitDefinitionsProduction()
  : Utils::Production()
{
  UnitDefinitionsProduction::instance = this;

  this->elements.push_back(new Utils::TokenProduction(T_UNITS_KW));
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(UnitDefinitionListProduction::get());
}

UnitDefinitionsProduction *UnitDefinitionsProduction::instance = 0;

Utils::Production *
UnitDefinitionsProduction::get()
{
  if (0 == UnitDefinitionsProduction::instance)
    new UnitDefinitionsProduction();

  return UnitDefinitionsProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of UnitDefinitionListProduction:
 *
 * UnitDefinitionList =
 *   Identifier "=" ScaledUnitList [EOL UnitDefinitionList]
 * ******************************************************************************************** */
UnitDefinitionListProduction::UnitDefinitionListProduction()
  : Production()
{
  UnitDefinitionListProduction::instance = this;

  // Identifier "=" ScaledUnitList
  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new Utils::TokenProduction(T_ASSIGN));
  this->elements.push_back(ScaledUnitListProduction::get());

  // [EOL UnitDefinitionList]
  std::list<Production *> tmp;
  tmp.push_back(EndOfLineProduction::get());
  tmp.push_back(this);
  this->elements.push_back(new Utils::OptionalProduction(new Production(tmp)));
}

UnitDefinitionListProduction *UnitDefinitionListProduction::instance = 0;

Utils::Production *
UnitDefinitionListProduction::get()
{
  if (0 == UnitDefinitionListProduction::instance)
    new UnitDefinitionListProduction();

  return UnitDefinitionListProduction::instance;
}


/* ******************************************************************************************** *
 * ScaledUnitModifierList =
 *   ScaledUnitModifier "=" NUMBER [',' ScaledUnitModifierList];
 * ******************************************************************************************** */
ScaledUnitModifierListProduction::ScaledUnitModifierListProduction()
  : Production()
{
  ScaledUnitModifierListProduction::instance = this;

  // ("m" | "s" | "e") "=" NUMBER
  this->elements.push_back(ScaledUnitModifierProduction::get());
  this->elements.push_back(new Utils::TokenProduction(T_ASSIGN));
  this->elements.push_back(NumberProduction::get());
  // ["," ScaledUnitModifierList]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, new Utils::TokenProduction(T_COMMA), this)));
}

ScaledUnitModifierListProduction *ScaledUnitModifierListProduction::instance = 0;

Utils::Production *
ScaledUnitModifierListProduction::get()
{
  if (0 == ScaledUnitModifierListProduction::instance)
    return new ScaledUnitModifierListProduction();
  return ScaledUnitModifierListProduction::instance;
}


/* ******************************************************************************************** *
 * Implementation of ScaledUnitListProduction:
 *
 * ScaledUnitList =
 *   ScaledUnitIdentifier [":" ScaledUnitModifier "=" Number] ";" [ScaledUnitList];
 * ******************************************************************************************** */
ScaledUnitListProduction::ScaledUnitListProduction()
  : Production()
{
  ScaledUnitListProduction::instance = this;

  // ("mole" | "litre" | ...)
  this->elements.push_back(ScaledUnitIdentifierProduction::get());

  // [":" ScaledUnitModifierList]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, new Utils::TokenProduction(T_COLON), ScaledUnitModifierListProduction::get())));

  // ";"
  this->elements.push_back(new Utils::TokenProduction(T_SEMICOLON));

  // [ScaledUnitList]
  this->elements.push_back(new Utils::OptionalProduction(this));
}

ScaledUnitListProduction * ScaledUnitListProduction::instance = 0;

Utils::Production *
ScaledUnitListProduction::get()
{
  if (0 == ScaledUnitListProduction::instance)
    new ScaledUnitListProduction();

  return ScaledUnitListProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ScaledUnitIdentifierProduction:
 *
 * ScaledUnitIdentifier =
 *   ("mole" | "litre" | "second" | ...);
 * ******************************************************************************************** */
ScaledUnitIdentifierProduction::ScaledUnitIdentifierProduction()
  : TokenProduction(T_IDENTIFIER)
{
  ScaledUnitIdentifierProduction::instance = this;

  valid_units.insert("ampere"); valid_units.insert("becquerel"); valid_units.insert("candela");
  valid_units.insert("coulomb"); valid_units.insert("dimensionless"); valid_units.insert("farad");
  valid_units.insert("gram"); valid_units.insert("katal"); valid_units.insert("gray");
  valid_units.insert("kelvin"); valid_units.insert("henry"); valid_units.insert("kilogram");
  valid_units.insert("hertz"); valid_units.insert("litre"); valid_units.insert("item");
  valid_units.insert("lumen"); valid_units.insert("joule"); valid_units.insert("lux");
  valid_units.insert("metre"); valid_units.insert("mole"); valid_units.insert("newton");
  valid_units.insert("ohm"); valid_units.insert("pascal"); valid_units.insert("radian");
  valid_units.insert("second"); valid_units.insert("watt"); valid_units.insert("siemens");
  valid_units.insert("weber"); valid_units.insert("sievert"); valid_units.insert("steradian");
  valid_units.insert("tesla"); valid_units.insert("volt");
}


void
ScaledUnitIdentifierProduction::parse(Utils::Lexer &lexer, Utils::ConcreteSyntaxTree &element)
{
  // Get value of current token.
  std::string id = lexer.current().getValue();
  unsigned int line = lexer.current().getLine();

  // Parse identifier:
  TokenProduction::parse(lexer, element);

  //std::cerr << "Parsed ScaledUnitIdentifier: " << id << std::endl;

  // Check if identifier not matches:
  if (this->valid_units.end() == this->valid_units.find(id))
  {
    Utils::SyntaxError err;
    err << "@line " << line << ": "
        << "Invalid (base-) unit " << id;
    throw err;
  }
}


ScaledUnitIdentifierProduction *ScaledUnitIdentifierProduction::instance = 0;

Utils::Production *
ScaledUnitIdentifierProduction::get()
{
  if (0 == ScaledUnitIdentifierProduction::instance)
    new ScaledUnitIdentifierProduction();

  return ScaledUnitIdentifierProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ScaledUnitModifierProduction:
 *
 * ScaledUnitModifier =
 *   ("e" | "m" | "s");
 * ******************************************************************************************** */
ScaledUnitModifierProduction::ScaledUnitModifierProduction()
  : TokenProduction(T_IDENTIFIER)
{
  ScaledUnitModifierProduction::instance = this;
}


void
ScaledUnitModifierProduction::parse(Utils::Lexer &lexer, Utils::ConcreteSyntaxTree &element)
{
  // Get identifier
  std::string id = lexer.current().getValue();
  unsigned int line = lexer.current().getLine();

  // Parse identifier:
  TokenProduction::parse(lexer, element);

  // Check identifier:
  if ("e" != id && "m" != id && "s" != id) {
    Utils::SyntaxError err;
    err << "@line " << line << ": "
        << "Invalid unit-modifier: " << id;
    throw err;
  }
}


ScaledUnitModifierProduction *ScaledUnitModifierProduction::instance = 0;

Utils::Production *
ScaledUnitModifierProduction::get()
{
  if (0 == ScaledUnitModifierProduction::instance)
    new ScaledUnitModifierProduction();

  return ScaledUnitModifierProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ComparmentDefinitionsProduction:
 *
 * CompartmentDefinitions =
 *   "@compartments" EOL CompartmentDefinitionList;
 * ******************************************************************************************** */
CompartmentDefinitionsProduction::CompartmentDefinitionsProduction()
  : Production()
{
  CompartmentDefinitionsProduction::instance = this;

  this->elements.push_back(new Utils::TokenProduction(T_COMPARTMENTS_KW));
  this->elements.push_back(new Utils::TokenProduction(T_END_OF_LINE));
  this->elements.push_back(CompartmentDefinitionListProduction::get());
}

CompartmentDefinitionsProduction *CompartmentDefinitionsProduction::instance = 0;

Utils::Production *
CompartmentDefinitionsProduction::get()
{
  if (0 == CompartmentDefinitionsProduction::instance)
    new CompartmentDefinitionsProduction();

  return CompartmentDefinitionsProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ComparmentDefinitionListProduction:
 *
 * CompartmentDefinitionList =
 *   Identifier ["=" Number] [QuotedString] [EOL CompartmentDefinitionList];
 * ******************************************************************************************** */
CompartmentDefinitionListProduction::CompartmentDefinitionListProduction()
  : Production()
{
  CompartmentDefinitionListProduction::instance = this;

  // Identifier
  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));

  // ["=" NUMBER]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, new Utils::TokenProduction(T_ASSIGN), NumberProduction::get())));

  // [QuotedString]
  this->elements.push_back(
        new Utils::OptionalProduction(new Utils::TokenProduction(T_QUOTED_STRING)));

  // [EOL CompartmentDefinitionList]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(2, EndOfLineProduction::get(), this)));
}

CompartmentDefinitionListProduction *CompartmentDefinitionListProduction::instance = 0;

Utils::Production *
CompartmentDefinitionListProduction::get()
{
  if (0 == CompartmentDefinitionListProduction::instance)
    new CompartmentDefinitionListProduction();

  return CompartmentDefinitionListProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of SpeciesDefinitionProduction:
 *
 * SpeciesDefinitions =
 *   "@species" EOL SpeciesDefinitionList;
 * ******************************************************************************************** */
SpeciesDefinitionsProduction::SpeciesDefinitionsProduction()
  : Production()
{
  SpeciesDefinitionsProduction::instance = this;

  this->elements.push_back(new Utils::TokenProduction(T_SPECIES_KW));
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(SpeciesDefinitionListProduction::get());
}

SpeciesDefinitionsProduction *SpeciesDefinitionsProduction::instance = 0;

Utils::Production *
SpeciesDefinitionsProduction::get()
{
  if (0 == SpeciesDefinitionsProduction::instance)
    new SpeciesDefinitionsProduction();

  return SpeciesDefinitionsProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of SpeciesDefinitionListProduction:
 *
 * SpeciesDefinitionList =
 *   ID ":" (("[" ID "]") | ID) "=" Number [SpeciesModifierList] [QuotedString] [EOL SpeciesDefinitionList];
 * ******************************************************************************************** */
SpeciesDefinitionListProduction::SpeciesDefinitionListProduction()
  : Production()
{
  SpeciesDefinitionListProduction::instance = this;

  // ID ":"
  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new Utils::TokenProduction(T_COLON));

  // ("[" ID "]" | ID)
  this->elements.push_back(
        new Utils::AltProduction(
          2, new Utils::Production(
            3, new Utils::TokenProduction(T_LBRAC),
            new Utils::TokenProduction(T_IDENTIFIER),
            new Utils::TokenProduction(T_RBRAC)),
          new Utils::TokenProduction(T_IDENTIFIER)));

  // "=" Number
  this->elements.push_back(new Utils::TokenProduction(T_ASSIGN));
  this->elements.push_back(NumberProduction::get());

  // [SpeciesModifierList]
  this->elements.push_back(new Utils::OptionalProduction(SpeciesModifierListProduction::get()));

  // [QuotedString]
  this->elements.push_back(
        new Utils::OptionalProduction(new Utils::TokenProduction(T_QUOTED_STRING)));

  // [EOL SpeciesDefinitionList]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(2, EndOfLineProduction::get(), this)));
}

SpeciesDefinitionListProduction *SpeciesDefinitionListProduction::instance = 0;

Utils::Production *
SpeciesDefinitionListProduction::get()
{
  if (0 == SpeciesDefinitionListProduction::instance)
    new SpeciesDefinitionListProduction();

  return SpeciesDefinitionListProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of SpeciesModifierListProduction:
 *
 * SpeciesModifierList =
 *   SpeciesModifier [SpeciesModifierList];
 * ******************************************************************************************** */
SpeciesModifierListProduction::SpeciesModifierListProduction()
  : Production()
{
  SpeciesModifierListProduction::instance = this;

  this->elements.push_back(SpeciesModifierProduction::get());
  this->elements.push_back(new Utils::OptionalProduction(this));
}

SpeciesModifierListProduction *SpeciesModifierListProduction::instance = 0;

Utils::Production *
SpeciesModifierListProduction::get()
{
  if (0 == SpeciesModifierListProduction::instance)
    new SpeciesModifierListProduction();

  return SpeciesModifierListProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of SpeciesModifierProduction:
 *
 * SpeciesModifier = Any series of 's', 'b' and 'c'
 * ******************************************************************************************** */
SpeciesModifierProduction::SpeciesModifierProduction()
  : TokenProduction(T_IDENTIFIER)
{
  SpeciesModifierProduction::instance = this;
}

void
SpeciesModifierProduction::parse(Utils::Lexer &lexer, Utils::ConcreteSyntaxTree &element)
{
  // Get token value:
  std::string id = lexer.current().getValue();
  unsigned int line    = lexer.current().getLine();

  // consume token.
  Utils::TokenProduction::parse(lexer, element);

  // Check if id matches:
  for (size_t i=0; i<id.size(); i++)
  {
    if (('s' != id[i]) && ('b' != id[i]) && ('c' != id[i]))
    {
      Utils::SyntaxError err;
      err << "@line " << line << ": "
          << "Unknown species-modifier: " << id << ", expected: s, b or c.";
      throw err;
    }
  }
}

SpeciesModifierProduction *SpeciesModifierProduction::instance = 0;

Utils::Production *
SpeciesModifierProduction::get()
{
  if (0 == SpeciesModifierProduction::instance)
    new SpeciesModifierProduction();

  return SpeciesModifierProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ParameterDefinitionsProduction:
 *
 * ParameterDefinitions =
 *   "@parameters" EOL ParameterDefinitionList;
 * ******************************************************************************************** */
ParameterDefinitionsProduction::ParameterDefinitionsProduction()
  : Production()
{
  ParameterDefinitionsProduction::instance = this;
  this->elements.push_back(new Utils::TokenProduction(T_PARAMETERS_KW));
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(ParameterDefinitionListProduction::get());
}

ParameterDefinitionsProduction *ParameterDefinitionsProduction::instance = 0;

Utils::Production *
ParameterDefinitionsProduction::get()
{
  if (0 == ParameterDefinitionsProduction::instance)
    new ParameterDefinitionsProduction();

  return ParameterDefinitionsProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ParameterDefinitionListProduction:
 *
 * ParameterDefinitionList =
 *   Identifier "=" Number [ParameterModifier] [QuotedString] [EOL ParameterDefinitionList];
 * ******************************************************************************************** */
ParameterDefinitionListProduction::ParameterDefinitionListProduction()
  : Production()
{
  ParameterDefinitionListProduction::instance = this;

  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new Utils::TokenProduction(T_ASSIGN));
  this->elements.push_back(NumberProduction::get());
  this->elements.push_back(new Utils::OptionalProduction(ParameterModifierProduction::get()));
  this->elements.push_back(new Utils::OptionalProduction(new Utils::TokenProduction(T_QUOTED_STRING)));
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(2, EndOfLineProduction::get(), this)));
}

ParameterDefinitionListProduction *ParameterDefinitionListProduction::instance = 0;

Utils::Production *
ParameterDefinitionListProduction::get()
{
  if (0 == ParameterDefinitionListProduction::instance)
    new ParameterDefinitionListProduction();

  return ParameterDefinitionListProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ParameterModifierProduction:
 *
 * ParameterModifier =
 *   "v";
 * ******************************************************************************************** */
ParameterModifierProduction::ParameterModifierProduction()
  : TokenProduction(T_IDENTIFIER)
{
  ParameterModifierProduction::instance = this;
}

void
ParameterModifierProduction::parse(Utils::Lexer &lexer, Utils::ConcreteSyntaxTree &element)
{
  std::string id = lexer.current().getValue();
  unsigned int    line = lexer.current().getLine();

  // Parse token:
  TokenProduction::parse(lexer, element);

  if ("v" != id) {
    Utils::SyntaxError err;
    err << "@line " << line << ": "
        << "Unknown parameter modifier " << id << " expected v.";
    throw err;
  }
}

ParameterModifierProduction *ParameterModifierProduction::instance = 0;

Utils::Production *
ParameterModifierProduction::get()
{
  if (0 == ParameterModifierProduction::instance)
    new ParameterModifierProduction();

  return ParameterModifierProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of RuleDefinitionsProduction:
 *
 * RuleDefinitions =
 *   "@rules" EOL RuleDefinitionList;
 * ******************************************************************************************** */
RuleDefinitionsProduction::RuleDefinitionsProduction()
  : Utils::Production()
{
  RuleDefinitionsProduction::instance = this;
  this->elements.push_back(new Utils::TokenProduction(T_RULES_KW));
  this->elements.push_back(RuleDefinitionListProduction::get());
}

RuleDefinitionsProduction *RuleDefinitionsProduction::instance = 0;

Utils::Production *
RuleDefinitionsProduction::get()
{
  if (0 == RuleDefinitionsProduction::instance)
    new RuleDefinitionsProduction();

  return RuleDefinitionsProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of RuleDefinitionListProduction:
 *
 * RuleDefinitionList =
 *   [("@rate"|"@assign")] ":" Identifier "=" Expression [EOL RuleDefinitionList];
 * ******************************************************************************************** */
RuleDefinitionListProduction::RuleDefinitionListProduction()
  : Production()
{
  RuleDefinitionListProduction::instance = this;

  // [("@rate"|"@assign)]"
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::AltProduction(
            2, new Utils::TokenProduction(T_RATE_KW), new Utils::TokenProduction(T_ASSIGN_KW))));
  // ":" Identifier "=" Expression
  this->elements.push_back(new Utils::TokenProduction(T_COLON));
  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new Utils::TokenProduction(T_ASSIGN));
  this->elements.push_back(ExpressionProduction::get());
  //  [EOL RuleDefinitionList]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(2, EndOfLineProduction::get(), this)));
}

RuleDefinitionListProduction *RuleDefinitionListProduction::instance = 0;

Utils::Production *
RuleDefinitionListProduction::get()
{
  if (0 == RuleDefinitionListProduction::instance)
    new RuleDefinitionListProduction();

  return RuleDefinitionListProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ReactionDefinitionsProduction:
 *
 * ReactionDefinitions =
 *   "@reactions" EOL ReactionDefinitionList;
 * ******************************************************************************************** */
ReactionDefinitionsProduction::ReactionDefinitionsProduction()
  : Production()
{
  ReactionDefinitionsProduction::instance = this;

  this->elements.push_back(new Utils::TokenProduction(T_REACTIONS_KW));
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(ReactionDefinitionListProduction::get());
}

ReactionDefinitionsProduction *ReactionDefinitionsProduction::instance = 0;

Utils::Production *
ReactionDefinitionsProduction::get()
{
  if (0 == ReactionDefinitionsProduction::instance)
    new ReactionDefinitionsProduction();

  return ReactionDefinitionsProduction::instance;
}


/* ******************************************************************************************** *
 * Implementation of ReactionModifierListProduction:
 *
 * ReactionModifierList =
 *   Identifier ["," ReactionModifierList];
 * ******************************************************************************************** */
ReactionModifierList::ReactionModifierList()
  : Utils::Production()
{
  ReactionModifierList::instance = this;
  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(2, new Utils::TokenProduction(T_COMMA), this)));
}

ReactionModifierList *ReactionModifierList::instance = 0;

Utils::Production *
ReactionModifierList::get()
{
  if (0 == ReactionModifierList::instance)
    return new ReactionModifierList();
  return ReactionModifierList::instance;
}


/* ******************************************************************************************** *
 * Implementation of ReactionDefinitionListProduction:
 *
 * ReactionDefinitionList =
 *   ("@r" | "@rr") "=" Identifier [QuotedString] EOL
 *   ReactionEquation [":" ReactionModifierList] EOL
 *   KineticLaw
 *   [EOL ReactionDefinitionList];
 * ******************************************************************************************** */
ReactionDefinitionListProduction::ReactionDefinitionListProduction()
  : Production()
{
  ReactionDefinitionListProduction::instance = this;

  // ("@r"|"@rr")
  this->elements.push_back(
        new Utils::AltProduction(
          2, new Utils::TokenProduction(T_R_KW), new Utils::TokenProduction(T_RR_KW)));

  // "="
  this->elements.push_back(
        new Utils::TokenProduction(T_ASSIGN));

  // ID
  this->elements.push_back(
        new Utils::TokenProduction(T_IDENTIFIER));

  // [QuotedString]
  this->elements.push_back(
        new Utils::OptionalProduction(new Utils::TokenProduction(T_QUOTED_STRING)));

  // EOL ReactionEquation
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(ReactionEquationProduction::get());

  // [":" ReactionModifierList]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2,
            new Utils::TokenProduction(T_COLON),
            ReactionModifierList::get())));

  // EOL KineticLaw
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(KineticLawProduction::get());

  // [EOL ReactionDefinitionList]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(2, EndOfLineProduction::get(), this)));
}


ReactionDefinitionListProduction *ReactionDefinitionListProduction::instance = 0;

Utils::Production *
ReactionDefinitionListProduction::get()
{
  if (0 == ReactionDefinitionListProduction::instance)
    new ReactionDefinitionListProduction();

  return ReactionDefinitionListProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ReactionEquationProduction:
 *
 * ReactionEquation =
 *   (StoichiometrySum "->" [StoichiometrySum]) | ( "->" StoichiometrySum);
 * ******************************************************************************************** */
ReactionEquationProduction::ReactionEquationProduction()
  : AltProduction()
{
  ReactionEquationProduction::instance = this;

  this->alternatives.resize(2);

  // StoichiometrySum "-> [StoichiometrySum];
  this->alternatives[0] =
      new Utils::Production(
        3, StoichiometrySumProduction::get(),
        new Utils::TokenProduction(T_RARROW),
        StoichiometrySumProduction::get());

  // "->" StoichiometrySum
  this->alternatives[1] =
      new Utils::Production(
        2, new Utils::TokenProduction(T_RARROW),
        StoichiometrySumProduction::get());
}

ReactionEquationProduction *ReactionEquationProduction::instance = 0;

Utils::Production *
ReactionEquationProduction::get()
{
  if (0 == ReactionEquationProduction::instance)
    new ReactionEquationProduction();

  return ReactionEquationProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of StoichiometrySumProduction:
 *
 * StochiometrySum =
 *   [INTEGER] Identifier ["+" StochiometrySum];
 * ******************************************************************************************** */
StoichiometrySumProduction::StoichiometrySumProduction()
  : Production()
{
  StoichiometrySumProduction::instance = this;

  // [Integer]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::TokenProduction(T_INTEGER)));

  // Itentifier:
  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));

  // ["+" StoichiometrySum]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production( 2, new Utils::TokenProduction(T_PLUS), this)));
}

StoichiometrySumProduction *StoichiometrySumProduction::instance = 0;

Utils::Production *
StoichiometrySumProduction::get()
{
  if (0 == StoichiometrySumProduction::instance)
    new StoichiometrySumProduction();

  return StoichiometrySumProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of KineticLawProduction:
 *
 * KineticLaw =
 *   Expression [":" LocalParameterDefinitionList];
 * ******************************************************************************************** */
KineticLawProduction::KineticLawProduction()
  : Production()
{
  KineticLawProduction::instance = this;

  this->elements.push_back(ExpressionProduction::get());
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, new Utils::TokenProduction(T_COLON),
            LocalParameterListProduction::get())));
}

KineticLawProduction *KineticLawProduction::instance = 0;

Utils::Production *
KineticLawProduction::get()
{
  if (0 == KineticLawProduction::instance)
    new KineticLawProduction();

  return KineticLawProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of LocalParameterListProduction:
 *
 * LocalParameterList =
 *   Identifier "=" Expression ["," LocalParameterList];
 * ******************************************************************************************** */
LocalParameterListProduction::LocalParameterListProduction()
  : Production()
{
  LocalParameterListProduction::instance = this;

  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new Utils::TokenProduction(T_ASSIGN));
  this->elements.push_back(ExpressionProduction::get());

  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, new Utils::TokenProduction(T_COMMA), this)));
}

LocalParameterListProduction *LocalParameterListProduction::instance = 0;

Utils::Production *
LocalParameterListProduction::get()
{
  if (0 == LocalParameterListProduction::instance)
    new LocalParameterListProduction();

  return LocalParameterListProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of EventDefinitionsProduction:
 *
 * EventDefinitions =
 *   "@events" EOL EventDefinitionList;
 * ******************************************************************************************** */
EventDefinitionsProduction::EventDefinitionsProduction()
  : Production()
{
  EventDefinitionsProduction::instance = this;

  this->elements.push_back(new Utils::TokenProduction(T_EVENTS_KW));
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(EventDefinitionListProduction::get());
}

EventDefinitionsProduction *EventDefinitionsProduction::instance = 0;

Utils::Production *
EventDefinitionsProduction::get()
{
  if (0 == EventDefinitionsProduction::instance)
    new EventDefinitionsProduction();

  return EventDefinitionsProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of EventDefinitionListProduction:
 *
 * EventDefinitionList =
 *   Identifier "=" ConditionExpression [";" Number] ":" AssignmentList [QuotedString]
 *   [EOL EventDefinitionList];
 * ******************************************************************************************** */
EventDefinitionListProduction::EventDefinitionListProduction()
  : Production()
{
  EventDefinitionListProduction::instance = this;

  // Identifier "=" ConditionExpression
  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new Utils::TokenProduction(T_ASSIGN));
  this->elements.push_back(ConditionExpressionProduction::get());

  // [";" Number]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, new Utils::TokenProduction(T_SEMICOLON), NumberProduction::get())));

  // ":" AssignmentList
  this->elements.push_back(new Utils::TokenProduction(T_COLON));
  this->elements.push_back(AssignmentListProduction::get());

  // [QuotedString]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::TokenProduction(T_QUOTED_STRING)));

  // [EOL EventDefinitionList]
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(2, EndOfLineProduction::get(), this)));
}

EventDefinitionListProduction *EventDefinitionListProduction::instance = 0;

Utils::Production *
EventDefinitionListProduction::get()
{
  if (0 == EventDefinitionListProduction::instance)
    new EventDefinitionListProduction();

  return EventDefinitionListProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ConditionExpressionProduction:
 *
 * ConditionExpression =
 *   Identifier ("==" | "!=" | ">" | ">=" | "<" | "<=") Number;
 * ******************************************************************************************** */
ConditionExpressionProduction::ConditionExpressionProduction()
  : Production()
{
  ConditionExpressionProduction::instance = this;

  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(
        new Utils::AltProduction(
          6, new Utils::TokenProduction(T_EQUAL),
          new Utils::TokenProduction(T_NEQUAL),
          new Utils::TokenProduction(T_GREATERTHAN),
          new Utils::TokenProduction(T_GREATEREQUAL),
          new Utils::TokenProduction(T_LESSTHAN),
          new Utils::TokenProduction(T_LESSEQUAL)));
  this->elements.push_back(NumberProduction::get());
}

ConditionExpressionProduction *ConditionExpressionProduction::instance = 0;

Utils::Production *
ConditionExpressionProduction::get()
{
  if (0 == ConditionExpressionProduction::instance)
    new ConditionExpressionProduction();

  return ConditionExpressionProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of AssignmentListProduction:
 *
 * AssignmentList =
 *   Identifier "=" Number [";" AssignmentList];
 * ******************************************************************************************** */
AssignmentListProduction::AssignmentListProduction()
  : Production()
{
  AssignmentListProduction::instance = this;

  this->elements.push_back(new Utils::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new Utils::TokenProduction(T_ASSIGN));
  this->elements.push_back(NumberProduction::get());
  this->elements.push_back(
        new Utils::OptionalProduction(
          new Utils::Production(
            2, new Utils::TokenProduction(T_SEMICOLON), this)));
}

AssignmentListProduction *AssignmentListProduction::instance = 0;

Utils::Production *
AssignmentListProduction::get()
{
  if (0 == AssignmentListProduction::instance)
    new AssignmentListProduction();

  return AssignmentListProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of NumberProduction:
 *
 * Number =
 *   ["-"] (INTEGER | FLOAT);
 * ******************************************************************************************** */
NumberProduction::NumberProduction()
  : Production()
{
  // Register singleton instance:
  NumberProduction::instance = this;

  // Assemble grammar:
  // [-]
  this->elements.push_back(
        new Utils::OptionalProduction(new Utils::TokenProduction(T_MINUS)));

  // (INTEGER | FLOAT)
  this->elements.push_back(
        new Utils::AltProduction(
          2, new Utils::TokenProduction(T_INTEGER), new Utils::TokenProduction(T_FLOAT)));
}

NumberProduction *NumberProduction::instance = 0;

Utils::Production *
NumberProduction::get()
{
  if (0 == NumberProduction::instance)
    new NumberProduction();

  return NumberProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of EndOfLineProduction:
 *
 * EndOfLine =
 *   EOL [EndOfLine];
 * ******************************************************************************************** */
EndOfLineProduction::EndOfLineProduction()
  : Production()
{
  EndOfLineProduction::instance = this;

  this->elements.push_back(new Utils::TokenProduction(T_END_OF_LINE));
  this->elements.push_back(new Utils::OptionalProduction(this));
}

EndOfLineProduction *EndOfLineProduction::instance = 0;

Utils::Production *
EndOfLineProduction::get()
{
  if (0 == EndOfLineProduction::instance)
    new EndOfLineProduction();

  return EndOfLineProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ExpressionProduction:
 *
 * Expression =
 *   (ProductExpression ("+"|"-") Expression) | ProductExpression;
 * ******************************************************************************************** */
ExpressionProduction::ExpressionProduction()
  : AltProduction()
{
  ExpressionProduction::instance = this;

  this->alternatives.resize(2);

  this->alternatives[0] =
      new Utils::Production(
        3, ProductExpressionProduction::get(),
        new Utils::AltProduction(
          2, new Utils::TokenProduction(T_PLUS), new Utils::TokenProduction(T_MINUS)),
        ExpressionProduction::get());

  this->alternatives[1] = ProductExpressionProduction::get();
}

ExpressionProduction *ExpressionProduction::instance = 0;

Utils::Production *
ExpressionProduction::get()
{
  if (0 == ExpressionProduction::instance)
    new ExpressionProduction();

  return ExpressionProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ProductExpressionProduction:
 *
 * ProductExpression =
 *   (AtomicExpression ("*" | "/") ProductExpression) | AtomicExpression;
 * ******************************************************************************************** */
ProductExpressionProduction::ProductExpressionProduction()
  : AltProduction()
{
  ProductExpressionProduction::instance = this;

  this->alternatives.resize(2);

  this->alternatives[0] =
      new Utils::Production(
        3, AtomicExpressionProduction::get(),
        new Utils::AltProduction(
          2, new Utils::TokenProduction(T_TIMES), new Utils::TokenProduction(T_DIVIVE)),
        ProductExpressionProduction::get());

  this->alternatives[1] = AtomicExpressionProduction::get();
}

ProductExpressionProduction *ProductExpressionProduction::instance = 0;

Utils::Production *
ProductExpressionProduction::get()
{
  if (0 == ProductExpressionProduction::instance)
    new ProductExpressionProduction();

  return ProductExpressionProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of AtomicExpressionProduction:
 *
 * AtomicExpression =
 *   Identifier | Number | ("(" Expression ")");
 * ******************************************************************************************** */
AtomicExpressionProduction::AtomicExpressionProduction()
  : AltProduction()
{
  AtomicExpressionProduction::instance = this;

  this->alternatives.resize(3);

  this->alternatives[0] = new Utils::TokenProduction(T_IDENTIFIER);
  this->alternatives[1] = NumberProduction::get();
  this->alternatives[2] = new Utils::Production(
        3, new Utils::TokenProduction(T_LPAR),
        ExpressionProduction::get(),
        new Utils::TokenProduction(T_RPAR));
}

AtomicExpressionProduction *AtomicExpressionProduction::instance = 0;

Utils::Production *
AtomicExpressionProduction::get()
{
  if (0 == AtomicExpressionProduction::instance)
    new AtomicExpressionProduction();

  return AtomicExpressionProduction::instance;
}

