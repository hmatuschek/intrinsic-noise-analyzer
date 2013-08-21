#include "assembler.hh"
#include "math.hh"

using namespace iNA;
using namespace iNA::Parser::Sbml;


/* ********************************************************************************************* *
 * Implementation of ParserContext...
 * ********************************************************************************************* */
ParserContext::ParserContext(Ast::Model &model)
  : _model(model)
{
  // Store pointer to module (also a scope) into scope stack:
  this->_scope_stack.push_back(&_model);

  // Define predefined units...
  _units["ampere"] = Ast::Unit(Ast::ScaledBaseUnit::AMPERE, 1, 0, 1);
  _units["becquerel"] = Ast::Unit(Ast::ScaledBaseUnit::BECQUEREL, 1, 0, 1);
  _units["candela"] = Ast::Unit(Ast::ScaledBaseUnit::CANDELA, 1, 0, 1);
  _units["coulomb"] = Ast::Unit(Ast::ScaledBaseUnit::COULOMB, 1, 0, 1);
  _units["dimensionless"] = Ast::Unit(Ast::ScaledBaseUnit::DIMENSIONLESS, 1, 0, 1);
  _units["farad"] = Ast::Unit(Ast::ScaledBaseUnit::FARAD, 1, 0, 1);
  _units["gram"] = Ast::Unit(Ast::ScaledBaseUnit::GRAM, 1, 0, 1);
  _units["katal"] = Ast::Unit(Ast::ScaledBaseUnit::KATAL, 1, 0, 1);
  _units["gray"] = Ast::Unit(Ast::ScaledBaseUnit::GRAY, 1, 0, 1);
  _units["kelvin"] = Ast::Unit(Ast::ScaledBaseUnit::KELVIN, 1, 0, 1);
  _units["henry"] = Ast::Unit(Ast::ScaledBaseUnit::HENRY, 1, 0, 1);
  _units["kilogram"] = Ast::Unit(Ast::ScaledBaseUnit::KILOGRAM, 1, 0, 1);
  _units["hertz"] = Ast::Unit(Ast::ScaledBaseUnit::HERTZ, 1, 0, 1);
  _units["litre"] = Ast::Unit(Ast::ScaledBaseUnit::LITRE, 1, 0, 1);
  _units["item"] = Ast::Unit(Ast::ScaledBaseUnit::ITEM, 1, 0, 1);
  _units["lumen"] = Ast::Unit(Ast::ScaledBaseUnit::LUMEN, 1, 0, 1);
  _units["joule"] = Ast::Unit(Ast::ScaledBaseUnit::JOULE, 1, 0, 1);
  _units["lux"] = Ast::Unit(Ast::ScaledBaseUnit::LUX, 1, 0, 1);
  _units["metre"] = Ast::Unit(Ast::ScaledBaseUnit::METRE, 1, 0, 1);
  _units["mole"] = Ast::Unit(Ast::ScaledBaseUnit::MOLE, 1, 0, 1);
  _units["newton"] = Ast::Unit(Ast::ScaledBaseUnit::NEWTON, 1, 0, 1);
  _units["ohm"] = Ast::Unit(Ast::ScaledBaseUnit::OHM, 1, 0, 1);
  _units["pascal"] = Ast::Unit(Ast::ScaledBaseUnit::PASCAL, 1, 0, 1);
  _units["radian"] = Ast::Unit(Ast::ScaledBaseUnit::RADIAN, 1, 0, 1);
  _units["second"] = Ast::Unit(Ast::ScaledBaseUnit::SECOND, 1, 0, 1);
  _units["watt"] = Ast::Unit(Ast::ScaledBaseUnit::WATT, 1, 0, 1);
  _units["siemens"] = Ast::Unit(Ast::ScaledBaseUnit::SIEMENS, 1, 0, 1);
  _units["weber"] = Ast::Unit(Ast::ScaledBaseUnit::WEBER, 1, 0, 1);
  _units["sievert"] = Ast::Unit(Ast::ScaledBaseUnit::SIEVERT, 1, 0, 1);
  _units["steradian"] = Ast::Unit(Ast::ScaledBaseUnit::STERADIAN, 1, 0, 1);
  _units["tesla"] = Ast::Unit(Ast::ScaledBaseUnit::TESLA, 1, 0, 1);
  _units["volt"] = Ast::Unit(Ast::ScaledBaseUnit::VOLT, 1, 0, 1);
}


void
ParserContext::pushScope(Ast::Scope *scope)
{
  this->_scope_stack.push_back(scope);
}


void
ParserContext::popScope()
{
  this->_scope_stack.pop_back();
}


Ast::Scope *
ParserContext::currentScope()
{
  return this->_scope_stack.back();
}


