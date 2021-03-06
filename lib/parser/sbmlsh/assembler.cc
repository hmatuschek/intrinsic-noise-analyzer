#include "assembler.hh"
#include "ast/model.hh"
#include "ast/reaction.hh"
#include "parser/expr/assembler.hh"


using namespace iNA;
using namespace iNA::Parser::Sbmlsh;


Assembler::Assembler(Ast::Model &model, Parser::Lexer &lexer)
  : _lexer(lexer), _model(model)
{
  // Assemble base unit map;
  _base_unit_map["ampere"] = Ast::Unit::AMPERE;
  _base_unit_map["becquerel"] = Ast::Unit::BECQUEREL;
  _base_unit_map["candela"] = Ast::Unit::CANDELA;
  _base_unit_map["coulomb"] = Ast::Unit::COULOMB;
  _base_unit_map["dimensionless"] = Ast::Unit::DIMENSIONLESS;
  _base_unit_map["farad"] = Ast::Unit::FARAD;
  _base_unit_map["gram"] = Ast::Unit::GRAM;
  _base_unit_map["katal"] = Ast::Unit::KATAL;
  _base_unit_map["gray"] = Ast::Unit::GRAY;
  _base_unit_map["kelvin"] = Ast::Unit::KELVIN;
  _base_unit_map["henry"] = Ast::Unit::HENRY;
  _base_unit_map["kilogram"] = Ast::Unit::KILOGRAM;
  _base_unit_map["hertz"] = Ast::Unit::HERTZ;
  _base_unit_map["litre"] = Ast::Unit::LITRE;
  _base_unit_map["item"] = Ast::Unit::ITEM;
  _base_unit_map["lumen"] = Ast::Unit::LUMEN;
  _base_unit_map["joule"] = Ast::Unit::JOULE;
  _base_unit_map["lux"] = Ast::Unit::LUX;
  _base_unit_map["metre"] = Ast::Unit::METRE;
  _base_unit_map["mole"] = Ast::Unit::MOLE;
  _base_unit_map["newton"] = Ast::Unit::NEWTON;
  _base_unit_map["ohm"] = Ast::Unit::OHM;
  _base_unit_map["pascal"] = Ast::Unit::PASCAL;
  _base_unit_map["radian"] = Ast::Unit::RADIAN;
  _base_unit_map["second"] = Ast::Unit::SECOND;
  _base_unit_map["watt"] = Ast::Unit::WATT;
  _base_unit_map["siemens"] = Ast::Unit::SIEMENS;
  _base_unit_map["weber"] = Ast::Unit::WEBER;
  _base_unit_map["sievert"] = Ast::Unit::SIEVERT;
  _base_unit_map["steradian"] = Ast::Unit::STERADIAN;
  _base_unit_map["tesla"] = Ast::Unit::TESLA;
  _base_unit_map["volt"] = Ast::Unit::VOLT;
}


void
Assembler::process(Parser::ConcreteSyntaxTree &root)
{
  processModel(root);
}



