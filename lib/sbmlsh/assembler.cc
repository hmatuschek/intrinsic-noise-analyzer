#include "assembler.hh"
#include "ast/model.hh"
#include "ast/reaction.hh"


using namespace Fluc;
using namespace Fluc::Sbmlsh;


Assembler::Assembler(Ast::Model &model, Utils::Lexer &lexer)
  : _lexer(lexer), _model(model), _scope_stack(1)
{
  _scope_stack[0] = &_model;

  // Assemble base unit map;
  _base_unit_map["ampere"] = Ast::ScaledBaseUnit::AMPERE;
  _base_unit_map["becquerel"] = Ast::ScaledBaseUnit::BECQUEREL;
  _base_unit_map["candela"] = Ast::ScaledBaseUnit::CANDELA;
  _base_unit_map["coulomb"] = Ast::ScaledBaseUnit::COULOMB;
  _base_unit_map["dimensionless"] = Ast::ScaledBaseUnit::DIMENSIONLESS;
  _base_unit_map["farad"] = Ast::ScaledBaseUnit::FARAD;
  _base_unit_map["gram"] = Ast::ScaledBaseUnit::GRAM;
  _base_unit_map["katal"] = Ast::ScaledBaseUnit::KATAL;
  _base_unit_map["gray"] = Ast::ScaledBaseUnit::GRAY;
  _base_unit_map["kelvin"] = Ast::ScaledBaseUnit::KELVIN;
  _base_unit_map["henry"] = Ast::ScaledBaseUnit::HENRY;
  _base_unit_map["kilogram"] = Ast::ScaledBaseUnit::KILOGRAM;
  _base_unit_map["hertz"] = Ast::ScaledBaseUnit::HERTZ;
  _base_unit_map["litre"] = Ast::ScaledBaseUnit::LITRE;
  _base_unit_map["item"] = Ast::ScaledBaseUnit::ITEM;
  _base_unit_map["lumen"] = Ast::ScaledBaseUnit::LUMEN;
  _base_unit_map["joule"] = Ast::ScaledBaseUnit::JOULE;
  _base_unit_map["lux"] = Ast::ScaledBaseUnit::LUX;
  _base_unit_map["metre"] = Ast::ScaledBaseUnit::METRE;
  _base_unit_map["mole"] = Ast::ScaledBaseUnit::MOLE;
  _base_unit_map["newton"] = Ast::ScaledBaseUnit::NEWTON;
  _base_unit_map["ohm"] = Ast::ScaledBaseUnit::OHM;
  _base_unit_map["pascal"] = Ast::ScaledBaseUnit::PASCAL;
  _base_unit_map["radian"] = Ast::ScaledBaseUnit::RADIAN;
  _base_unit_map["second"] = Ast::ScaledBaseUnit::SECOND;
  _base_unit_map["watt"] = Ast::ScaledBaseUnit::WATT;
  _base_unit_map["siemens"] = Ast::ScaledBaseUnit::SIEMENS;
  _base_unit_map["weber"] = Ast::ScaledBaseUnit::WEBER;
  _base_unit_map["sievert"] = Ast::ScaledBaseUnit::SIEVERT;
  _base_unit_map["steradian"] = Ast::ScaledBaseUnit::STERADIAN;
  _base_unit_map["tesla"] = Ast::ScaledBaseUnit::TESLA;
  _base_unit_map["volt"] = Ast::ScaledBaseUnit::VOLT;
}


void
Assembler::process(Utils::ConcreteSyntaxTree &root)
{
  processModel(root);
}



void
Assembler::processModel(Utils::ConcreteSyntaxTree &model)
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

  // If there are Compartment definitions ...
  if (model[2].matched()) {
    // EOL CompartmentDefinitions  : model[2][0]
    // CompartmentDefinitions      : model[2][0][1]
    // "@compartments"               : model[2][0][1][0]
    // EOL
    // CompartmentDefinitionList;    : model[2][0][1][2]
    processCompartmentDefinitions(model[2][0][1][2]);
  }

  // If there are species definitions...
  if (model[3].matched()) {
    // EOL SpeciesDefinitions      : model[3][0]
    // SpeciesDefinitions          : model[3][0][1]
    //   "@species"                  : model[3][0][1][0]
    //   EOL
    //   SpeciesDefinitionList       : model[3][0][1][2]
    processSpeciesDefinition(model[3][0][1][2]);
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

  // Do not process Rule productions...
  if (model[5].matched()) {
    throw SBMLFeatureNotSupported("Can not parse SBML SH: Rule definitions are not supported yet.");
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
    throw SBMLFeatureNotSupported("Can not parse SBML SH: Event definitions are not supported yet.");
  }
}


