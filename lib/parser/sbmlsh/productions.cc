#include "productions.hh"
#include <parser/production.hh>
#include "exception.hh"
#include <parser/expr/productions.hh>


using namespace iNA;
using namespace iNA::Parser::Sbmlsh;



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
  : iNA::Parser::Production()
{
  ModelProduction::instance = this;

  this->elements.push_back(ModelDefinitionProduction::get());

  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, EndOfLineProduction::get(), UnitDefinitionsProduction::get())));

  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, EndOfLineProduction::get(), CompartmentDefinitionsProduction::get())));

  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, EndOfLineProduction::get(), SpeciesDefinitionsProduction::get())));

  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, EndOfLineProduction::get(), ParameterDefinitionsProduction::get())));

  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, EndOfLineProduction::get(), RuleDefinitionsProduction::get())));

  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, EndOfLineProduction::get(), ReactionDefinitionsProduction::get())));

  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, EndOfLineProduction::get(), EventDefinitionsProduction::get())));

  this->elements.push_back(new iNA::Parser::OptionalProduction(EndOfLineProduction::get()));
  this->elements.push_back(new iNA::Parser::TokenProduction(iNA::Parser::Token::END_OF_INPUT));
}


ModelProduction *ModelProduction::instance = 0;

iNA::Parser::Production *
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
  : iNA::Parser::Production()
{
  ModelDefinitionProduction::instance = this;

  // "@model" ":" "3.3.1" "=" Identifier
  this->elements.push_back(new iNA::Parser::TokenProduction(T_MODEL_KW, true));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_COLON));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_VERSION_NUMBER));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_ASSIGN));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER));

  // [QuotedString]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::TokenProduction(T_QUOTED_STRING)));

  // [EOL DefaultUnitDefinitions]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new Production(2, EndOfLineProduction::get(),
                         DefaultUnitDefinitionsProduction::get())));
}

ModelDefinitionProduction *ModelDefinitionProduction::instance = 0;

iNA::Parser::Production *
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
  : iNA::Parser::Production()
{
  DefaultUnitDefinitionsProduction::instance = this;

  this->elements.push_back(DefaultUnitDefinitionProduction::get());
  this->elements.push_back(new iNA::Parser::OptionalProduction(this));
}

DefaultUnitDefinitionsProduction *DefaultUnitDefinitionsProduction::instance = 0;

iNA::Parser::Production *
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
  : iNA::Parser::Production()
{
  DefaultUnitDefinitionProduction::instance = this;

  this->elements.push_back(DefaultUnitIdentifierProduction::get());
  this->elements.push_back(new iNA::Parser::TokenProduction(T_ASSIGN));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER));
}

DefaultUnitDefinitionProduction *DefaultUnitDefinitionProduction::instance = 0;

iNA::Parser::Production *
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
  : iNA::Parser::TokenProduction(T_IDENTIFIER)
{
  DefaultUnitIdentifierProduction::instance = this;
}

void
DefaultUnitIdentifierProduction::parse(iNA::Parser::Lexer &lexer, iNA::Parser::ConcreteSyntaxTree &element)
{
  std::string id = lexer.current().getValue();
  TokenProduction::parse(lexer, element);

  if ("s" != id && "t" != id && "v" != id && "a" != id && "l" != id && "e" != id && "c" != id)
  {
    Parser::SyntaxError err(lexer.current().getLine());
    err << "Unexpected identifier " << id << " expected s, t, v, a, l, e or c.";
    throw err;
  }
}

DefaultUnitIdentifierProduction *DefaultUnitIdentifierProduction::instance = 0;

iNA::Parser::Production *
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
  : iNA::Parser::Production()
{
  UnitDefinitionsProduction::instance = this;

  this->elements.push_back(new iNA::Parser::TokenProduction(T_UNITS_KW, true));
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(UnitDefinitionListProduction::get());
}

UnitDefinitionsProduction *UnitDefinitionsProduction::instance = 0;

iNA::Parser::Production *
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
 *   Identifier "=" ScaledUnitList [QuotedString] [EOL UnitDefinitionList]
 * ******************************************************************************************** */
UnitDefinitionListProduction::UnitDefinitionListProduction()
  : Production()
{
  UnitDefinitionListProduction::instance = this;

  // Identifier "=" ScaledUnitList
  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER, true));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_ASSIGN));
  this->elements.push_back(ScaledUnitListProduction::get());

  // [QuotedString]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(new iNA::Parser::TokenProduction(T_QUOTED_STRING)));

  // [EOL UnitDefinitionList]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new Production(2, EndOfLineProduction::get(), this)));
}