Ast::Species *
ParserContext::resolveSpecies(const std::string &id)
{
  Ast::VariableDefinition *var = this->resolveVariable(id);

  if (! Ast::Node::isSpecies(var))
  {
    SymbolError err;
    err << "There is no species named " << id << " defined.";
    throw err;
  }

  return static_cast<Ast::Species *>(var);
}


Ast::VariableDefinition *
ParserContext::resolveVariable(const std::string &id)
{
  // Start reverse search variable in scope stack:
  return this->resolveVariable(id, this->_scope_stack.rbegin());
}


Ast::VariableDefinition *
ParserContext::resolveVariable(const std::string &id, std::list<Ast::Scope *>::reverse_iterator scope)
{
  if (this->_scope_stack.rend() == scope)
  {
    SymbolError err;
    err << "No symbol named " << id << " found!";
    throw err;
  }

  if (! (*scope)->hasDefinition(id))
  {
    if ((*scope)->isClosed())
    {
      SymbolError err;
      err << "No symbol named " << id << " found in closed scope.";
      throw err;
    }
    return this->resolveVariable(id, ++scope);
  }

  Ast::Definition *def = (*scope)->getDefinition(id);
  if (! Ast::Node::isVariableDefinition(def))
  {
    InternalError err;
    err << "Can resolve symbol " << id << " to a variable: its not a variable!";
    throw err;
  }

  return static_cast<Ast::VariableDefinition *>(def);
}


Ast::Model &
ParserContext::model() {
  return _model;
}

bool
ParserContext::hasUnit(const std::string &id) const {
  // check if unit ID is one of the model units
  if (("substance"==id) || ("volume"==id) || ("area"==id) || ("length"==id) || ("time"==id)) {
    return true;
  }
  // otherwise resolve identifier:
  return 0 != _units.count(id);
}

const Ast::Unit &
ParserContext::unit(const std::string &id) const {
  // Resolve model units
  if ("substance"==id)   { return _model.getSubstanceUnit(); }
  else if ("volume"==id) { return _model.getVolumeUnit(); }
  else if ("area"==id)   { return _model.getAreaUnit(); }
  else if ("length"==id) { return _model.getLengthUnit(); }
  else if ("time"==id)   { return _model.getTimeUnit(); }
  // Resolve unit identifier in user and pre-defined units.
  std::map<std::string, Ast::Unit>::const_iterator unit = _units.find(id);
  if (_units.end() == unit) {
    SymbolError err; err << "Unit '" << id << "' not defined!";
    throw err;
  }
  return unit->second;
}

void
ParserContext::defineUnit(const std::string &id, const Ast::Unit &unit) {
  if (hasUnit(id)) {
    SymbolError err; err << "Can not redefine unit '" << id << "'!";
    throw err;
  }
  _units[id] = unit;
}


/* ******************************************************************************************** *
 * Implementation of helper functions to assemble the Ast::Model from a SBML model
 * ******************************************************************************************** */