void
Assembler::processModelDefinition(Utils::ConcreteSyntaxTree &model_header)
{
  /* ModelDefinition =               : model_header
   *   "@model"                        : model_header[0]
   *   ":"
   *   "3.3.1"
   *   "="
   *   Identifier                      : model_header[4]
   *   [QuotedString]                  : model_header[5]
   *   [EOL DefaultUnitDefinitions];   : model_header[6] */

  // Set identifier
  //_model.setIdentifier(_lexer[model_header[4].getTokenIdx()].getValue());

  // Set name if defined
  if (model_header[5].matched()) {
    _model.setName(_lexer[model_header[5][0].getTokenIdx()].getValue());
  }

  // Handle default units if defined:
  if (model_header[6].matched()) {
    processDefaultUnitDefinitions(model_header[6][0]);
  }
}


void
Assembler::processDefaultUnitDefinitions(Utils::ConcreteSyntaxTree &def_units)
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
  std::map<std::string, Ast::ScaledBaseUnit::BaseUnit>::iterator item = _base_unit_map.find(unit_id);
  if (_base_unit_map.end() == item) {
    SBMLParserError err;
    err << "Can not parse SBML SH: Unknown unit name: " << unit_id;
    throw err;
  }
  Ast::ScaledBaseUnit base_unit(item->second, 1,0,1);

  // Dispatch by flag...
  if (def_unit_id == "s") {
    _model.setDefaultSubstanceUnit(base_unit);
  } else if (def_unit_id == "t") {
    _model.setDefaultTimeUnit(base_unit);
  } else if (def_unit_id == "v") {
    _model.setDefaultVolumeUnit(base_unit);
  } else if (def_unit_id == "a") {
    _model.setDefaultAreaUnit(base_unit);
  } else if (def_unit_id == "l") {
    _model.setDefaultLengthUnit(base_unit);
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
Assembler::processUnitDefinition(Utils::ConcreteSyntaxTree &unit)
{
  // UnitDefinitionList =          : unit
  //   Identifier                    :unit[0]  (Token)
  //   "="
  //   ScaledUnitList                :unit[2]
  //   [EOL UnitDefinitionList]      :unit[3]

  // Collect scaled base units for this unit:
  std::list<Ast::ScaledBaseUnit> units;
  processScaledUnitList(unit[2], units);

  // Add unit definition to model:
  std::string identifier = _lexer[unit[0].getTokenIdx()].getValue();
  _model.addDefinition(new Ast::UnitDefinition(identifier, units));

  // If there are some unit definitions left
  if (unit[3].matched()) {
    processUnitDefinition(unit[3][1]);
  }
}


void
Assembler::processScaledUnitList(Utils::ConcreteSyntaxTree &unit,
                                 std::list<Ast::ScaledBaseUnit> &unit_list)
{
  // ScaledUnitList =                      : units
  //   ScaledUnitIdentifier                  : units[0] (Token)
  //   [":" ScaledUnitModifier "=" Number]   : units[1]
  //   ";"
  //   [ScaledUnitList];                     : units[3]

  /* Dispatch by base unit name. */
  const Utils::Token &uid_token = _lexer[unit[0].getTokenIdx()];
  std::string base_unit_id = uid_token.getValue();
  std::map<std::string, Ast::ScaledBaseUnit::BaseUnit>::iterator item
      = _base_unit_map.find(base_unit_id);

  if (_base_unit_map.end() == item) {
    SBMLParserError err;
    err << "Can not parse SBML SH @ line "
        << uid_token.getLine() << ": Unknown unit: " << base_unit_id;
    throw err;
  }
  Ast::ScaledBaseUnit::BaseUnit base_unit = item->second;

  /* Handle unit modifiers. */
  double exponent = 1; double multiplier = 1; int scale = 0;
  if (unit[1].matched()) {
    // units[1] = ":" ScaledUnitModifier "=" Number
    // units[1][1] = ScaledUnitModifier     (token)
    // units[1][3] = Number
    const Utils::Token &modifier_token = _lexer[unit[1][1].getTokenIdx()];

    if (modifier_token.getValue() == "s") {
      scale = processNumber(unit[1][3]);
    } else if (modifier_token.getValue() == "e") {
      exponent = processNumber(unit[1][3]);
    } else if (modifier_token.getValue() == "m") {
      multiplier = processNumber(unit[1][3]);
    } else {
      SBMLParserError err;
      err << "Can not parse SBML SH @ line " << modifier_token.getLine()
          << ": Unknown modifier " << modifier_token.getValue()
          << " expected s, e or m";
      throw err;
    }
  }

  // Assemble scaled base unit:
  unit_list.push_back(Ast::ScaledBaseUnit(base_unit, multiplier, scale, exponent));

  // If there are scaled units left:
  if (unit[3].matched()) {
    processScaledUnitList(unit[3][1], unit_list);
  }
}


void
Assembler::processCompartmentDefinitions(Utils::ConcreteSyntaxTree &comp)
{
  /* CompartmentDefinitionList =        : comp
   *  Identifier                          : comp[0]
   *  ["=" Number]                        : comp[1], Number: comp[1][1]
   *  [QuotedString]                      : comp[2]  (token)
   *  [EOL CompartmentDefinitionList];    : comp[3] */

  std::string id = _lexer[comp[0].getTokenIdx()].getValue();
  Ast::Compartment *compartment = new Ast::Compartment(id, _model.getDefaultVolumeUnit(),
                                                       Ast::Compartment::VOLUME, true);

  // Handle initial value (if defined)
  if (comp[1].matched()) {
    compartment->setValue(processNumber(comp[1][0][1]));
  }

  // Set name if defined:
  if (! comp[2].matched()) {
    compartment->setName(_lexer[comp[2][0].getTokenIdx()].getValue());
  }

  // Add compartment to model:
  _model.addDefinition(compartment);

  // Process remaining compartments
  if (comp[3].matched()) {
    processCompartmentDefinitions(comp[3][1]);
  }
}


void
Assembler::processSpeciesDefinition(Utils::ConcreteSyntaxTree &spec)
{
  /* SpeciesDefinitionList =           : spec
   *   ID                                : spec[0]
   *   ":"                               : spec[1]
   *   (("[" ID "]") | ID)               : spec[2]
   *     "[" ID "]"                        : spec[2][0][1]
   *     ID                                : spec[2][0]
   *   "="                               : spec[3]
   *   Number                            : spec[4]
   *   [SpeciesModifierList]             : spec[5]
   *   [QuotedString]                    : spec[6]
   *   [EOL SpeciesDefinitionList];      : spec[7]
   */

  std::string compartment_id = _lexer[spec[0].getTokenIdx()].getValue();
  std::string identifier;
  double initial_value;
  bool has_substance_units=false; bool has_boundary_condition=false; bool is_constant=false;

  Ast::Compartment *compartment = _model.getCompartment(compartment_id);
  if (0 == spec[2].getAltIdx()) {
    identifier = _lexer[spec[2][0][1].getTokenIdx()].getValue();
  } else {
    identifier = _lexer[spec[2][0].getTokenIdx()].getValue();
  }

  initial_value = processNumber(spec[4]);
  if (spec[5].matched()) {
    processSpeciesModifierList(spec[5], has_substance_units, has_boundary_condition, is_constant);
  }

  Ast::Species *species;
  if (has_substance_units) {
    species = new Ast::Species(identifier, _model.getDefaultSubstanceUnit(), compartment, is_constant);
  } else {
    /// @bug Handle concentration units properly here!
    species = new Ast::Species(identifier, _model.getDefaultSubstanceUnit(), compartment, is_constant);
  }

  // Set initial value...
  species->setValue(initial_value);

  // Handle name
  if (spec[6].matched()) {
    species->setName(_lexer[spec[6][0].getTokenIdx()].getValue());
  }

  // Add species definition to model:
  _model.addDefinition(species);

  // Handle remaining species definition:
  if (spec[7].matched()) {
    processSpeciesDefinition(spec[7][0][1]);
  }
}


void
Assembler::processSpeciesModifierList(Utils::ConcreteSyntaxTree &spec_mod,
                                      bool &has_substance_units, bool &has_boundary_condition,
                                      bool &is_constant)
{
  /* SpeciesModifierList =      : spec_mod
   *   SpeciesModifier            : spec_mod[0]   (token)
   *   [SpeciesModifierList];     : spec_mod[1] */
  const Utils::Token &token = _lexer[spec_mod[0].getTokenIdx()];
  std::string modifier = token.getValue();

  if (modifier == "s") {
    has_substance_units = true;
  } else if (modifier == "b") {
    has_boundary_condition = true;
  } else if (modifier == "c") {
    is_constant = true;
  } else {
    SBMLParserError err;
    err << "Can not parse SBML SH @ line " << token.getLine()
        << ": Invalid species modifier " << modifier << ", expected s,b or c";
    throw err;
  }

  // Handle remaining modifiers...
  if (spec_mod[1].matched()) {
    processSpeciesModifierList(
          spec_mod[1][0], has_substance_units, has_boundary_condition, is_constant);
  }
}


void
Assembler::processParameterDefinition(Utils::ConcreteSyntaxTree &params)
{
  /* ParameterDefinitionList =          : params
   *   Identifier                         : params[0]
   *   "="
   *   Number                             : params[2]
   *   [ParameterModifier]                : params[3]
   *   [QuotedString]                     : params[4]
   *   [EOL ParameterDefinitionList];     : params[5], ParameterDefinitionList = params[5][0][1] */

  std::string identifier = _lexer[params[0].getTokenIdx()].getValue();
  double value = processNumber(params[1]);
  bool is_constant = true;

  // There is just one modifier, making paramerer non-constant
  if (params[3].matched()) {
    is_constant = false;
  }

  Ast::Parameter *parameter = new Ast::Parameter(identifier, Ast::Unit::dimensionless(), is_constant);
  parameter->setValue(value);

  // If a name is given
  if (params[4].matched()) {
    parameter->setName(_lexer[params[4][0].getTokenIdx()].getValue());
  }

  // Add parameter to model:
  _model.addDefinition(parameter);

  // Process remaining definitions...
  if (params[5].matched()) {
    processParameterDefinition(params[5][0][1]);
  }
}


void
Assembler::processReactionDefinitions(Utils::ConcreteSyntaxTree &reac)
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
   *   [":" Identifier]              : reac[6]
   *   EOL
   *   KineticLaw                    : reac[8]
   *   [EOL ReactionDefinitionList]; : reac[9]         */
  bool is_reversible = false;
  std::string identifier, name;

  // Determine if reaction is reversible (== @rr)
  if (1 == reac[0].getAltIdx()) is_reversible = true;

  // Get reaction identifier
  identifier = _lexer[reac[2].getTokenIdx()].getValue();

  // Handle name if present
  if (reac[3].matched()) {
    name = _lexer[reac[3][0].getTokenIdx()].getValue();
  }

  // Get kinetic law
  Ast::KineticLaw *kinetic_law = processKineticLaw(reac[8]);
  Ast::Reaction   *reaction = new Ast::Reaction(identifier, name, kinetic_law, is_reversible);

  /* Asseble Reaction Equation */
  processReactionEquation(reac[5], reaction);

  // Handle modifier
  if (reac[6].matched()) {
    std::string modifier_id = _lexer[reac[6][0][1].getTokenIdx()].getValue();
    Ast::Species *modifier = _model.getSpecies(modifier_id);
    reaction->addModifier(modifier);
  }

  // Add reaction to model:
  _model.addDefinition(reaction);

  // Handle remaining reactions:
  if (reac[9].matched()) {
    processReactionDefinitions(reac[9][0][1]);
  }
}