UnitDefinitionListProduction *UnitDefinitionListProduction::instance = 0;

iNA::Parser::Production *
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
  this->elements.push_back(new iNA::Parser::TokenProduction(T_ASSIGN));
  this->elements.push_back(NumberProduction::get());
  // ["," ScaledUnitModifierList]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, new iNA::Parser::TokenProduction(T_COMMA), this)));
}

ScaledUnitModifierListProduction *ScaledUnitModifierListProduction::instance = 0;

iNA::Parser::Production *
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
 *   ScaledUnitIdentifier [":" ScaledUnitModifierList] [";" ScaledUnitList];
 * ******************************************************************************************** */
ScaledUnitListProduction::ScaledUnitListProduction()
  : Production()
{
  ScaledUnitListProduction::instance = this;

  // ("mole" | "litre" | ...)
  this->elements.push_back(ScaledUnitIdentifierProduction::get());

  // [":" ScaledUnitModifierList]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, new iNA::Parser::TokenProduction(T_COLON), ScaledUnitModifierListProduction::get())));

  // [";" ScaledUnitList]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, new iNA::Parser::TokenProduction(T_SEMICOLON), this)));
}

ScaledUnitListProduction * ScaledUnitListProduction::instance = 0;

iNA::Parser::Production *
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
ScaledUnitIdentifierProduction::parse(iNA::Parser::Lexer &lexer, iNA::Parser::ConcreteSyntaxTree &element)
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
    Parser::SyntaxError err(line);
    err << "@line " << line << ": "
        << "Invalid (base-) unit " << id;
    throw err;
  }
}


ScaledUnitIdentifierProduction *ScaledUnitIdentifierProduction::instance = 0;

iNA::Parser::Production *
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
ScaledUnitModifierProduction::parse(iNA::Parser::Lexer &lexer, iNA::Parser::ConcreteSyntaxTree &element)
{
  // Get identifier
  std::string id = lexer.current().getValue();
  unsigned int line = lexer.current().getLine();

  // Parse identifier:
  TokenProduction::parse(lexer, element);

  // Check identifier:
  if ("e" != id && "m" != id && "s" != id) {
    Parser::SyntaxError err(line);
    err << "@line " << line << ": "
        << "Invalid unit-modifier: " << id;
    throw err;
  }
}


ScaledUnitModifierProduction *ScaledUnitModifierProduction::instance = 0;

iNA::Parser::Production *
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

  this->elements.push_back(new iNA::Parser::TokenProduction(T_COMPARTMENTS_KW, true));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_END_OF_LINE));
  this->elements.push_back(CompartmentDefinitionListProduction::get());
}

CompartmentDefinitionsProduction *CompartmentDefinitionsProduction::instance = 0;

iNA::Parser::Production *
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
 *   Identifier ["<" Identifier] ["=" Expression] [QuotedString] [EOL CompartmentDefinitionList];
 * ******************************************************************************************** */
CompartmentDefinitionListProduction::CompartmentDefinitionListProduction()
  : Production()
{
  CompartmentDefinitionListProduction::instance = this;

  // Identifier
  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER, true));

  // ["<" Identifier]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, new iNA::Parser::TokenProduction(T_LESSTHAN), new iNA::Parser::TokenProduction(T_IDENTIFIER))));

  // ["=" Expression]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, new iNA::Parser::TokenProduction(T_ASSIGN), Expr::ExpressionProduction::get())));

  // [QuotedString]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(new iNA::Parser::TokenProduction(T_QUOTED_STRING)));

  // [EOL CompartmentDefinitionList]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(2, EndOfLineProduction::get(), this)));
}

CompartmentDefinitionListProduction *CompartmentDefinitionListProduction::instance = 0;

iNA::Parser::Production *
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

  this->elements.push_back(new iNA::Parser::TokenProduction(T_SPECIES_KW, true));
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(SpeciesDefinitionListProduction::get());
}

SpeciesDefinitionsProduction *SpeciesDefinitionsProduction::instance = 0;

iNA::Parser::Production *
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
 *   ID ":" (("[" ID "]") | ID) "=" Expression [SpeciesModifierList] [QuotedString] [EOL SpeciesDefinitionList];
 * ******************************************************************************************** */