void
Assembler::processModel(Parser::ConcreteSyntaxTree &model)
{
  /* Model =                          : model
   *   ModelDefinition                  : model[0]
   *   [EOL UnitDefinitions]            : model[1]
   *   [EOL CompartmentDefinitions]     : model[2]
   *   [EOL SpeciesDefinitions]         : model[3]
   *   [EOL ParameterDefinitions]       : model[4]
   *   [EOL RuleDefinitions]            : model[5]
   *   [EOL ReactionDefinitions]        : model[6]
   *   [EOL EventDefinitions];          : model[7];
   */

  // There must be a model definition @ index 0:
  processModelDefinition(model[0]);

  // If there are UnitDefinitions...
  if (model[1].matched()) {
    // model[1][0]      := EOL UnitDefinitions;
    // model[1][0][1]    = UnitDefinitions := "@units" EOL UnitDefinitionList
    // model[1][0][1][2] = UnitDefinitionList;
    processUnitDefinition(model[1][0][1][2]);
  }

  // If there are parameter definitions...
  if (model[4].matched()) {
    /* EOL ParameterDefinitions       : model[4][0]
     * ParameterDefinitions =         : model[4][0][1]
     *   "@parameters"
     *   EOL
     *   ParameterDefinitionList        : model[4][0][1][2] */
    processParameterDefinition(model[4][0][1][2]);
  }

  // If there are Compartment definitions ...
  if (model[2].matched()) {
    // EOL CompartmentDefinitions  : model[2][0]
    // CompartmentDefinitions      : model[2][0][1]
    // "@compartments"               : model[2][0][1][0]
    // EOL
    // CompartmentDefinitionList;    : model[2][0][1][2]
    processCompartmentDefinitions(model[2][0][1][2]);
  }

  // If this flag is true, all species are defined in substance units...
  bool allSpeciesHaveSubstanceUnits = model[3].matched();
  // If there are species definitions...
  if (model[3].matched()) {
    // EOL SpeciesDefinitions      : model[3][0]
    // SpeciesDefinitions          : model[3][0][1]
    //   "@species"                  : model[3][0][1][0]
    //   EOL
    //   SpeciesDefinitionList       : model[3][0][1][2]
    processSpeciesDefinition(model[3][0][1][2], allSpeciesHaveSubstanceUnits);
  }

  // If all species are defined in substance units, the model too...
  _model.setSpeciesHaveSubstanceUnits(allSpeciesHaveSubstanceUnits);

  // Do not process Rule productions...
  if (model[5].matched()) {
    // EOL RuleDefinitions         : model[5][0]
    // RuleDefinitions    =        : model[5][0][1]
    //   '@rules'                    : model[5][0][1][0]
    //   EOL
    //   RuleDefinitionList          : model[5][0][1][2]
    processRuleDefinitionList(model[5][0][1][2]);
  }

  // If there are reaction definitions...
  if (model[6].matched()) {
    /* EOL ReactionDefinitions        : model[5][0]
     * ReactionDefinitions            : model[5][0][1]
     *  "@reactions"
     *  EOL
     *  ReactionDefinitionList          : model[5][0][1][2]  */
    processReactionDefinitions(model[6][0][1][2]);
  }

  // Do not process event definitions...
  if (model[7].matched()) {
    throw SBMLFeatureNotSupported("Cannot parse SBML-SH: Event definitions are not supported yet.");
  }

  // Once the model was created, link all expressions of the model
  linkModel(model);
}


void
Assembler::linkModel(ConcreteSyntaxTree &model)
{
  // Some species might be scaled once the model is parsed to match units. This can only be
  // perfromed on the final model.
  Trafo::VariableScaling scaleing;

  // If there are parameter definitions...
  if (model[4].matched()) {
    // link initial values of parameters:
    linkParameterDefinition(model[4][0][1][2]);
  }

  // If there are Compartment definitions ...
  if (model[2].matched()) {
    // Link initial values of compartments
    linkCompartmentDefinitions(model[2][0][1][2]);
  }

  // If there are species definitions...
  if (model[3].matched()) {
    linkSpeciesDefinition(model[3][0][1][2], scaleing);
  }

  // If there are reaction definitions...
  if (model[6].matched()) {
    linkReactionDefinitions(model[6][0][1][2]);
  }

  // Now scale the species:
  _model.apply(scaleing);
}


void
Assembler::processModelDefinition(Parser::ConcreteSyntaxTree &model_header)
{
  /* ModelDefinition =               : model_header
   *   "@model"                        : model_header[0]
   *   ":"
   *   "3.1.1"
   *   "="
   *   Identifier                      : model_header[4]
   *   [QuotedString]                  : model_header[5]
   *   [EOL DefaultUnitDefinitions];   : model_header[6] */

  // Set identifier
  _model.setIdentifier(_lexer[model_header[4].getTokenIdx()].getValue());

  // Set name if defined
  if (model_header[5].matched()) {
    std::string name = _lexer[model_header[5][0].getTokenIdx()].getValue();
    unquote(name); _model.setName(name);
  }

  // Handle default units if defined:
  if (model_header[6].matched()) {
    processDefaultUnitDefinitions(model_header[6][0][1]);
  }
}