void
Parser::Sbml::__process_model(LIBSBML_CPP_NAMESPACE_QUALIFIER Model *model, ParserContext &ctx)
{
  // Set model identifier:
  ctx.model().setIdentifier(model->getId());

  // Set name of model if set
  if (model->isSetName()) {
    ctx.model().setName(model->getName());
  }

  // First of all, check if all species are defined in substance units, if it is the case,
  // set model to substance units:
  bool all_species_have_substance_units = (0 != model->getNumSpecies());
  for (size_t i=0; i<model->getNumSpecies(); i++) {
    all_species_have_substance_units &=
        model->getSpecies(i)->isSetHasOnlySubstanceUnits();
  }
  // if all species have substance units -> the model too...
  if (all_species_have_substance_units) {
    ctx.model().setSpeciesHaveSubstanceUnits(true);
  }

  /* Process all function definitions in model by forwarding them to
   * __process_function_definition. */
  for (size_t i=0; i<model->getNumFunctionDefinitions(); i++) {
    LIBSBML_CPP_NAMESPACE_QUALIFIER FunctionDefinition *func_def = model->getFunctionDefinition(i);
    Ast::FunctionDefinition *def = __process_function_definition(func_def, ctx);
    ctx.model().addDefinition(def);
  }

  /// @bug Unit definitions live in a separate scope, they identifier may collide with those
  /// used to species etc. Simply do not add unit-definitions into the model scope. Store them
  /// in a temporary table for parsing only, hence models will have no unit definitions as
  /// only parameters may have individual units.

  /* First of all, process unit-definitions. */
  for (size_t i=0; i<model->getNumUnitDefinitions(); i++)
  {
    LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *sbml_unit = model->getUnitDefinition(i);
    // Handle default units
    if (__is_default_unit_redefinition(sbml_unit, ctx)) {
      __process_default_unit_redefinition(sbml_unit, ctx);
      continue;
    }
    // Otherwise handle a new unit:
    __process_unit_definition(sbml_unit, ctx);
  }

  /* Process all parameters of the model by forwarding them to processParameterDefinition. */
  for (size_t i=0; i<model->getNumParameters(); i++)
  {
    // Get parameter
    LIBSBML_CPP_NAMESPACE_QUALIFIER Parameter *sbml_param = model->getParameter(i);
    // Turn into Ast::ParameterDefinition
    Ast::VariableDefinition *param = __process_parameter_definition(sbml_param, ctx);
    // Store definition into global module:
    ctx.model().addDefinition(param);
  }

  // Some substitutions may be needed to maintain a cosistent model as species and compartment
  // units are unified to default units
  Trafo::VariableScaling scaling;

  /* Process all compartments defined in the SBML model. */
  for (size_t i=0; i<model->getNumCompartments(); i++)
  {
    // Get SBML compartment
    LIBSBML_CPP_NAMESPACE_QUALIFIER Compartment *comp = model->getCompartment(i);
    // Translate into AST
    Ast::VariableDefinition *comp_def = __process_compartment_definition(comp, ctx, scaling);
    // Store in module:
    ctx.model().addDefinition(comp_def);
  }

  /* Process all species defined in the model. */
  for (size_t i=0; i<model->getNumSpecies(); i++)
  {
    // Get species:
    LIBSBML_CPP_NAMESPACE_QUALIFIER Species *sp = model->getSpecies(i);
    // convert
    Ast::VariableDefinition *sp_def = __process_species_definition(sp, ctx, scaling);
    // store
    ctx.model().addDefinition(sp_def);
  }

  /* Process initial assignments. */
  for (size_t i=0; i<model->getNumInitialAssignments(); i++) {
    LIBSBML_CPP_NAMESPACE_QUALIFIER InitialAssignment *sbml_assignment = model->getInitialAssignment(i);
    // Search for variable
    if (!ctx.model().hasVariable(sbml_assignment->getId())) {
      SymbolError err;
      err << "Can not process initial assingment for identifier "
          << sbml_assignment->getId() << "; Unknown variable.";
      throw err;
    }
    Ast::VariableDefinition *var = ctx.model().getVariable(sbml_assignment->getId());
    var->setValue(__process_expression(sbml_assignment->getMath(), ctx));
  }

  /* Process rules for variables: */
  for (size_t i=0; i<model->getNumRules(); i++)
  {
    // Get SBML rule:
    LIBSBML_CPP_NAMESPACE_QUALIFIER Rule *sbml_rule = model->getRule(i);
    // Construct expression of rule:
    GiNaC::ex rule_expr = __process_expression(sbml_rule->getMath(), ctx);

    // Dispatch by type:
    if (sbml_rule->isAssignment()) {
      // Find variable the rule is associated with:
      if (! ctx.model().hasDefinition(sbml_rule->getVariable())) {
        SymbolError err;
        err << "Variable " << sbml_rule->getVariable()
            << " in assignment rule not defined in module.";
        throw err;
      }

      // Check if definition is a variable definition.
      Ast::VariableDefinition *def = ctx.model().getVariable(sbml_rule->getVariable());
      // Construct rule and assign it to variable:
      def->setRule(new Ast::AssignmentRule(rule_expr));
    } else if (sbml_rule->isRate()) {
      // Find variable the rule is associated with:
      if (! ctx.model().hasDefinition(sbml_rule->getVariable())) {
        SymbolError err;
        err << "Variable " << sbml_rule->getVariable()
            << " in rate rule not defined in module.";
        throw err;
      }
      // Check if definition is a variable definition.
      Ast::VariableDefinition *def = ctx.model().getVariable(sbml_rule->getVariable());
      // Construct rule and assign it to variable:
      def->setRule(new Ast::RateRule(rule_expr));
    } else {
      throw SBMLFeatureNotSupported("Can not create rule: rule type not supported.");
    }
  }

  /* Process all reactions, especially all the kinetic laws of them. The identifier of the kinetic
   * law is taken as the identifier of the reaction. */
  for (size_t i=0; i<model->getNumReactions(); i++)
  {
    // Get SBML Reaction object from model
    LIBSBML_CPP_NAMESPACE_QUALIFIER Reaction *reac = model->getReaction(i);
    Ast::Reaction *ast_reac = __process_reaction(reac, ctx);
    // store in module:
    ctx.model().addDefinition(ast_reac);
  }

  /* Check if there are any Events defined, they are not supported by iNA! */
  if (0 != model->getNumEvents())
  {
    SBMLFeatureNotSupported err;
    err << "Can not handle event definitions. Feature not supported.";
    throw err;
  }

  // Finally apply all substitutions as they are needed to maintain a consistent model (due to
  // unit changes).
  ctx.model().apply(scaling);
}