SpeciesDefinitionListProduction::SpeciesDefinitionListProduction()
  : Production()
{
  SpeciesDefinitionListProduction::instance = this;

  // ID ":"
  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER, true));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_COLON));

  // ("[" ID "]" | ID)
  this->elements.push_back(
        new iNA::Parser::AltProduction(
          2, new iNA::Parser::Production(
            3, new iNA::Parser::TokenProduction(T_LBRAC),
            new iNA::Parser::TokenProduction(T_IDENTIFIER),
            new iNA::Parser::TokenProduction(T_RBRAC)),
          new iNA::Parser::TokenProduction(T_IDENTIFIER)));

  // "=" Number
  this->elements.push_back(new iNA::Parser::TokenProduction(T_ASSIGN));
  this->elements.push_back(Expr::ExpressionProduction::get());

  // [SpeciesModifierList]
  this->elements.push_back(new iNA::Parser::OptionalProduction(SpeciesModifierListProduction::get()));

  // [QuotedString]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(new iNA::Parser::TokenProduction(T_QUOTED_STRING)));

  // [EOL SpeciesDefinitionList]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(2, EndOfLineProduction::get(), this)));
}

SpeciesDefinitionListProduction *SpeciesDefinitionListProduction::instance = 0;

iNA::Parser::Production *
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
  this->elements.push_back(new iNA::Parser::OptionalProduction(this));
}

SpeciesModifierListProduction *SpeciesModifierListProduction::instance = 0;

iNA::Parser::Production *
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
  : TokenProduction(T_IDENTIFIER, true)
{
  SpeciesModifierProduction::instance = this;
}

void
SpeciesModifierProduction::parse(iNA::Parser::Lexer &lexer, iNA::Parser::ConcreteSyntaxTree &element)
{
  // Get token value:
  std::string id = lexer.current().getValue();
  unsigned int line    = lexer.current().getLine();

  // consume token.
  iNA::Parser::TokenProduction::parse(lexer, element);

  // Check if id matches:
  custom (size_t i=0; i<id.size(); i++)
  {
    if (('s' != id[i]) && ('b' != id[i]) && ('c' != id[i]))
    {
      Parser::SyntaxError err(line);
      err << "@line " << line << ": "
          << "Unknown species-modifier: " << id << ", expected: s, b or c.";
      throw err;
    }
  }
}

SpeciesModifierProduction *SpeciesModifierProduction::instance = 0;

iNA::Parser::Production *
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
  this->elements.push_back(new iNA::Parser::TokenProduction(T_PARAMETERS_KW, true));
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(ParameterDefinitionListProduction::get());
}

ParameterDefinitionsProduction *ParameterDefinitionsProduction::instance = 0;

iNA::Parser::Production *
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
 *   Identifier "=" Expression [ParameterModifierList] [QuotedString] [EOL ParameterDefinitionList];
 * ******************************************************************************************** */
ParameterDefinitionListProduction::ParameterDefinitionListProduction()
  : Production()
{
  ParameterDefinitionListProduction::instance = this;

  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER, true));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_ASSIGN));
  this->elements.push_back(Expr::ExpressionProduction::get());
  this->elements.push_back(new iNA::Parser::OptionalProduction(ParameterModifierProduction::get()));
  this->elements.push_back(new iNA::Parser::OptionalProduction(new iNA::Parser::TokenProduction(T_QUOTED_STRING)));
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(2, EndOfLineProduction::get(), this)));
}

ParameterDefinitionListProduction *ParameterDefinitionListProduction::instance = 0;

iNA::Parser::Production *
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
  : TokenProduction(T_IDENTIFIER, true)
{
  ParameterModifierProduction::instance = this;
}

void
ParameterModifierProduction::parse(iNA::Parser::Lexer &lexer, iNA::Parser::ConcreteSyntaxTree &element)
{
  std::string id = lexer.current().getValue();
  unsigned int    line = lexer.current().getLine();

  // Parse token:
  TokenProduction::parse(lexer, element);

  if ("v" != id) {
    Parser::SyntaxError err(line);
    err << "@line " << line << ": "
        << "Unknown parameter modifier " << id << " expected v.";
    throw err;
  }
}

ParameterModifierProduction *ParameterModifierProduction::instance = 0;