void
Assembler::processDefaultUnitDefinitions(Parser::ConcreteSyntaxTree &def_units)
{
  /* DefaultUnitDefinitions =   : def_units
   *   DefaultUnitDefinition      : def_units[0]
   *   [DefaultUnitDefinitions];  : def_units[1];
   *
   * DefaultUnitDefinition =    : def_units[0]
   * DefaultUnitIdentifier        : def_units[0][0]  (token)
   * "="
   * Identifier;                  : def_units[0][2]; (token) */

  std::string def_unit_id = _lexer[def_units[0][0].getTokenIdx()].getValue();
  std::string unit_id     = _lexer[def_units[0][2].getTokenIdx()].getValue();

  // Get unit from identifier def_units[0][2]
  std::map<std::string, Ast::Unit::BaseUnit>::iterator item = _base_unit_map.find(unit_id);
  if (_base_unit_map.end() == item) {
    SBMLParserError err;
    err << "Cannot parse SBML-SH: Unknown unit name: " << unit_id;
    throw err;
  }
  Ast::Unit base_unit(item->second, 1,0,1);

  // Dispatch by flag...
  if (def_unit_id == "s") {
    _model.setSubstanceUnit(base_unit, false);
  } else if (def_unit_id == "t") {
    _model.setTimeUnit(base_unit, false);
  } else if (def_unit_id == "v") {
    _model.setVolumeUnit(base_unit, false);
  } else if (def_unit_id == "a") {
    _model.setAreaUnit(base_unit, false);
  } else if (def_unit_id == "l") {
    _model.setLengthUnit(base_unit, false);
  } else if (def_unit_id == "e") {
    throw SBMLFeatureNotSupported("Setting the extent unit is not supported yet.");
  } else if (def_unit_id == "c") {
    throw SBMLFeatureNotSupported("Setting the conversion factor is not supported yet.");
  }

  // Handle further default unit definitions...
  if (def_units[1].matched()) {
    processDefaultUnitDefinitions(def_units[1][0]);
  }
}


void
Assembler::processUnitDefinition(Parser::ConcreteSyntaxTree &unit_prod)
{
  // UnitDefinitionList =          : unit
  //   Identifier                    :unit[0]  (Token)
  //   "="
  //   ScaledUnitList                :unit[2]
  //   [QuotedString]                :unit[3]; unit[3][0] -> QuotedString
  //   [EOL UnitDefinitionList]      :unit[4]; 4.0.1

  // Collect scaled base units for this unit:
  Ast::Unit unit;
  processScaledUnitList(unit_prod[2], unit);

  std::string identifier = _lexer[unit_prod[0].getTokenIdx()].getValue();

  if ("substance" == identifier) {
    _model.setSubstanceUnit(unit, false);
  } else if ("volume" == identifier) {
    _model.setVolumeUnit(unit, false);
  } else if ("area" == identifier) {
    _model.setAreaUnit(unit, false);
  } else if ("length" == identifier) {
    _model.setLengthUnit(unit, false);
  } else if ("time" == identifier) {
    _model.setTimeUnit(unit, false);
  } else {
    defineUnit(identifier, unit);
  }

  // If there are some unit definitions left
  if (unit_prod[4].matched()) {
    processUnitDefinition(unit_prod[4][0][1]);
  }
}


void
Assembler::processScaledUnitList(Parser::ConcreteSyntaxTree &unit_prod, Ast::Unit &unit)
{
  // ScaledUnitList =                      : units
  //   ScaledUnitIdentifier                  : units[0] (Token)
  //   [                                     : units[1]
  //     ":"                                   : units[1][0][0]
  //     ScaledUnitModifierList]               : units[1][0][1]
  //   [";" ScaledUnitList];                   : units[2] 2,0,1

  /* Dispatch by base unit name. */
  const Parser::Token &uid_token = _lexer[unit_prod[0].getTokenIdx()];
  std::string base_unit_id = uid_token.getValue();
  std::map<std::string, Ast::Unit::BaseUnit>::iterator item
      = _base_unit_map.find(base_unit_id);

  if (_base_unit_map.end() == item) {
    SBMLParserError err;
    err << "Cannot parse SBML-SH @ line "
        << uid_token.getLine() << ": Unknown unit: " << base_unit_id;
    throw err;
  }
  Ast::Unit::BaseUnit base_unit = item->second;

  /* Handle unit modifiers. */
  double multiplier = 1; int scale = 0; int exponent = 1;
  if (unit_prod[1].matched()) {
    processScaledUnitModifierList(unit_prod[1][0][1], multiplier, scale, exponent);
  }

  // Assemble scaled base unit:
  unit = unit*Ast::Unit(base_unit, multiplier, scale, exponent);

  // If there are scaled units left:
  if (unit_prod[2].matched()) {
    processScaledUnitList(unit_prod[2][0][1], unit);
  }
}