Ast::VariableDefinition *
Parser::Sbml::__process_species_definition(
  LIBSBML_CPP_NAMESPACE_QUALIFIER Species *node, ParserContext &ctx, Trafo::VariableScaling &subst)
{
  // Get compartment for species:
  if (! ctx.model().hasCompartment(node->getCompartment())) {
      SymbolError err;
      err << "Can not assemble species definition " << node->getId()
          << ", compartment "<< node->getCompartment() << " not defined.";
      throw err;
  }
  Ast::Compartment *compartment = ctx.model().getCompartment(node->getCompartment());

  /* Second, process unit and initial value of species. */
  bool model_has_substance_units = ctx.model().speciesHaveSubstanceUnits();
  bool species_has_substance_units = node->getHasOnlySubstanceUnits();
  Ast::Unit model_substance_unit = ctx.model().getSubstanceUnit();

  /* Setup initial value. It is ensured that the initial value is an amount if the species is
   * defined to have substance units and concentration if the species is defined to have
   * concentration units. */
  GiNaC::ex init_value;
  if (node->isSetInitialAmount()) {
    if (species_has_substance_units) {
      init_value = GiNaC::numeric(node->getInitialAmount());
    } else {
      init_value = GiNaC::numeric(node->getInitialAmount())/compartment->getSymbol();
    }
  } else if (node->isSetInitialConcentration()){
    if (species_has_substance_units) {
      init_value = GiNaC::numeric(node->getInitialConcentration())*compartment->getSymbol();
    } else {
      init_value = GiNaC::numeric(node->getInitialConcentration());
    }
  }

  // Assemble species definition
  Ast::Species *species = new Ast::Species(node->getId(), init_value, compartment, node->getName(),
                                           node->getConstant());

  /* Now, init_value is now given in a unit corresponding to the unit specified in the species
   * definition. While SBML allows to specify species specific units the species itself might be
   * scaled to match the global units of the model. */
  // by default the subtance unit of the species is the global substance unit
  Ast::Unit species_substance_unit = model_substance_unit;
  // If the species has its own unit
  if (node->isSetUnits()) {
    // Get unit defined for species:
    species_substance_unit = ctx.unit(node->getUnits());
  }

  // Get the factor that scales the species substance unit into the model substance unit
  GiNaC::ex factor = 1;
  if (species_substance_unit != model_substance_unit) {
    // The quotient of the model substance units and the species substance unit.
    Ast::Unit quotient = model_substance_unit/species_substance_unit;
    // If the quotient is a number -> use the number as a factor to scale the species:
    if (! quotient.isDimensionless()) {
      factor = factor * quotient.getMultiplier();
      factor = factor * GiNaC::pow(10, quotient.getScale());
    } else if ((2==quotient.size()) && quotient.hasVariantOf(Ast::ScaledBaseUnit::MOLE, 1) &&
        quotient.hasVariantOf(Ast::ScaledBaseUnit::ITEM, -1)) {
      // Quotient is linear scaleing of MOLE/#
      factor = factor * quotient.getMultiplier();
      factor = factor * GiNaC::pow(10, quotient.getScale());
      factor = factor * constants::AVOGADRO;
    } else if ((2==quotient.size()) && quotient.hasVariantOf(Ast::ScaledBaseUnit::MOLE, -1) &&
              quotient.hasVariantOf(Ast::ScaledBaseUnit::ITEM, 1)) {
      // Quotient is linear scaleing of #/MOLE
      factor = factor * quotient.getMultiplier();
      factor = factor * GiNaC::pow(10, quotient.getScale());
      factor = factor / constants::AVOGADRO;
    } else {
      // Unknown quotient
      TypeError err;
      err << "Can not rescale species with unit " << species_substance_unit.dump()
          << ". Can not be traslated into unit " << model_substance_unit.dump();
      throw err;
    }
  }

  // Finally, if the species is defined in concentrations and the model in substance units,
  // scale species accordingly. Also the other way around, if the species is defined in substance
  // units and the model is defined in concentration units.
  if (model_has_substance_units && (! species_has_substance_units)) {
    factor = factor * species->getCompartment()->getSymbol();
  } else if ( (!model_has_substance_units) && species_has_substance_units) {
    factor = factor / species->getCompartment()->getSymbol();
  }

  // if the species needs to be rescaled:
  if (1 != factor) {
    subst.add(species->getSymbol(), factor);
  }

  return species;
}


Ast::FunctionDefinition *
Parser::Sbml::__process_function_definition(LIBSBML_CPP_NAMESPACE_QUALIFIER FunctionDefinition *funcdef, ParserContext &ctx)
{
  // Extract identifier:
  std::string id = funcdef->getId();

  // Extract and assemble function-arguments
  std::vector<Ast::VariableDefinition *> args(funcdef->getNumArguments());
  for (size_t i=0; i<funcdef->getNumArguments(); i++) {
    // Construct a new VariableDefinition for the argument:
    args[i]= new Ast::FunctionArgument(funcdef->getArgument(i)->getName());
  }

  // Assemble and return function definition
  Ast::FunctionDefinition *def = new Ast::FunctionDefinition(id, args, GiNaC::ex());

  // Push scope of function, function body is processed in this scope:
  ctx.pushScope(def);

  // Extract and assemble function body
  GiNaC::ex body = __process_expression(funcdef->getBody(), ctx);

  // Done. pop scope:
  ctx.popScope();

  // Set body of function:
  def->setBody(body);

  // Done.
  return def;
}


