#include "assembler.hh"
#include "math.hh"


using namespace Fluc;
using namespace Fluc::Parser::Sbml;


/* ********************************************************************************************* *
 * Implementation of ParserContext...
 * ********************************************************************************************* */
ParserContext::ParserContext(Ast::Model &model)
  : _model(model)
{
  // Store pointer to module (also a scope) into scope stack:
  this->_scope_stack.push_back(&_model);
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



/* ******************************************************************************************** *
 * Implementation of helper functions to assemble the Ast::Model from a SBML model
 * ******************************************************************************************** */
void
Parser::Sbml::__process_model(libsbml::Model *model, ParserContext &ctx)
{
  // Set name of model:
  if (model->isSetName()) {
    ctx.model().setName(model->getName());
  }

  /* Process all function definitions in model by forwarding them to
   * __process_function_definition. */
  for (size_t i=0; i<model->getNumFunctionDefinitions(); i++)
  {
    libsbml::FunctionDefinition *func_def = model->getFunctionDefinition(i);
    Ast::FunctionDefinition *def = __process_function_definition(func_def, ctx);
    ctx.model().addDefinition(def);
  }

  /* First of all, process unit-definitions. */
  for (size_t i=0; i<model->getNumUnitDefinitions(); i++)
  {
    libsbml::UnitDefinition *sbml_unit = model->getUnitDefinition(i);

    if (__is_default_unit_redefinition(sbml_unit, ctx)) {
      __process_default_unit_redefinition(sbml_unit, ctx);
      continue;
    }

    Ast::UnitDefinition *unit = __process_unit_definition(sbml_unit, ctx);
    ctx.model().addDefinition(unit);
  }

  /* Process all parameters of the model by forwarding them to processParameterDefinition. */
  for (size_t i=0; i<model->getNumParameters(); i++)
  {
    // Get parameter
    libsbml::Parameter *sbml_param = model->getParameter(i);
    // Turn into Ast::ParameterDefinition
    Ast::VariableDefinition *param = __process_parameter_definition(sbml_param, ctx);
    // Store definition into global module:
    ctx.model().addDefinition(param);
  }

  /* Process all compartments defined in the SBML model. */
  for (size_t i=0; i<model->getNumCompartments(); i++)
  {
    // Get SBML compartment
    libsbml::Compartment *comp = model->getCompartment(i);
    // Translate into AST
    Ast::VariableDefinition *comp_def = __process_compartment_definition(comp, ctx);
    // Store in module:
    ctx.model().addDefinition(comp_def);
  }

  /* Process all species defined in the model. */
  for (size_t i=0; i<model->getNumSpecies(); i++)
  {
    // Get species:
    libsbml::Species *sp = model->getSpecies(i);
    // convert
    Ast::VariableDefinition *sp_def = __process_species_definition(sp, ctx);
    // store
    ctx.model().addDefinition(sp_def);
  }

  /* Process all initial value assignments */
  for (size_t i=0; i<model->getNumInitialAssignments(); i++)
  {
    libsbml::InitialAssignment *ass = model->getInitialAssignment(i);
    Ast::VariableDefinition *var_def = ctx.model().getVariable(ass->getId());
    // Construct initial value expression:
    GiNaC::ex value = __process_expression(ass->getMath(), ctx);
    // Replace 'old' initial value expression:
    var_def->setValue(value);
  }

  /* Process rules for variables: */
  for (size_t i=0; i<model->getNumRules(); i++)
  {
    // Get SBML rule:
    libsbml::Rule *sbml_rule = model->getRule(i);
    // Construct expression of rule:
    GiNaC::ex rule_expr = __process_expression(sbml_rule->getMath(), ctx);

    // Dispatch by type:
    if (sbml_rule->isAssignment())
    {
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
    }
    else if (sbml_rule->isRate())
    {
      // Find variable the rule is associated with:
      if (! ctx.model().hasDefinition(sbml_rule->getVariable()))
      {
        SymbolError err;
        err << "Variable " << sbml_rule->getVariable()
            << " in rate rule not defined in module.";
        throw err;
      }

      // Check if definition is a variable definition.
      Ast::VariableDefinition *def = ctx.model().getVariable(sbml_rule->getVariable());
      // Construct rule and assign it to variable:
      def->setRule(new Ast::RateRule(rule_expr));
    }
    else if(sbml_rule->isAlgebraic())
    {
      ctx.model().addConstraint(new Ast::AlgebraicConstraint(rule_expr));
    }
    else
    {
      InternalError err;
      err << "Unknown variable-rule type.";
      throw err;
    }
  }

  /* Process all reactions, especially all the kinetic laws of them. The identifier of the kinetic
   * law is taken as the identifier of the reaction.
   */
  for (size_t i=0; i<model->getNumReactions(); i++)
  {
    // Get SBML Reaction object from model
    libsbml::Reaction *reac = model->getReaction(i);
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
}


Ast::VariableDefinition *
Parser::Sbml::__process_species_definition(libsbml::Species *node, ParserContext &ctx)
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
  Ast::Unit unit(ctx.model().getDefaultSubstanceUnit());

  // Get unit for species:
  if (node->isSetUnits()) {
    unit = ctx.model().getUnit(node->getUnits());
  }

  // If the flag "hasOnlySubstanceUnits == false" (default)
  // => Unit(Species) = SubstanceUnit/Volume
  if (! node->getHasOnlySubstanceUnits()) {
    unit = unit / compartment->getUnit();
  }

  /* Setup initial value. */
  GiNaC::ex init_value;
  // If unit of species is substance (mole, gram, ...)
  if (node->getHasOnlySubstanceUnits())
  {
    if (node->isSetInitialAmount()) {
      init_value = GiNaC::numeric(node->getInitialAmount());
    } else if (node->isSetInitialConcentration()) {
      // If a concentration is given but the units of the species is amount:
      init_value = GiNaC::numeric(node->getInitialConcentration())*compartment->getSymbol();
    }
  }
  // If unit of species is concentration
  else
  {
    if (node->isSetInitialAmount()) {
      // If an amount is given but the unit if the species is concentration:
      init_value = GiNaC::numeric(node->getInitialAmount())/compartment->getSymbol();
    } else if (node->isSetInitialConcentration()) {
      init_value = GiNaC::numeric(node->getInitialConcentration());
    }
  }

  // Assemble species definition
  return new Ast::Species(node->getId(), init_value, unit, node->getHasOnlySubstanceUnits(),
                          compartment, node->getName(), node->getConstant());
}


Ast::FunctionDefinition *
Parser::Sbml::__process_function_definition(libsbml::FunctionDefinition *funcdef, ParserContext &ctx)
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
Parser::Sbml::__process_parameter_definition(libsbml::Parameter *node, ParserContext &ctx)
{
  GiNaC::ex init_value;
  if (node->isSetValue()) {
    init_value = GiNaC::numeric(node->getValue());
  }

  // Get units for parameter (it there is one):
  Ast::Unit unit(Ast::Unit::dimensionless());
  if (node->isSetUnits()) {
    unit = ctx.model().getUnit(node->getUnits());
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
Parser::Sbml::__process_compartment_definition(libsbml::Compartment *node, ParserContext &ctx)
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

  Ast::Unit unit(ctx.model().getDefaultVolumeUnit());

  // Check if there is a Unit assigned to the compartment:
  if (node->isSetUnits()) {
    unit = ctx.model().getUnit(node->getUnits());
  }

  // Get Spatial dimension of compartment
  Ast::Compartment::SpatialDimension sp_dim = Ast::Compartment::VOLUME;
  if (node->isSetSpatialDimensions()) {
    switch(node->getSpatialDimensions())
    {
    case 0: sp_dim = Ast::Compartment::POINT;  break;
    case 1: sp_dim = Ast::Compartment::LINE;   break;
    case 2: sp_dim = Ast::Compartment::AREA;   break;
    case 3: sp_dim = Ast::Compartment::VOLUME; break;
    default:
    {
      SemanticError err;
      err << "Can not define compartment " << node->getId()
          << " unkown spacial dimension " << node->getSpatialDimensions();
      throw err;
    }
    }
  }

  // Assemble and return compartment:
  Ast::Compartment *comp =
      new Ast::Compartment(node->getId(), init_value, unit, sp_dim, node->getConstant());

  // Set display name if defined:
  if (node->isSetName()) {
    comp->setName(node->getName());
  }

  return comp;
}


bool
Parser::Sbml::__is_default_unit_redefinition(libsbml::UnitDefinition *node, ParserContext &ctx)
{
  if ("substance" == node->getId() || "volume" == node->getId() || "area" == node->getId() ||
      "length" == node->getId() || "time" == node->getId()) {
    return true;
  }

  return false;
}


void
Parser::Sbml::__process_default_unit_redefinition(libsbml::UnitDefinition *node, ParserContext &ctx)
{
  // Check number of units:
  if (1 != node->getNumUnits()) {
    SemanticError err;
    err << "Redefinition of default unit " << node->getId() << " must contain exactly one "
        << " unit element! Found: " << node->getNumUnits();
    throw err;
  }

  if ("substance" == node->getId()) {
    Ast::ScaledBaseUnit unit = __process_unit(node->getUnit(0), ctx);
    if (! unit.isSubstanceUnit()) {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a substance unit.";
      throw err;
    }
    ctx.model().setDefaultSubstanceUnit(unit);
  } else if ("volume" == node->getId()) {
    Ast::ScaledBaseUnit unit = __process_unit(node->getUnit(0), ctx);
    if (! unit.isVolumeUnit()) {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a volume unit.";
      throw err;
    }
    ctx.model().setDefaultVolumeUnit(unit);
  } else if ("area" == node->getId()) {
    Ast::ScaledBaseUnit unit = __process_unit(node->getUnit(0), ctx);
    if (! unit.isAreaUnit()) {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a area unit.";
      throw err;
    }
    ctx.model().setDefaultAreaUnit(unit);
  } else if ("length" == node->getId()) {
    Ast::ScaledBaseUnit unit = __process_unit(node->getUnit(0), ctx);
    if (! unit.isLengthUnit()) {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a length unit.";
      throw err;
    }
    ctx.model().setDefaultLengthUnit(unit);
  } else if ("time" == node->getId()) {
    Ast::ScaledBaseUnit unit = __process_unit(node->getUnit(0), ctx);
    if (! unit.isTimeUnit()) {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " required time unit.";
      throw err;
    }
    ctx.model().setDefaultTimeUnit(unit);
  }
}


Ast::UnitDefinition *
Parser::Sbml::__process_unit_definition(libsbml::UnitDefinition *node, ParserContext &ctx)
{
  // Assemble ScaledUnits vector:
  std::list<Ast::ScaledBaseUnit> units;
  for (size_t i=0; i<node->getNumUnits(); i++) {
    units.push_back(__process_unit(node->getUnit(0), ctx));
  }

  // Assemble UnitDefinition:
  return new Ast::UnitDefinition(node->getId(), units);
}


Ast::ScaledBaseUnit
Parser::Sbml::__process_unit(libsbml::Unit *unit, ParserContext &ctx)
{
  // Dispatch by base unit:
  switch (unit->getKind())
  {
  case libsbml::UNIT_KIND_AMPERE:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::AMPERE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_AVOGADRO:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::AVOGADRO, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_BECQUEREL:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::BECQUEREL, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_CANDELA:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::CANDELA, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_CELSIUS:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::CELSIUS, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_COULOMB:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::COULOMB, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_DIMENSIONLESS:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::DIMENSIONLESS, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_FARAD:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::FARAD, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_GRAM:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::GRAM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_GRAY:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::GRAY, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_HENRY:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::HENRY, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_HERTZ:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::HERTZ, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_ITEM:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::ITEM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_JOULE:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::JOULE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_KATAL:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::KATAL, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_KELVIN:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::KELVIN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_KILOGRAM:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::KILOGRAM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_LITER:
  case libsbml::UNIT_KIND_LITRE:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::LITRE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_LUMEN:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::LUMEN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_LUX:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::LUX, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_METER:
  case libsbml::UNIT_KIND_METRE:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::METRE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_MOLE:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::MOLE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_NEWTON:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::NEWTON, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_OHM:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::OHM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_PASCAL:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::PASCAL, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_RADIAN:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::RADIAN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_SECOND:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::SECOND, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_SIEMENS:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::SIEMENS, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_SIEVERT:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::SIEVERT, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_STERADIAN:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::STERADIAN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_TESLA:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::TESLA, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_VOLT:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::VOLT, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_WATT:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::WATT, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_WEBER:
    return Ast::ScaledBaseUnit(Ast::ScaledBaseUnit::WEBER, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_INVALID:
    break;
  }

  InternalError err;
  err << "Unknown UNIT KIND: " << (unsigned int) unit->getKind();
  throw err;
}


Ast::Reaction *
Parser::Sbml::__process_reaction(libsbml::Reaction *node, ParserContext &ctx)
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
    libsbml::SpeciesReference *r = node->getReactant(i);
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
    libsbml::SpeciesReference *r = node->getProduct(i);
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
Parser::Sbml::__process_kinetic_law(libsbml::KineticLaw *node, ParserContext &ctx)
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
Parser::Sbml::__process_expression(const libsbml::ASTNode *node, ParserContext &ctx)
{
  /*
   * Big dispatcher for all expression types:
   */
  switch (node->getType())
  {
  case libsbml::AST_CONSTANT_E:
    return GiNaC::numeric(Fluc::constants::e);

  case libsbml::AST_CONSTANT_PI:
    return GiNaC::numeric(Fluc::constants::pi);

  case libsbml::AST_CONSTANT_TRUE:
  case libsbml::AST_CONSTANT_FALSE:
    // Handle boolean constants:
    return __process_const_boolean(node, ctx);

  case libsbml::AST_INTEGER:
    // Handle integer constants
    return __process_const_integer(node, ctx);

  case libsbml::AST_REAL:
  case libsbml::AST_REAL_E:
  case libsbml::AST_RATIONAL:
    // Handle floating-point constants
    return __process_const_float(node, ctx);

  case libsbml::AST_PLUS:
  case libsbml::AST_MINUS:
  case libsbml::AST_TIMES:
  case libsbml::AST_DIVIDE:
  case libsbml::AST_POWER:
    // Handle arithmetic operators:
    return __process_arithmetic_operator(node, ctx);

  case libsbml::AST_NAME:
    // Handle variable symbols:
    return __process_symbol(node, ctx);

  case libsbml::AST_NAME_TIME:
    // Handle "time" symbol
    return ctx.model().getTime();

  case libsbml::AST_FUNCTION:
    // Handle user-defined functions
    return __process_function_call(node, ctx);

  case libsbml::AST_FUNCTION_EXP:
    // Handle exponential, make sure, GiNaC handles the constant properly:
    return GiNaC::exp(__process_expression(node->getChild(0), ctx));

  case libsbml::AST_FUNCTION_POWER:
    // Handle general power as function:
    return GiNaC::power(__process_expression(node->getChild(0), ctx),
                        __process_expression(node->getChild(1), ctx));

  case libsbml::AST_FUNCTION_ROOT:
    // Handle general root:
    return GiNaC::power(__process_expression(node->getChild(1), ctx),
                        1./__process_expression(node->getChild(0), ctx));

  case libsbml::AST_FUNCTION_LN:
    return GiNaC::log(__process_expression(node->getChild(0), ctx));

  case libsbml::AST_FUNCTION_ABS:
    return GiNaC::abs(__process_expression(node->getChild(0), ctx));

  case libsbml::AST_FUNCTION_ARCCOS:
  case libsbml::AST_FUNCTION_ARCCOSH:
  case libsbml::AST_FUNCTION_ARCCOT:
  case libsbml::AST_FUNCTION_ARCCOTH:
  case libsbml::AST_FUNCTION_ARCCSC:
  case libsbml::AST_FUNCTION_ARCCSCH:
  case libsbml::AST_FUNCTION_ARCSEC:
  case libsbml::AST_FUNCTION_ARCSECH:
  case libsbml::AST_FUNCTION_ARCSIN:
  case libsbml::AST_FUNCTION_ARCSINH:
  case libsbml::AST_FUNCTION_ARCTAN:
  case libsbml::AST_FUNCTION_ARCTANH:
  case libsbml::AST_FUNCTION_CEILING:
  case libsbml::AST_FUNCTION_COS:
  case libsbml::AST_FUNCTION_COSH:
  case libsbml::AST_FUNCTION_COT:
  case libsbml::AST_FUNCTION_COTH:
  case libsbml::AST_FUNCTION_CSC:
  case libsbml::AST_FUNCTION_CSCH:
  case libsbml::AST_FUNCTION_DELAY:
  case libsbml::AST_FUNCTION_FACTORIAL:
  case libsbml::AST_FUNCTION_FLOOR:
  case libsbml::AST_FUNCTION_LOG:
  case libsbml::AST_FUNCTION_PIECEWISE:
  case libsbml::AST_FUNCTION_SEC:
  case libsbml::AST_FUNCTION_SECH:
  case libsbml::AST_FUNCTION_SIN:
  case libsbml::AST_FUNCTION_SINH:
  case libsbml::AST_FUNCTION_TAN:
  case libsbml::AST_FUNCTION_TANH:
  {
    SBMLFeatureNotSupported err;
    err << "Can not handle built-in function " << node->getName()
        << ": function not supported yet.";
    throw err;
  }

  case libsbml::AST_LAMBDA:
  {
    SBMLFeatureNotSupported err;
    err << "Can not handle lambda expressions, feature not supported yet.";
    throw err;
  }

  case libsbml::AST_LOGICAL_AND:
  case libsbml::AST_LOGICAL_NOT:
  case libsbml::AST_LOGICAL_OR:
  case libsbml::AST_LOGICAL_XOR:
  case libsbml::AST_RELATIONAL_EQ:
  case libsbml::AST_RELATIONAL_GEQ:
  case libsbml::AST_RELATIONAL_GT:
  case libsbml::AST_RELATIONAL_LEQ:
  case libsbml::AST_RELATIONAL_LT:
  case libsbml::AST_RELATIONAL_NEQ:
  {
    SBMLFeatureNotSupported err;
    err << "Can not handle built-in operator " << node->getName()
        << ": operator not supported yet.";
    throw err;
  }


  case libsbml::AST_NAME_AVOGADRO:
  {
    SBMLFeatureNotSupported err;
    err << "Can not handle built-in constant " << node->getName()
        << ": constant not supported yet.";
    throw err;
  }

  case libsbml::AST_UNKNOWN:
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
Parser::Sbml::__process_const_boolean(const libsbml::ASTNode *node, ParserContext &ctx)
{
  bool value;

  switch (node->getType())
  {
  case libsbml::AST_CONSTANT_TRUE:
    value = true;
    break;

  case libsbml::AST_CONSTANT_FALSE:
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
Parser::Sbml::__process_const_integer(const libsbml::ASTNode *node, ParserContext &ctx)
{
  // Check if given node is an integer node:
  if (libsbml::AST_INTEGER != node->getType())
  {
    InternalError err;
    err << "Unknown SBML integer type: " << (unsigned int) node->getType();
    throw err;
  }

  return GiNaC::numeric(node->getInteger());
}


GiNaC::ex
Parser::Sbml::__process_const_float(const libsbml::ASTNode *node, ParserContext &ctx)
{
  // Check if the given node is a floating point number (constant):
  switch (node->getType()) {
  case libsbml::AST_REAL:
  case libsbml::AST_REAL_E:
  case libsbml::AST_RATIONAL:
    break;

  default:
    InternalError err;
    err << "Unknown SBML float type: " << (unsigned int) node->getType();
    throw err;
  }

  return GiNaC::numeric(node->getReal());
}


GiNaC::ex
Parser::Sbml::__process_arithmetic_operator(const libsbml::ASTNode *node, ParserContext &ctx)
{
  switch(node->getType())
  {
  case libsbml::AST_PLUS:
    return __process_expression(node->getLeftChild(), ctx) +
        __process_expression(node->getRightChild(), ctx);

  case libsbml::AST_MINUS:
    return __process_expression(node->getLeftChild(), ctx) -
        __process_expression(node->getRightChild(), ctx);

  case libsbml::AST_TIMES:
    return __process_expression(node->getLeftChild(), ctx) *
        __process_expression(node->getRightChild(), ctx);

  case libsbml::AST_DIVIDE:
    return __process_expression(node->getLeftChild(), ctx) /
        __process_expression(node->getRightChild(), ctx);

  case libsbml::AST_POWER:
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
Parser::Sbml::__process_symbol(const libsbml::ASTNode *node, ParserContext &ctx)
{
  // Check if node is a symbol (name):
  if (libsbml::AST_NAME != node->getType())
  {
    InternalError err;
    err << "Unknown SBML symbol type: " << (unsigned int) node->getType();
    throw err;
  }

  return ctx.resolveVariable(node->getName())->getSymbol();
}


GiNaC::ex
Parser::Sbml::__process_function_call(const libsbml::ASTNode *node, ParserContext &ctx)
{
  // Check if node is a user-defined function-call:
  if (libsbml::AST_FUNCTION != node->getType()) {
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