void
Assembler::processScaledUnitModifierList(Parser::ConcreteSyntaxTree &sulist,
                                         double &multiplier, int &scale, int &exponent)
{
  /* ScaledUnitModifierList =               : sulist
   *   ('m'|'s'|'e')                          : sulist[0]
   *   '='
   *   NUMBER                                 : sulist[2]
   *   [                                      : sulist[3]
   *     ','                                    : sulist[3][0][0]
   *     ScaledUnitModifierList]                : sulist[3][0][1]   */

  const Parser::Token &flag = _lexer[sulist[0].getTokenIdx()];
  Parser::Expr::ScopeContext context(&_model);
  Parser::Expr::Assembler expr_assembler(context, _lexer);
  if ("m" == flag.getValue()) {
    multiplier = expr_assembler.processNumber(sulist[2]).to_double();
  } else if ("s" == flag.getValue()) {
    scale = expr_assembler.processNumber(sulist[2]).to_int();
  } else if ("e" == flag.getValue()) {
    exponent = expr_assembler.processNumber(sulist[2]).to_int();
  } else {
    SBMLParserError err;
    err << "@line " << flag.getLine()
        << ": Unexpected scaled unit flag " << flag.getValue();
    throw err;
  }

  // Handle remaining flags if there are some left:
  if (sulist[3].matched()) {
    processScaledUnitModifierList(sulist[3][0][1], multiplier, scale, exponent);
  }
}


void
Assembler::processCompartmentDefinitions(Parser::ConcreteSyntaxTree &comp)
{
  /* CompartmentDefinitionList =        : comp
   *  Identifier                          : comp[0]
   *  ["<" Identifier]                    : comp[1]
   *  ["=" Expression]                    : comp[2], Expression: comp[2][0][1]
   *  [QuotedString]                      : comp[3]  (token)
   *  [EOL CompartmentDefinitionList];    : comp[4] */

  std::string id = _lexer[comp[0].getTokenIdx()].getValue();
  Ast::Compartment *compartment = new Ast::Compartment(id, Ast::Compartment::VOLUME, true);

  // Handle inner/outer relation if needed:
  if (comp[1].matched()) {
    SBMLFeatureNotSupported err;
    err << "Can not define inner/outer relation for compartment " << id << ": feature not supported.";
    throw err;
  }

  // Skip initial values, will be processes in linkCompartmentDefinitions.

  // Set name if defined:
  if (comp[3].matched()) {
    std::string name = _lexer[comp[3][0].getTokenIdx()].getValue();
    unquote(name); compartment->setName(name);
  }

  // Add compartment to model:
  _model.addDefinition(compartment);

  // Process remaining compartments
  if (comp[4].matched()) {
    processCompartmentDefinitions(comp[4][0][1]);
  }
}


void
Assembler::linkCompartmentDefinitions(Parser::ConcreteSyntaxTree &comp)
{
  std::string id = _lexer[comp[0].getTokenIdx()].getValue();
  Ast::Compartment *compartment = _model.getCompartment(id);

  // Handle initial value (if defined)
  if (comp[2].matched()) {
    Parser::Expr::ScopeContext context(&_model);
    Parser::Expr::Assembler expr_assembler(context, _lexer);
    compartment->setValue(expr_assembler.processExpression(comp[2][0][1]));
  }

  // Process remaining compartments
  if (comp[4].matched()) {
    linkCompartmentDefinitions(comp[4][0][1]);
  }
}