Ast::KineticLaw *
Assembler::processKineticLaw(Utils::ConcreteSyntaxTree &law)
{
  /* KineticLaw =                           : law
   *   Expression                             : law[0]
   *   [":" LocalParameterDefinitionList];    : law[1] ; */

  GiNaC::ex expression;
  Ast::KineticLaw *kinetic_law = new Ast::KineticLaw(expression);

  // push kinetic_law on stack of variable scopes
  _scope_stack.push_back(kinetic_law);

  // First, process local parameters
  if (law[1].matched()) {
    processLocalParameters(law[1][0][1]);
  }

  // Assemble expression...
  expression = processExpression(law[0]);

  // Done, remove from scope stack...
  _scope_stack.pop_back();

  // Update kinetic_law
  kinetic_law->setRateLaw(expression);

  // Done...
  return kinetic_law;
}


void
Assembler::processLocalParameters(Utils::ConcreteSyntaxTree &params)
{
  /* LocalPrameterList =        : params
   *   Identifier                 : params[0]
   *   "="
   *   Expression                 : params[2]
   *   ["," LocalParameterList];  : params[3] */
  std::string identifier = _lexer[params[0].getTokenIdx()].getValue();
  GiNaC::ex expression = processExpression(params[2]);

  _scope_stack.back()->addDefinition(
        new Ast::Parameter(identifier, expression, Ast::Unit::dimensionless(), true));

  // Process remaining parameters...
  if (params[3].matched()) {
    processLocalParameters(params[3][0][1]);
  }
}