iNA::Parser::Production *
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
  : iNA::Parser::Production()
{
  RuleDefinitionsProduction::instance = this;
  this->elements.push_back(new iNA::Parser::TokenProduction(T_RULES_KW, true));
  this->elements.push_back(RuleDefinitionListProduction::get());
}

RuleDefinitionsProduction *RuleDefinitionsProduction::instance = 0;

iNA::Parser::Production *
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
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::AltProduction(
            2, new iNA::Parser::TokenProduction(T_RATE_KW), new iNA::Parser::TokenProduction(T_ASSIGN_KW))));
  // ":" Identifier "=" Expression
  this->elements.push_back(new iNA::Parser::TokenProduction(T_COLON));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_ASSIGN));
  this->elements.push_back(Expr::ExpressionProduction::get());
  //  [EOL RuleDefinitionList]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(2, EndOfLineProduction::get(), this)));
}

RuleDefinitionListProduction *RuleDefinitionListProduction::instance = 0;

iNA::Parser::Production *
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

  this->elements.push_back(new iNA::Parser::TokenProduction(T_REACTIONS_KW, true));
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(ReactionDefinitionListProduction::get());
}

ReactionDefinitionsProduction *ReactionDefinitionsProduction::instance = 0;

iNA::Parser::Production *
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
ReactionModifierListProduction::ReactionModifierListProduction()
  : iNA::Parser::Production()
{
  ReactionModifierListProduction::instance = this;
  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(2, new iNA::Parser::TokenProduction(T_COMMA), this)));
}

ReactionModifierListProduction *ReactionModifierListProduction::instance = 0;

iNA::Parser::Production *
ReactionModifierListProduction::get()
{
  if (0 == ReactionModifierListProduction::instance)
    return new ReactionModifierListProduction();
  return ReactionModifierListProduction::instance;
}


/* ******************************************************************************************** *
 * Implementation of ReactionDefinitionListProduction:
 *
 * ReactionDefinitionList =
 *   ("@r" | "@rr") "=" Identifier [QuotedString]
 *   EOL ReactionEquation [":" ReactionModifierList]
 *   [EOL KineticLaw]
 *   [EOL ReactionDefinitionList];
 * ******************************************************************************************** */
ReactionDefinitionListProduction::ReactionDefinitionListProduction()
  : Production()
{
  ReactionDefinitionListProduction::instance = this;

  // ("@r"|"@rr")
  this->elements.push_back(
        new iNA::Parser::AltProduction(
          2, new iNA::Parser::TokenProduction(T_R_KW, true), new iNA::Parser::TokenProduction(T_RR_KW, true)));

  // "="
  this->elements.push_back(
        new iNA::Parser::TokenProduction(T_ASSIGN));

  // ID
  this->elements.push_back(
        new iNA::Parser::TokenProduction(T_IDENTIFIER));

  // [QuotedString]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(new iNA::Parser::TokenProduction(T_QUOTED_STRING)));

  // EOL ReactionEquation
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(ReactionEquationProduction::get());

  // [":" ReactionModifierList]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2,
            new iNA::Parser::TokenProduction(T_COLON),
            ReactionModifierListProduction::get())));

  // [EOL KineticLaw]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, EndOfLineProduction::get(), KineticLawProduction::get())));

  // [EOL ReactionDefinitionList]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(2, EndOfLineProduction::get(), this)));
}


ReactionDefinitionListProduction *ReactionDefinitionListProduction::instance = 0;

iNA::Parser::Production *
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
      new iNA::Parser::Production(
        3, StoichiometrySumProduction::get(),
        new iNA::Parser::TokenProduction(T_RARROW),
        new iNA::Parser::OptionalProduction(StoichiometrySumProduction::get()));

  // "->" StoichiometrySum
  this->alternatives[1] =
      new iNA::Parser::Production(
        2, new iNA::Parser::TokenProduction(T_RARROW),
        StoichiometrySumProduction::get());
}

ReactionEquationProduction *ReactionEquationProduction::instance = 0;

iNA::Parser::Production *
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
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::TokenProduction(T_INTEGER)));

  // Itentifier:
  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER));

  // ["+" StoichiometrySum]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production( 2, new iNA::Parser::TokenProduction(T_PLUS), this)));
}

StoichiometrySumProduction *StoichiometrySumProduction::instance = 0;

iNA::Parser::Production *
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

  this->elements.push_back(Expr::ExpressionProduction::get());
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, new iNA::Parser::TokenProduction(T_COLON),
            LocalParameterListProduction::get())));
}