void
Assembler::processSpeciesDefinition(Parser::ConcreteSyntaxTree &spec, bool &speciesHasSubstanceUnits)
{
  /* SpeciesDefinitionList =           : spec
   *   ID                                : spec[0]
   *   ":"                               : spec[1]
   *   (("[" ID "]") | ID)               : spec[2]
   *     "[" ID "]"                        : spec[2][0][1] -> ID
   *     ID                                : spec[2][0]    -> ID
   *   "="                               : spec[3]
   *   Expression                        : spec[4]
   *   [SpeciesModifierList]             : spec[5]
   *   [QuotedString]                    : spec[6]
   *   [EOL SpeciesDefinitionList];      : spec[7]
   */

  std::string compartment_id = _lexer[spec[0].getTokenIdx()].getValue();
  std::string identifier;
  bool has_substance_units=false; bool has_boundary_condition=false; bool is_constant=false;

  Ast::Compartment *compartment = _model.getCompartment(compartment_id);
  if (0 == spec[2].getAltIdx()) {
    identifier = _lexer[spec[2][0][1].getTokenIdx()].getValue();
  } else {
    identifier = _lexer[spec[2][0].getTokenIdx()].getValue();
  }

  // Parse modifier:
  if (spec[5].matched()) {
    processSpeciesModifierList(spec[5][0], has_substance_units, has_boundary_condition, is_constant);
  }

  // Check species modifier:
  if (has_boundary_condition) {
    SBMLFeatureNotSupported err;
    err << "SBML feature 'boudary condition' for species is not implemented yet.";
    throw err;
  }

  speciesHasSubstanceUnits &= has_substance_units;

  // Create species:
  Ast::Species *species = new Ast::Species(identifier, compartment, is_constant);

  // Handle name
  if (spec[6].matched()) {
    std::string name = _lexer[spec[6][0].getTokenIdx()].getValue();
    unquote(name); species->setName(name);
  }

  // Add species definition to model:
  _model.addDefinition(species);

  // Handle remaining species definition:
  if (spec[7].matched()) {
    processSpeciesDefinition(spec[7][0][1], speciesHasSubstanceUnits);
  }
}


void
Assembler::linkSpeciesDefinition(Parser::ConcreteSyntaxTree &spec, Trafo::VariableScaling &scaleing)
{
  std::string identifier;
  GiNaC::ex initial_value;

  // Default species flags
  bool has_substance_units=false, has_boundary_condition=false, is_constant=false;
  // If initial value species is given in concentrations or amount (default).
  bool has_initial_amount=true;
  // Determine if species are defined globally in amount
  bool species_have_substance_units = _model.speciesHaveSubstanceUnits();

  // Get compartment compartment of species
  std::string compartment_id = _lexer[spec[0].getTokenIdx()].getValue();
  Ast::Compartment *compartment = _model.getCompartment(compartment_id);

  // Get species ID an determine if its initial value is in amount or concentraion
  if (0 == spec[2].getAltIdx()) {
    identifier = _lexer[spec[2][0][1].getTokenIdx()].getValue();
    has_initial_amount = false;
  } else {
    identifier = _lexer[spec[2][0].getTokenIdx()].getValue();
    has_initial_amount = true;
  }
  // Get species instance
  Ast::Species *species = _model.getSpecies(identifier);

  // parse initial value:
  Parser::Expr::ScopeContext context(&_model);
  Parser::Expr::Assembler expr_assembler(context, _lexer);
  initial_value = expr_assembler.processExpression(spec[4]);

  // Get species modifiers
  if (spec[5].matched()) {
    processSpeciesModifierList(spec[5][0], has_substance_units, has_boundary_condition, is_constant);
  }

  // Check for boundary condition modifier:
  if (has_boundary_condition) {
    SBMLFeatureNotSupported err;
    err << "SBML feature 'boudary condition' for species is not implemented yet.";
    throw err;
  }

  // First step, ensure that the initial value is consistent with the species flags.
  // This means, if the species has only substance units, the initial value is scaled to be in
  // substance units also:
  if (has_substance_units && (! has_initial_amount)) {
    // Species has substance units but an initial concentration is given!
    initial_value = initial_value * compartment->getSymbol();
  } else if ((! has_substance_units) && (has_initial_amount)) {
    // Species has concentration units but an initial amount is given:
    initial_value = initial_value / compartment->getSymbol();
  }

  // Set initial value
  species->setValue(initial_value);

  // Second step, the species initial value is now consistent with the species modifier flags.
  // Now, it has to be ensured, that the species is defined in substance units if the model is
  // defined in substance units. This also means that all occurences of the species must be
  // scaled accordingly. This can only be done, once the complete model is parsed. Therefore
  // the scaleing is stored in a @c VariableScaleing class to be applied later.
  if (species_have_substance_units && (! has_substance_units)) {
    // The model is defined in substance units but the species has concentration units:
    scaleing.add(species->getSymbol(), compartment->getSymbol());
  } else if ((!species_have_substance_units) && has_substance_units) {
    // The model is defined in concentration units but the species has substance units:
    scaleing.add(species->getSymbol(), 1/compartment->getSymbol());
  }

  // Handle remaining species definition:
  if (spec[7].matched()) {
    linkSpeciesDefinition(spec[7][0][1], scaleing);
  }
}