Ast::VariableDefinition *
Parser::Sbml::__process_parameter_definition(LIBSBML_CPP_NAMESPACE_QUALIFIER Parameter *node, ParserContext &ctx)
{
  /* Handle initial value for paramter. */
  GiNaC::ex init_value;
  if (node->isSetValue()) {
    init_value = GiNaC::numeric(node->getValue());
  }

  // Get units for parameter (it there is one):
  Ast::Unit unit(Ast::Unit::dimensionless());
  if (node->isSetUnits()) {
    unit = ctx.unit(node->getUnits());
  }

  // Construct and return parameter definition (constant variable)
  Ast::Parameter *param = new Ast::Parameter(node->getId(), init_value, unit, node->getConstant());

  // Set display name if defined:
  if (node->isSetName()) {
    param->setName(node->getName());
  }

  return param;
}


Ast::VariableDefinition *
Parser::Sbml::__process_compartment_definition(
  LIBSBML_CPP_NAMESPACE_QUALIFIER Compartment *node, ParserContext &ctx, Trafo::VariableScaling &subst)
{
  if (3 != node->getSpatialDimensions()) {
    SBMLFeatureNotSupported err;
    err << "Spacial dimension of compartment must be 3!";
    throw err;
  }

  if (node->isSetOutside()) {
    SBMLFeatureNotSupported err;
    err << "No nested compartments are supported.";
    throw err;
  }

  // Check if there is an size value:
  GiNaC::ex init_value;
  if (node->isSetSize()) {
    init_value = GiNaC::numeric(node->getSize());
  }

  Ast::Unit unit(ctx.model().getVolumeUnit());

  // Get Spatial dimension of compartment
  Ast::Compartment::SpatialDimension sp_dim = Ast::Compartment::VOLUME;

  // Assemble compartment:
  Ast::Compartment *comp =
      new Ast::Compartment(node->getId(), init_value, sp_dim, node->getConstant());

  // Set display name if defined:
  if (node->isSetName()) {
    comp->setName(node->getName());
  }

  // Now, handle unit of compartment
  if (node->isSetUnits() && !(unit == ctx.unit(node->getUnits()))) {
    Ast::Unit comp_unit = ctx.unit(node->getUnits());
    Ast::Unit scale = unit/comp_unit;
    if (! scale.isDimensionless()) {
      TypeError err;
      err << "Can not convert unit of compartment " << node->getId() << ": "
          << comp_unit.dump() << " into model volume unit "
          << unit.dump();
      throw err;
    }
    double factor = scale.getMultiplier() * std::pow(10., scale.getScale());
    subst.add(comp->getSymbol(), factor);
  }

  return comp;
}


bool
Parser::Sbml::__is_default_unit_redefinition(LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *node, ParserContext &ctx)
{
  if ("substance" == node->getId() || "volume" == node->getId() || "area" == node->getId() ||
      "length" == node->getId() || "time" == node->getId()) {
    return true;
  }

  return false;
}


void
Parser::Sbml::__process_default_unit_redefinition(LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *node, ParserContext &ctx)
{
  // Check number of units:
  if (1 != node->getNumUnits()) {
    SemanticError err;
    err << "Redefinition of default unit " << node->getId() << " must contain exactly one "
        << " unit element! Found: " << node->getNumUnits();
    throw err;
  }

  if ("substance" == node->getId()) {
    Ast::Unit unit = __process_unit(node->getUnit(0), ctx);
    if (! unit.isSubstanceUnit()) {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a substance unit.";
      throw err;
    }
    ctx.model().setSubstanceUnit(unit, false);
  } else if ("volume" == node->getId()) {
    Ast::Unit unit = __process_unit(node->getUnit(0), ctx);
    if (! unit.isVolumeUnit()) {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a volume unit.";
      throw err;
    }
    ctx.model().setVolumeUnit(unit, false);
  } else if ("area" == node->getId()) {
    Ast::Unit unit = __process_unit(node->getUnit(0), ctx);
    if (! unit.isAreaUnit()) {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a area unit.";
      throw err;
    }
    ctx.model().setAreaUnit(unit, false);
  } else if ("length" == node->getId()) {
    Ast::Unit unit = __process_unit(node->getUnit(0), ctx);
    if (! unit.isLengthUnit()) {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a length unit.";
      throw err;
    }
    ctx.model().setLengthUnit(unit, false);
  } else if ("time" == node->getId()) {
    Ast::Unit unit = __process_unit(node->getUnit(0), ctx);
    if (! unit.isTimeUnit()) {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " required time unit.";
      throw err;
    }
    ctx.model().setTimeUnit(unit, false);
  }
}