void
Assembler::processReactionEquation(Utils::ConcreteSyntaxTree &equ, Ast::Reaction *reaction)
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
Assembler::processReactants(Utils::ConcreteSyntaxTree &sum, Ast::Reaction *reaction) {
  /* StochiometrySum =
   *   [INTEGER] Identifier ["+" StochiometrySum]; */
  double stoichiometry = 1;

  if (sum[0].matched()) {
    stoichiometry = processNumber(sum[0][0]);
  }

  std::string identifier = _lexer[sum[1].getTokenIdx()].getValue();
  reaction->addReactantStoichiometry(_model.getSpecies(identifier), stoichiometry);

  // If there are reactants left...
  if (sum[2].matched()) {
    processReactants(sum[2][0][1], reaction);
  }
}


void
Assembler::processProducts(Utils::ConcreteSyntaxTree &sum, Ast::Reaction *reaction) {
  /* StochiometrySum =
   *   [INTEGER] Identifier ["+" StochiometrySum]; */
  double stoichiometry = 1;

  if (sum[0].matched()) {
    stoichiometry = processNumber(sum[0][0]);
  }

  std::string identifier = _lexer[sum[1].getTokenIdx()].getValue();
  reaction->addProductStoichiometry(_model.getSpecies(identifier), stoichiometry);

  // If there are products left...
  if (sum[2].matched()) {
    processProducts(sum[2][0][1], reaction);
  }
}