void
Assembler::processSpeciesModifierList(Parser::ConcreteSyntaxTree &spec_mod,
                                      bool &has_substance_units, bool &has_boundary_condition,
                                      bool &is_constant)
{
  /* SpeciesModifierList =      : spec_mod
   *   SpeciesModifier            : spec_mod[0]   (token)
   *   [SpeciesModifierList];     : spec_mod[1] */
  const Parser::Token &token = _lexer[spec_mod[0].getTokenIdx()];
  std::string modifier = token.getValue();

  for (size_t i=0; i<modifier.size(); i++) {
    switch (modifier[i]) {
    case 's':
      has_substance_units = true;
      break;
    case 'b':
      has_boundary_condition = true;
      break;
    case 'c':
      is_constant = true;
      break;
    default: {
      SBMLParserError err;
      err << "Cannot parse SBML-SH @ line " << token.getLine()
          << ": Invalid species modifier " << modifier << ", expected s, b or c";
      throw err;
    }
    }
  }

  // Handle remaining modifiers...
  if (spec_mod[1].matched()) {
    processSpeciesModifierList(
          spec_mod[1][0], has_substance_units, has_boundary_condition, is_constant);
  }
}


void
Assembler::processParameterDefinition(Parser::ConcreteSyntaxTree &params)
{
  /* ParameterDefinitionList =          : params
   *   Identifier                         : params[0]
   *   "="
   *   Expression                         : params[2]
   *   [ParameterModifier]                : params[3]
   *   [QuotedString]                     : params[4]
   *   [EOL ParameterDefinitionList];     : params[5], ParameterDefinitionList = params[5][0][1] */

  std::string identifier = _lexer[params[0].getTokenIdx()].getValue();
  bool is_constant = true;

  // There is just one modifier, making paramerer non-constant
  if (params[3].matched()) {
    is_constant = false;
  }

  Ast::Parameter *parameter = new Ast::Parameter(identifier, Ast::Unit::dimensionless(), is_constant);

  // If a name is given
  if (params[4].matched()) {
    std::string name = _lexer[params[4][0].getTokenIdx()].getValue();
    unquote(name); parameter->setName(name);
  }

  // Add parameter to model:
  _model.addDefinition(parameter);

  // Process remaining definitions...
  if (params[5].matched()) {
    processParameterDefinition(params[5][0][1]);
  }
}


void
Assembler::linkParameterDefinition(Parser::ConcreteSyntaxTree &params)
{
  std::string identifier = _lexer[params[0].getTokenIdx()].getValue();
  Parser::Expr::ScopeContext context(&_model);
  Parser::Expr::Assembler expr_assembler(context, _lexer);
  GiNaC::ex value = expr_assembler.processExpression(params[2]);
  Ast::Parameter *parameter = _model.getParameter(identifier);
  parameter->setValue(value);

  // Process remaining definitions...
  if (params[5].matched()) {
    linkParameterDefinition(params[5][0][1]);
  }
}


void
Assembler::processRuleDefinitionList(Parser::ConcreteSyntaxTree &rules)
{
  /* RuleDefinitionList =        : rules
   *   [("@rate"|"@assign")]       : rules[0]
   *   ":"
   *   Identifier                  : rules[2]
   *   "="
   *   Expression                  : rules[4]
   *   [                           : rules[5]
   *     EOL                       : rules[5][0][0]
   *     RuleDefinitionList];      : rules[5][0][1]  */

  // Get variable addressed by identifier:

  Ast::VariableDefinition *var = _model.getVariable(_lexer[rules[2].getTokenIdx()].getValue());
  // Parse expression
  Parser::Expr::ScopeContext context(&_model);
  Parser::Expr::Assembler expr_assembler(context, _lexer);
  GiNaC::ex expr = expr_assembler.processExpression(rules[4]);

  // Determine type of rule
  if (! rules[0].matched()) {
    // Assignment rule by default:
    var->setRule(new Ast::AssignmentRule(expr));
  } else {
    if (0 == rules[0][0].getAltIdx()) {
      // If flag is given and == "@rate"
      var->setRule(new Ast::RateRule(expr));
    } else {
      var->setRule(new Ast::AssignmentRule(expr));
    }
  }

  // Handle remaining rules if present:
  if (rules[5].matched()) {
    processRuleDefinitionList(rules[5][0][1]);
  }
}