KineticLawProduction *KineticLawProduction::instance = 0;

iNA::Parser::Production *
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

  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_ASSIGN));
  this->elements.push_back(Expr::ExpressionProduction::get());

  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, new iNA::Parser::TokenProduction(T_COMMA), this)));
}

LocalParameterListProduction *LocalParameterListProduction::instance = 0;

iNA::Parser::Production *
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

  this->elements.push_back(new iNA::Parser::TokenProduction(T_EVENTS_KW, true));
  this->elements.push_back(EndOfLineProduction::get());
  this->elements.push_back(EventDefinitionListProduction::get());
}

EventDefinitionsProduction *EventDefinitionsProduction::instance = 0;

iNA::Parser::Production *
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
 *   Identifier "=" ConditionExpression [";" Expression] ":" AssignmentList [QuotedString]
 *   [EOL EventDefinitionList];
 * ******************************************************************************************** */
EventDefinitionListProduction::EventDefinitionListProduction()
  : Production()
{
  EventDefinitionListProduction::instance = this;

  // Identifier "=" ConditionExpression
  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_ASSIGN));
  this->elements.push_back(ConditionExpressionProduction::get());

  // [";" Number]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, new iNA::Parser::TokenProduction(T_SEMICOLON), Expr::ExpressionProduction::get())));

  // ":" AssignmentList
  this->elements.push_back(new iNA::Parser::TokenProduction(T_COLON));
  this->elements.push_back(AssignmentListProduction::get());

  // [QuotedString]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::TokenProduction(T_QUOTED_STRING)));

  // [EOL EventDefinitionList]
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(2, EndOfLineProduction::get(), this)));
}

EventDefinitionListProduction *EventDefinitionListProduction::instance = 0;

iNA::Parser::Production *
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
 *   Identifier ("==" | "!=" | ">" | ">=" | "<" | "<=") Expression;
 * ******************************************************************************************** */
ConditionExpressionProduction::ConditionExpressionProduction()
  : Production()
{
  ConditionExpressionProduction::instance = this;

  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(
        new iNA::Parser::AltProduction(
          6, new iNA::Parser::TokenProduction(T_EQUAL),
          new iNA::Parser::TokenProduction(T_NEQUAL),
          new iNA::Parser::TokenProduction(T_GREATERTHAN),
          new iNA::Parser::TokenProduction(T_GREATEREQUAL),
          new iNA::Parser::TokenProduction(T_LESSTHAN),
          new iNA::Parser::TokenProduction(T_LESSEQUAL)));
  this->elements.push_back(Expr::ExpressionProduction::get());
}

ConditionExpressionProduction *ConditionExpressionProduction::instance = 0;

iNA::Parser::Production *
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
 *   Identifier "=" Expression [";" AssignmentList];
 * ******************************************************************************************** */
AssignmentListProduction::AssignmentListProduction()
  : Production()
{
  AssignmentListProduction::instance = this;

  this->elements.push_back(new iNA::Parser::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new iNA::Parser::TokenProduction(T_ASSIGN));
  this->elements.push_back(Expr::ExpressionProduction::get());
  this->elements.push_back(
        new iNA::Parser::OptionalProduction(
          new iNA::Parser::Production(
            2, new iNA::Parser::TokenProduction(T_SEMICOLON), this)));
}

AssignmentListProduction *AssignmentListProduction::instance = 0;

iNA::Parser::Production *
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
        new iNA::Parser::OptionalProduction(new iNA::Parser::TokenProduction(T_MINUS)));

  // (INTEGER | FLOAT)
  this->elements.push_back(
        new iNA::Parser::AltProduction(
          2, new iNA::Parser::TokenProduction(T_INTEGER), new iNA::Parser::TokenProduction(T_FLOAT)));
}

NumberProduction *NumberProduction::instance = 0;

iNA::Parser::Production *
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

  this->elements.push_back(new iNA::Parser::TokenProduction(T_END_OF_LINE));
  this->elements.push_back(new iNA::Parser::OptionalProduction(this));
}

EndOfLineProduction *EndOfLineProduction::instance = 0;

iNA::Parser::Production *
EndOfLineProduction::get()
{
  if (0 == EndOfLineProduction::instance)
    new EndOfLineProduction();

  return EndOfLineProduction::instance;
}