void
Parser::Sbml::__process_unit_definition(LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *node, ParserContext &ctx)
{
  // Assemble ScaledUnits vector:
  Ast::Unit unit; // initially dimensionless
  for (size_t i=0; i<node->getNumUnits(); i++) {
    unit = unit * __process_unit(node->getUnit(i), ctx);
  }

  // Assemble UnitDefinition:
  ctx.defineUnit(node->getId(), unit);
}


Ast::Unit
Parser::Sbml::__process_unit(LIBSBML_CPP_NAMESPACE_QUALIFIER Unit *unit, ParserContext &ctx)
{
  // Dispatch by base unit:
  switch (unit->getKind())
  {
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_AMPERE:
    return Ast::Unit(Ast::ScaledBaseUnit::AMPERE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_AVOGADRO:
    return Ast::Unit(Ast::ScaledBaseUnit::AVOGADRO, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_BECQUEREL:
    return Ast::Unit(Ast::ScaledBaseUnit::BECQUEREL, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_CANDELA:
    return Ast::Unit(Ast::ScaledBaseUnit::CANDELA, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_CELSIUS:
    return Ast::Unit(Ast::ScaledBaseUnit::CELSIUS, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_COULOMB:
    return Ast::Unit(Ast::ScaledBaseUnit::COULOMB, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_DIMENSIONLESS:
    return Ast::Unit(Ast::ScaledBaseUnit::DIMENSIONLESS, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_FARAD:
    return Ast::Unit(Ast::ScaledBaseUnit::FARAD, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_GRAM:
    return Ast::Unit(Ast::ScaledBaseUnit::GRAM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_GRAY:
    return Ast::Unit(Ast::ScaledBaseUnit::GRAY, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_HENRY:
    return Ast::Unit(Ast::ScaledBaseUnit::HENRY, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_HERTZ:
    return Ast::Unit(Ast::ScaledBaseUnit::HERTZ, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_ITEM:
    return Ast::Unit(Ast::ScaledBaseUnit::ITEM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_JOULE:
    return Ast::Unit(Ast::ScaledBaseUnit::JOULE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_KATAL:
    return Ast::Unit(Ast::ScaledBaseUnit::KATAL, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_KELVIN:
    return Ast::Unit(Ast::ScaledBaseUnit::KELVIN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_KILOGRAM:
    return Ast::Unit(Ast::ScaledBaseUnit::KILOGRAM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_LITER:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_LITRE:
    return Ast::Unit(Ast::ScaledBaseUnit::LITRE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_LUMEN:
    return Ast::Unit(Ast::ScaledBaseUnit::LUMEN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_LUX:
    return Ast::Unit(Ast::ScaledBaseUnit::LUX, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_METER:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_METRE:
    return Ast::Unit(Ast::ScaledBaseUnit::METRE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_MOLE:
    return Ast::Unit(Ast::ScaledBaseUnit::MOLE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_NEWTON:
    return Ast::Unit(Ast::ScaledBaseUnit::NEWTON, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_OHM:
    return Ast::Unit(Ast::ScaledBaseUnit::OHM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_PASCAL:
    return Ast::Unit(Ast::ScaledBaseUnit::PASCAL, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_RADIAN:
    return Ast::Unit(Ast::ScaledBaseUnit::RADIAN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_SECOND:
    return Ast::Unit(Ast::ScaledBaseUnit::SECOND, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_SIEMENS:
    return Ast::Unit(Ast::ScaledBaseUnit::SIEMENS, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_SIEVERT:
    return Ast::Unit(Ast::ScaledBaseUnit::SIEVERT, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_STERADIAN:
    return Ast::Unit(Ast::ScaledBaseUnit::STERADIAN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_TESLA:
    return Ast::Unit(Ast::ScaledBaseUnit::TESLA, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_VOLT:
    return Ast::Unit(Ast::ScaledBaseUnit::VOLT, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_WATT:
    return Ast::Unit(Ast::ScaledBaseUnit::WATT, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_WEBER:
    return Ast::Unit(Ast::ScaledBaseUnit::WEBER, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_INVALID:
    break;
  }

  InternalError err;
  err << "Unknown UNIT KIND: " << (unsigned int) unit->getKind();
  throw err;
}


Ast::Reaction *
Parser::Sbml::__process_reaction(LIBSBML_CPP_NAMESPACE_QUALIFIER Reaction *node, ParserContext &ctx)
{
  if (! node->isSetKineticLaw()) {
    SemanticError err;
    err << "In Reaction " << node->getId() << ": A reaction needs a KineticLaw!";
    throw err;
  }

  // Assemble kinetic law and reaction:
  Ast::KineticLaw *law = __process_kinetic_law(node->getKineticLaw(), ctx);
  Ast::Reaction *reaction = new Ast::Reaction(node->getId(), law, node->getReversible());

  // Set name if defined:
  if (node->isSetName()) {
    reaction->setName(node->getName());
  }

  // Add reactants:
  for (size_t i=0; i<node->getNumReactants(); i++)
  {
    LIBSBML_CPP_NAMESPACE_QUALIFIER SpeciesReference *r = node->getReactant(i);
    GiNaC::ex expr;

    // If the reactant has a complex stochiometry:
    if (r->isSetStoichiometryMath()) {
      expr = __process_expression(r->getStoichiometryMath()->getMath(), ctx);
    } else {
      expr = GiNaC::numeric(r->getStoichiometry());
    }

    reaction->addReactantStoichiometry(ctx.resolveSpecies(r->getSpecies()), expr);
  }


  // Add products:
  for (size_t i=0; i<node->getNumProducts(); i++)
  {
    LIBSBML_CPP_NAMESPACE_QUALIFIER SpeciesReference *r = node->getProduct(i);
    GiNaC::ex expr;

    // If the product has a complex stochiometry:
    if (r->isSetStoichiometryMath()) {
      expr = __process_expression(r->getStoichiometryMath()->getMath(), ctx);
    } else {
      expr = GiNaC::numeric(r->getStoichiometry());
    }

    reaction->addProductStoichiometry(ctx.resolveSpecies(r->getSpecies()), expr);
  }

  for (size_t i=0; i<node->getNumModifiers(); i++) {
    reaction->addModifier(ctx.resolveSpecies(node->getModifier(i)->getSpecies()));
  }

  return reaction;
}


Ast::KineticLaw *
Parser::Sbml::__process_kinetic_law(LIBSBML_CPP_NAMESPACE_QUALIFIER KineticLaw *node, ParserContext &ctx)
{
  // Construct kinetic law from empty expression:
  Ast::KineticLaw *law = new Ast::KineticLaw(GiNaC::ex());

  // A KineticLaw may have its own parameters (local scope)
  ctx.pushScope(law);

  // Now populate Scope of KineticLaw with its parameters:
  for (size_t i=0; i<node->getNumParameters(); i++) {
    // Add new (local) parameter to KineticLaw
    law->addDefinition(__process_parameter_definition(node->getParameter(i), ctx));
  }

  // Construct kinetic law expression
  GiNaC::ex expr = __process_expression(node->getMath(), ctx);

  // Done
  ctx.popScope();

  // Set correct rate law
  law->setRateLaw(expr);

  return law;
}


GiNaC::ex
Parser::Sbml::__process_expression(const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx)
{
  /*
   * Big dispatcher for all expression types:
   */
  switch (node->getType())
  {
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_CONSTANT_E:
    return GiNaC::numeric(iNA::constants::e);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_CONSTANT_PI:
    return GiNaC::numeric(iNA::constants::pi);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_CONSTANT_TRUE:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_CONSTANT_FALSE:
    // Handle boolean constants:
    return __process_const_boolean(node, ctx);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_INTEGER:
    // Handle integer constants
    return __process_const_integer(node, ctx);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_REAL:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_REAL_E:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_RATIONAL:
    // Handle floating-point constants
    return __process_const_float(node, ctx);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_PLUS:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_MINUS:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_TIMES:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_DIVIDE:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_POWER:
    // Handle arithmetic operators:
    return __process_arithmetic_operator(node, ctx);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_NAME:
    // Handle variable symbols:
    return __process_symbol(node, ctx);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_NAME_TIME:
    // Handle "time" symbol
    return ctx.model().getTime();

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION:
    // Handle user-defined functions
    return __process_function_call(node, ctx);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_EXP:
    // Handle exponential, make sure, GiNaC handles the constant properly:
    return GiNaC::exp(__process_expression(node->getChild(0), ctx));

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_POWER:
    // Handle general power as function:
    return GiNaC::power(__process_expression(node->getChild(0), ctx),
                        __process_expression(node->getChild(1), ctx));

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ROOT:
    // Handle general root:
    return GiNaC::power(__process_expression(node->getChild(1), ctx),
                        1./__process_expression(node->getChild(0), ctx));

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_LN:
    return GiNaC::log(__process_expression(node->getChild(0), ctx));

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ABS:
    return GiNaC::abs(__process_expression(node->getChild(0), ctx));

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCCOS:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCCOSH:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCCOT:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCCOTH:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCCSC:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCCSCH:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCSEC:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCSECH:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCSIN:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCSINH:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCTAN:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ARCTANH:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_CEILING:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_COS:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_COSH:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_COT:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_COTH:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_CSC:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_CSCH:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_DELAY:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_FACTORIAL:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_FLOOR:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_LOG:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_PIECEWISE:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_SEC:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_SECH:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_SIN:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_SINH:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_TAN:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_TANH:
  {
    SBMLFeatureNotSupported err;
    err << "Can not handle built-in function " << node->getName()
        << ": function not supported yet.";
    throw err;
  }

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_LAMBDA:
  {
    SBMLFeatureNotSupported err;
    err << "Can not handle lambda expressions, feature not supported yet.";
    throw err;
  }

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_LOGICAL_AND:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_LOGICAL_NOT:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_LOGICAL_OR:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_LOGICAL_XOR:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_RELATIONAL_EQ:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_RELATIONAL_GEQ:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_RELATIONAL_GT:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_RELATIONAL_LEQ:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_RELATIONAL_LT:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_RELATIONAL_NEQ:
  {
    SBMLFeatureNotSupported err;
    err << "Can not handle built-in operator " << node->getName()
        << ": operator not supported yet.";
    throw err;
  }


  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_NAME_AVOGADRO:
  {
    SBMLFeatureNotSupported err;
    err << "Can not handle built-in constant " << node->getName()
        << ": constant not supported yet.";
    throw err;
  }

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_UNKNOWN:
  {
    SBMLParserError err;
    err << "Unknown SBML expression!";
    throw err;
  }
  }

  InternalError err;
  err << "Unhandled SBML AST node type: " << (unsigned int) node->getType();
  throw err;
}


GiNaC::ex
Parser::Sbml::__process_const_boolean(const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx)
{
  bool value;

  switch (node->getType())
  {
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_CONSTANT_TRUE:
    value = true;
    break;

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_CONSTANT_FALSE:
    value = false;
    break;

  default:
    InternalError err;
    err << "Unknown SBML boolean expression: " << (unsigned int) node->getType();
    return 0;
  }

  return GiNaC::numeric(value);
}


GiNaC::ex
Parser::Sbml::__process_const_integer(const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx)
{
  // Check if given node is an integer node:
  if (LIBSBML_CPP_NAMESPACE_QUALIFIER AST_INTEGER != node->getType())
  {
    InternalError err;
    err << "Unknown SBML integer type: " << (unsigned int) node->getType();
    throw err;
  }

  return GiNaC::numeric(node->getInteger());
}


GiNaC::ex
Parser::Sbml::__process_const_float(const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx)
{
  // Check if the given node is a floating point number (constant):
  switch (node->getType()) {
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_REAL:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_REAL_E:
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_RATIONAL:
    break;

  default:
    InternalError err;
    err << "Unknown SBML float type: " << (unsigned int) node->getType();
    throw err;
  }

  return GiNaC::numeric(node->getReal());
}


GiNaC::ex
Parser::Sbml::__process_arithmetic_operator(const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx)
{
  switch(node->getType())
  {
  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_PLUS:
    return __process_expression(node->getLeftChild(), ctx) +
        __process_expression(node->getRightChild(), ctx);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_MINUS:
    return __process_expression(node->getLeftChild(), ctx) -
        __process_expression(node->getRightChild(), ctx);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_TIMES:
    return __process_expression(node->getLeftChild(), ctx) *
        __process_expression(node->getRightChild(), ctx);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_DIVIDE:
    return __process_expression(node->getLeftChild(), ctx) /
        __process_expression(node->getRightChild(), ctx);

  case LIBSBML_CPP_NAMESPACE_QUALIFIER AST_POWER:
    return GiNaC::power(__process_expression(node->getLeftChild(), ctx),
                        __process_expression(node->getRightChild(), ctx));

  default:
    break;
  }

  InternalError err;
  err << "Unknown SBML arithmetic operator: " << (unsigned int) node->getType();
  throw err;
}


GiNaC::symbol
Parser::Sbml::__process_symbol(const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx)
{
  // Check if node is a symbol (name):
  if (LIBSBML_CPP_NAMESPACE_QUALIFIER AST_NAME != node->getType())
  {
    InternalError err;
    err << "Unknown SBML symbol type: " << (unsigned int) node->getType();
    throw err;
  }

  return ctx.resolveVariable(node->getName())->getSymbol();
}


GiNaC::ex
Parser::Sbml::__process_function_call(const LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *node, ParserContext &ctx)
{
  // Check if node is a user-defined function-call:
  if (LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION != node->getType()) {
    InternalError err;
    err << "Unknwon SBML function call type: " << (unsigned int) node->getType();
    throw err;
  }

  // Get Function by name:
  Ast::FunctionDefinition *func = ctx.model().getFunction(node->getName());

  // Get function argument expressions:
  std::vector<GiNaC::ex> arguments(node->getNumChildren());
  for (size_t i=0; i<node->getNumChildren(); i++)
  {
    // construct Expression from each argument
    arguments[i] = __process_expression(node->getChild(i), ctx);
  }

  // Finally return inlined function call:
  return func->inlineFunction(arguments);
}