void
Assembler::processReactionDefinitions(Parser::ConcreteSyntaxTree &reac)
{
  /* ReactionDefinitionList =      : reac
   *   ("@r" | "@rr")                : reac[0]
   *     "@r"                          : reac[0][0]
   *     "@rr"                         : reac[0][1]
   *   "="
   *   Identifier                    : reac[2]
   *   [QuotedString]                : reac[3]
   *   EOL
   *   ReactionEquation              : reac[5]
   *   [":" ReactionModifierList]    : reac[6], reac[6,0,1] -> ReactionModifierList
   *   [EOL KineticLaw]              : reac[7], reac[7,0,1] -> KineticLaw
   *   [EOL ReactionDefinitionList]; : reac[8]         */
  bool is_reversible = false;
  std::string identifier, name;

  // Determine if reaction is reversible (== @rr)
  if (1 == reac[0].getAltIdx()) is_reversible = true;

  // Get reaction identifier
  identifier = _lexer[reac[2].getTokenIdx()].getValue();

  // Handle name if present
  if (reac[3].matched()) {
    name = _lexer[reac[3][0].getTokenIdx()].getValue(); unquote(name);
  }

  // Get kinetic law
  if (! reac[7].matched()) {
    SBMLFeatureNotSupported err;
    err << "Can not define reaction without kinetic law: Feature not supproted.";
    throw err;
  }

  Ast::KineticLaw *kinetic_law = processKineticLaw(reac[7][0][1]);
  Ast::Reaction   *reaction = new Ast::Reaction(identifier, name, kinetic_law, is_reversible);

  /* Asseble Reaction Equation */
  processReactionEquation(reac[5], reaction);

  // Ignore modifiers
  /*
  std::list<Ast::Species *> modifier_list;
  if (reac[6].matched()) {
    processReactionModifierList(reac[6][0][1], modifier_list);
    for(std::list<Ast::Species *>::iterator mod=modifier_list.begin(); mod!=modifier_list.end(); mod++) {
      reaction->addModifier(*mod);
    }
  }
  */

  // Add reaction to model:
  _model.addDefinition(reaction);

  // Handle remaining reactions:
  if (reac[8].matched()) {
    processReactionDefinitions(reac[8][0][1]);
  }
}


void
Assembler::linkReactionDefinitions(Parser::ConcreteSyntaxTree &reac)
{
  // Get reaction identifier:
  std::string identifier = _lexer[reac[2].getTokenIdx()].getValue();

  // Get kinetic law
  if (! reac[7].matched()) {
    SBMLFeatureNotSupported err;
    err << "Can not define reaction without kinetic law: Feature not supproted.";
    throw err;
  }

  linkKineticLaw(_model.getReaction(identifier)->getKineticLaw(), reac[7][0][1]);

  // Handle remaining reactions:
  if (reac[8].matched()) {
    linkReactionDefinitions(reac[8][0][1]);
  }
}


void
Assembler::processReactionModifierList(Parser::ConcreteSyntaxTree &lst, std::list<Ast::Species *> &mods)
{
  /* ReactionModifierList          : lst
   *   Identifier                    : lst[0]
   *   [',', ReactionModifierList]   : lst[1], lst[1][0][1] -> ReactionModifierList  */

  Ast::VariableDefinition *var = _model.getVariable(_lexer[lst[0].getTokenIdx()].getValue());
  if (! Ast::Node::isSpecies(var)) {
    SBMLParserError err;
    err << "@line "<< _lexer[lst[0].getTokenIdx()].getLine()
        << ": Identifier " << var->getSymbol() << " does not name a species.";
    throw err;
  }

  mods.push_back(static_cast<Ast::Species *>(var));

  if (lst[1].matched()) {
    processReactionModifierList(lst[1][0][1], mods);
  }
}


Ast::KineticLaw *
Assembler::processKineticLaw(Parser::ConcreteSyntaxTree &law)
{
  /* KineticLaw =                           : law
   *   Expression                             : law[0]
   *   [":" LocalParameterDefinitionList];    : law[1] ; */

  GiNaC::ex expression;
  Ast::KineticLaw *kinetic_law = new Ast::KineticLaw(expression);
  kinetic_law->setParent(&_model);

  // First, process local parameters
  if (law[1].matched()) {
    processLocalParameters(kinetic_law, law[1][0][1]);
  }

  // Assemble expression...
  Parser::Expr::ScopeContext context(kinetic_law);
  Parser::Expr::Assembler expr_assembler(context, _lexer);
  expression = expr_assembler.processExpression(law[0]);

  // Update kinetic_law
  kinetic_law->setRateLaw(expression);

  // Done...
  return kinetic_law;
}