GiNaC::ex
Assembler::processExpression(Utils::ConcreteSyntaxTree &expr)
{
  /* Expression =               : expr
   *   (                          : expr[0]
   *     ProductExpression          : expr[0][0]
   *     ("+"|"-")                  : expr[0][1]
   *     Expression) |              : expr[0][2]
   *    ProductExpression;        : expr[0] */

  if (0 == expr.getAltIdx()) {
    GiNaC::ex lhs = processProductExpression(expr[0][0]);
    GiNaC::ex rhs = processExpression(expr[0][2]);
    if (0 == expr[0][1].getAltIdx()) {
      return lhs + rhs;
    } else {
      return lhs - rhs;
    }
  }

  return processProductExpression(expr[0]);
}


GiNaC::ex
Assembler::processProductExpression(Utils::ConcreteSyntaxTree &expr)
{
 /* ProductExpression =         : expr
  *   (                         : expr[0]
  *     AtomicExpression          : expr[0][0]
  *     ("*" | "/")               : expr[0][1]
  *     ProductExpression) |      : expr[0][2]
  *   AtomicExpression;         : expr[0]   */

  if (0 == expr.getAltIdx()) {
    GiNaC::ex lhs = processAtomicExpression(expr[0][0]);
    GiNaC::ex rhs = processProductExpression(expr[0][2]);
    if (0 == expr[0][1].getAltIdx()) {
      return lhs + rhs;
    } else {
      return lhs - rhs;
    }
  }

  return processAtomicExpression(expr[0]);
}


GiNaC::ex
Assembler::processAtomicExpression(Utils::ConcreteSyntaxTree &expr)
{
  /* AtomicExpression =       : expr
   *   Identifier |           : expr[0]
   *   Number |               : expr[0]
   *   ("(" Expression ")");  : expr[0]; Expression : expr[0][1]; */

  if (0 == expr.getAltIdx()) {
    std::string identifier = _lexer[expr[0].getTokenIdx()].getValue();
    return resolveIdentifier(identifier);
  } else if (1 == expr.getAltIdx()) {
    return processNumber(expr[0]);
  } else {
    return processExpression(expr[0][1]);
  }
}


GiNaC::ex
Assembler::resolveIdentifier(const std::string &id)
{
  for (std::vector<Ast::Scope *>::reverse_iterator scope=_scope_stack.rbegin();
       scope != _scope_stack.rend(); scope++) {
    if ((*scope)->hasDefinition(id)) {
      Ast::Definition *def = (*scope)->getDefinition(id);
      Ast::VariableDefinition *var = 0;
      if (0 == (var = dynamic_cast<Ast::VariableDefinition *>(def))) {
        SymbolError err;
        err << "Can not resolve symbol " << id << ": Identifier does not refer to a variable!";
        throw err;
      }
      return var->getSymbol();
    }

    if ((*scope)->isClosed()) {
      SymbolError err;
      err << "Can not resolve symbol " << id << ": Identifier not found.";
      throw err;
    }
  }

  SymbolError err;
  err << "Can not resolve symbol " << id << ": Identifier not found.";
  throw err;
}


double
Assembler::processNumber(Utils::ConcreteSyntaxTree &num)
{
  /* Number =
   *   ["-"] (INTEGER | FLOAT); */
  double value = 1.0;

  if (num[0].matched()) {
    value = -1.0;
  }

  if (0 == num[1].getAltIdx()) {
    value *= toNumber<int>(_lexer[num[1][0].getTokenIdx()].getValue());
  } else {
    value *= toNumber<double>(_lexer[num[1][0].getTokenIdx()].getValue());
  }

  return value;
}