void
Assembler::linkKineticLaw(Ast::KineticLaw *kinetic_law, Parser::ConcreteSyntaxTree &law)
{
  // First, process local parameters
  if (law[1].matched()) {
    linkLocalParameters(kinetic_law, law[1][0][1]);
  }
}


void
Assembler::processLocalParameters(Ast::KineticLaw *kinetic_law, Parser::ConcreteSyntaxTree &params)
{
  /* LocalPrameterList =        : params
   *   Identifier                 : params[0]
   *   "="
   *   Expression                 : params[2]
   *   ["," LocalParameterList];  : params[3] */
  std::string identifier = _lexer[params[0].getTokenIdx()].getValue();

  // just predefine local parameter -> Initial value will be parsed by
  // linkLocalParameters()
  kinetic_law->addDefinition(
        new Ast::Parameter(identifier, 0, Ast::Unit::dimensionless(), true));

  // Process remaining parameters...
  if (params[3].matched()) {
    processLocalParameters(kinetic_law, params[3][0][1]);
  }
}


void
Assembler::linkLocalParameters(Ast::KineticLaw *kinetic_law, Parser::ConcreteSyntaxTree &params)
{
  std::string identifier = _lexer[params[0].getTokenIdx()].getValue();
  Ast::Parameter *parameter = kinetic_law->getParameter(identifier);
  Parser::Expr::ScopeContext context(kinetic_law);
  Parser::Expr::Assembler expr_assembler(context, _lexer);
  GiNaC::ex expression = expr_assembler.processExpression(params[2]);
  parameter->setValue(expression);

  // Process remaining parameters...
  if (params[3].matched()) {
    linkLocalParameters(kinetic_law, params[3][0][1]);
  }
}

void
Assembler::processReactionEquation(Parser::ConcreteSyntaxTree &equ, Ast::Reaction *reaction)
{
  /* ReactionEquation =      : equ
   *   (                       :
   *     StoichiometrySum      : equ[0][0]
   *     "->"                  : equ[0][1]
  `*     [StoichiometrySum]) | : equ[0][2]
   *     ( "->" StoichiometrySum)) : equ[0][0];
   */

  if (0 == equ.getAltIdx()) {
    processReactants(equ[0][0], reaction);
    if (equ[0][2].matched()) {
      processProducts(equ[0][2][0], reaction);
    }
  } else {
    processProducts(equ[0][1], reaction);
  }
}


void
Assembler::processReactants(Parser::ConcreteSyntaxTree &sum, Ast::Reaction *reaction) {
  /* StochiometrySum =
   *   [INTEGER] Identifier ["+" StochiometrySum]; */
  double stoichiometry = 1;

  if (sum[0].matched()) {
    stoichiometry = Parser::Expr::Assembler::toNumber<int>(
          _lexer[sum[0][0].getTokenIdx()].getValue());
  }

  std::string identifier = _lexer[sum[1].getTokenIdx()].getValue();
  reaction->addReactantStoichiometry(_model.getSpecies(identifier), stoichiometry);

  // If there are reactants left...
  if (sum[2].matched()) {
    processReactants(sum[2][0][1], reaction);
  }
}


void
Assembler::processProducts(Parser::ConcreteSyntaxTree &sum, Ast::Reaction *reaction) {
  /* StochiometrySum =
   *   [INTEGER] Identifier ["+" StochiometrySum]; */
  double stoichiometry = 1;

  if (sum[0].matched()) {
    stoichiometry = Parser::Expr::Assembler::toNumber<int>(
          _lexer[sum[0][0].getTokenIdx()].getValue());
  }

  std::string identifier = _lexer[sum[1].getTokenIdx()].getValue();
  reaction->addProductStoichiometry(_model.getSpecies(identifier), stoichiometry);

  // If there are products left...
  if (sum[2].matched()) {
    processProducts(sum[2][0][1], reaction);
  }
}


void
Assembler::unquote(std::string &name) {
  name = name.substr(1, name.size()-2);
}


void
Assembler::defineUnit(const std::string &id, const Ast::Unit &unit) {
  if (0 != _units.count(id)) {
    SymbolError err; err << "Can not redefine unit '" << id << "'!";
    throw err;
  }
  _units[id] = unit;
}
