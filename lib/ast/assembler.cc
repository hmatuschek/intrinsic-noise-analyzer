#include "assembler.hh"
#include "math.hh"


using namespace Fluc::Ast;


Assembler::Assembler(Ast::Model &model)
  : model(model)
{
  // Store pointer to module (also a scope) into scope stack:
  this->scope_stack.push_back(&model);
}


void
Assembler::processModel(libsbml::Model *model)
{
  // Set name of model:
  if (model->isSetName())
  {
    this->model.setName(model->getName());
  }

  /*
   * Process all function definitions in model by forwarding them to
   * processFunctionDefinition.
   */
  for (size_t i=0; i<model->getNumFunctionDefinitions(); i++)
  {
    libsbml::FunctionDefinition *func_def = model->getFunctionDefinition(i);
    FunctionDefinition *def = this->processFunctionDefinition(func_def);
    this->model.addDefinition(def);
  }

  /*
   * First of all, process unit-definitions.
   */
  for (size_t i=0; i<model->getNumUnitDefinitions(); i++)
  {
    libsbml::UnitDefinition *sbml_unit = model->getUnitDefinition(i);

    if (this->isDefaultUnitRedefinition(sbml_unit))
    {
      this->processDefaultUnitRedefinition(sbml_unit);
      continue;
    }

    UnitDefinition *unit = this->processUnitDefinition(sbml_unit);
    this->model.addDefinition(unit);
  }

  /*
   * Process all parameters of the model by forwarding them to processParameterDefinition.
   */
  for (size_t i=0; i<model->getNumParameters(); i++)
  {
    // Get parameter
    libsbml::Parameter *sbml_param = model->getParameter(i);
    // Turn into Ast::ParameterDefinition
    VariableDefinition *param = this->processParameterDefinition(sbml_param);
    // Store definition into global module:
    this->model.addDefinition(param);
  }

  /*
   * Process all compartments defined in the SBML model.
   */
  for (size_t i=0; i<model->getNumCompartments(); i++)
  {
    // Get SBML compartment
    libsbml::Compartment *comp = model->getCompartment(i);
    // Translate into AST
    VariableDefinition *comp_def = this->processCompartmentDefinition(comp);
    // Store in module:
    this->model.addDefinition(comp_def);
  }

  /*
   * Process all species defined in the model.
   */
  for (size_t i=0; i<model->getNumSpecies(); i++)
  {
    // Get species:
    libsbml::Species *sp = model->getSpecies(i);
    // convert
    VariableDefinition *sp_def = this->processSpeciesDefinition(sp);
    // store
    this->model.addDefinition(sp_def);
  }

  /*
   * Process all initial value assignments
   */
  for (size_t i=0; i<model->getNumInitialAssignments(); i++)
  {
    libsbml::InitialAssignment *ass = model->getInitialAssignment(i);
    VariableDefinition *var_def = this->model.getVariable(ass->getId());

    // Construct initial value expression:
    GiNaC::ex value = this->processExpression(ass->getMath());

    // Replace 'old' initial value expression:
    var_def->setValue(value);
  }

  /*
   * Process rules for variables:
   */
  for (size_t i=0; i<model->getNumRules(); i++)
  {
    // Get SBML rule:
    libsbml::Rule *sbml_rule = model->getRule(i);
    // Construct expression of rule:
    GiNaC::ex rule_expr = this->processExpression(sbml_rule->getMath());

    // Dispatch by type:
    if (sbml_rule->isAssignment())
    {
      // Find variable the rule is associated with:
      if (! this->model.hasDefinition(sbml_rule->getVariable()))
      {
        SymbolError err;
        err << "Variable " << sbml_rule->getVariable()
            << " in assignment rule not defined in module.";
        throw err;
      }

      // Check if definition is a variable definition.
      Ast::VariableDefinition *def = this->model.getVariable(sbml_rule->getVariable());

      // Construct rule and assign it to variable:
      def->setRule(new Ast::AssignmentRule(rule_expr));

      /*std::cerr << "Construct assignment rule for: " << def->getSymbol()
                << " with rule " << rule_expr << std::endl;*/
    }
    else if (sbml_rule->isRate())
    {
      // Find variable the rule is associated with:
      if (! this->model.hasDefinition(sbml_rule->getVariable()))
      {
        SymbolError err;
        err << "Variable " << sbml_rule->getVariable()
            << " in rate rule not defined in module.";
        throw err;
      }

      // Check if definition is a variable definition.
      Ast::VariableDefinition *def = this->model.getVariable(sbml_rule->getVariable());

      // Construct rule and assign it to variable:
      def->setRule(new Ast::RateRule(rule_expr));
    }
    else if(sbml_rule->isAlgebraic())
    {
      this->model.addConstraint(new Ast::AlgebraicConstraint(rule_expr));
    }
    else
    {
      InternalError err;
      err << "Unknown variable-rule type.";
      throw err;
    }
  }

  /*
   * Process all reactions, especially all the kinetic laws of them. The identifier of the kinetic
   * law is taken as the identifier of the reaction.
   */
  for (size_t i=0; i<model->getNumReactions(); i++)
  {
    // Get SBML Reaction object from model
    libsbml::Reaction *reac = model->getReaction(i);
    Reaction *ast_reac = this->processReaction(reac);
    // store in module:
    this->model.addDefinition(ast_reac);
  }

  /*
   * Check if there are any Events defined, they are not supported by L. Nassa!
   */
  if (0 != model->getNumEvents())
  {
    SBMLFeatureNotSupported err;
    err << "Can not handle event definitions. Featrue not supported.";
    throw err;
  }
}


VariableDefinition *
Assembler::processSpeciesDefinition(libsbml::Species *node)
{


  /*
  * First, get compartment of species.
  */
  // Get compartment for species:
  if (! this->model.hasCompartment(node->getCompartment()))
  {
      // Oops:
      SymbolError err;
      err << "Can not assemble species definition " << node->getId()
          << ", compartment "<< node->getCompartment() << " not defined.";
      throw err;
  }
  Compartment *compartment = this->model.getCompartment(node->getCompartment());

  /*
  * Second, process unit and initial value of species.
  */
  Unit unit(this->model.getDefaultSubstanceUnit());

  // Get unit for species:
  if (node->isSetUnits())
  {
    unit = this->model.getUnit(node->getUnits());
  }

//  GiNaC::ex init_value;

//  if (node->isSetInitialAmount())
//  {
//      init_value = GiNaC::numeric(node->getInitialAmount());
//  }
//  else if (node->isSetInitialConcentration())
//  {

//      init_value = GiNaC::numeric(node->getInitialConcentration());

//      if(!node->getHasOnlySubstanceUnits())
//      {
//          // make concentration unit
//          unit = unit / compartment->getUnit();
//      }
//      else
//      {
//         // if initial value is concentration but species has substance units
//         init_value *= compartment->getSymbol();
//      }

//  }
//  else
//  {
//      throw InternalError("Initial value of species must be set by either amount or concentration.");
//  }


  // If the flag "hasOnlySubstanceUnits == false" (default)
  // => Unit(Species) = SubstanceUnit/Volume
  if (! node->getHasOnlySubstanceUnits())
  {
    unit = unit / compartment->getUnit();
  }

  GiNaC::ex init_value;
  /*
   * Setup initial value.
   */
  // If unit of species is substance (mole, gram, ...)
  if (node->getHasOnlySubstanceUnits())
  {
    if (node->isSetInitialAmount())
    {
      init_value = GiNaC::numeric(node->getInitialAmount());
    }
    else if (node->isSetInitialConcentration())
    {
      // If a concentration is given but the units of the species is amount:
      init_value = GiNaC::numeric(node->getInitialConcentration())*compartment->getSymbol();
    }
  }
  // If unit of species is concentration
  else
  {
    if (node->isSetInitialAmount())
    {
      // If an amount is given but the unit if the species is concentration:
      init_value = GiNaC::numeric(node->getInitialAmount())/compartment->getSymbol();
    }
    else if (node->isSetInitialConcentration())
    {
      init_value = GiNaC::numeric(node->getInitialConcentration());
    }
  }

  // Assemble species definition
  return new Species(node->getId(), init_value, unit, node->getHasOnlySubstanceUnits(), compartment,
                     node->getName(), node->getConstant());
}


void
Assembler::pushScope(Scope *scope)
{
  this->scope_stack.push_back(scope);
}


void
Assembler::popScope()
{
  this->scope_stack.pop_back();
}


Scope *
Assembler::currentScope()
{
  return this->scope_stack.back();
}


Species *
Assembler::resolveSpecies(const std::string &id)
{
  VariableDefinition *var = this->resolveVariable(id);

  if (! Node::isSpecies(var))
  {
    SymbolError err;
    err << "There is no species named " << id << " defined.";
    throw err;
  }

  return static_cast<Species *>(var);
}


VariableDefinition *
Assembler::resolveVariable(const std::string &id)
{
  // Start reverse search variable in scope stack:
  return this->resolveVariable(id, this->scope_stack.rbegin());
}


VariableDefinition *
Assembler::resolveVariable(const std::string &id, std::list<Ast::Scope *>::reverse_iterator scope)
{
  if (this->scope_stack.rend() == scope)
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


FunctionDefinition *
Assembler::processFunctionDefinition(libsbml::FunctionDefinition *funcdef)
{
  // Extract identifier:
  std::string id = funcdef->getId();

  // Extract and assemble function-arguments
  std::vector<VariableDefinition *> args(funcdef->getNumArguments());
  for (size_t i=0; i<funcdef->getNumArguments(); i++) {
    // Construct a new VariableDefinition for the argument:
    args[i]= new FunctionArgument(funcdef->getArgument(i)->getName());
  }

  // Assemble and return function definition
  Ast::FunctionDefinition *def = new Ast::FunctionDefinition(id, args, GiNaC::ex());

  // Push scope of function, function body is processed in this scope:
  this->pushScope(def);

  // Extract and assemble function body
  GiNaC::ex body = this->processExpression(funcdef->getBody());

  // Done. pop scope:
  this->popScope();

  // Set body of function:
  def->setBody(body);

  // Done.
  return def;

}



VariableDefinition *
Assembler::processParameterDefinition(libsbml::Parameter *node)
{
  GiNaC::ex init_value;
  if (node->isSetValue())
  {
    init_value = GiNaC::numeric(node->getValue());
  }

  Unit unit(Unit::dimensionless());

  // Get units for parameter (it there is one):
  if (node->isSetUnits())
  {
    unit = this->model.getUnit(node->getUnits());
  }

  // Construct and return parameter definition (constant variable)
  Ast::Parameter *param = new Ast::Parameter(node->getId(), init_value, unit, node->getConstant());

  // Set display name if defined:
  if (node->isSetName()) {
    param->setName(node->getName());
  }

  return param;
}


VariableDefinition *
Assembler::processCompartmentDefinition(libsbml::Compartment *node)
{
  if (3 != node->getSpatialDimensions())
  {
    SBMLFeatureNotSupported err;
    err << "Spacial dimension of compartment must be 3!";
    throw err;
  }

  if (node->isSetOutside())
  {
    SBMLFeatureNotSupported err;
    err << "No nested compartments are supported.";
    throw err;
  }

  // Check if there is an size value:
  GiNaC::ex init_value;
  if (node->isSetSize())
  {
    init_value = GiNaC::numeric(node->getSize());
  }

  Unit unit(this->model.getDefaultVolumeUnit());

  // Check if there is a Unit assigned to the compartment:
  if (node->isSetUnits())
  {
    unit = this->model.getUnit(node->getUnits());
  }

  // Get Spatial dimension of compartment
  Ast::Compartment::SpatialDimension sp_dim = Ast::Compartment::VOLUME;
  if (node->isSetSpatialDimensions())
  {
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
Assembler::isDefaultUnitRedefinition(libsbml::UnitDefinition *node)
{
  if ("substance" == node->getId() || "volume" == node->getId() || "area" == node->getId() ||
      "length" == node->getId() || "time" == node->getId())
  {
    return true;
  }

  return false;
}


void
Assembler::processDefaultUnitRedefinition(libsbml::UnitDefinition *node)
{
  // Check number of units:
  if (1 != node->getNumUnits())
  {
    SemanticError err;
    err << "Redefinition of default unit " << node->getId() << " must contain exactly one "
        << " unit element! Found: " << node->getNumUnits();
    throw err;
  }

  if ("substance" == node->getId())
  {
    ScaledBaseUnit unit = this->processUnit(node->getUnit(0));

    if (! unit.isSubstanceUnit())
    {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a substance unit.";
      throw err;
    }

    this->model.setDefaultSubstanceUnit(unit);
  }
  else if ("volume" == node->getId())
  {
    ScaledBaseUnit unit = this->processUnit(node->getUnit(0));

    if (! unit.isVolumeUnit())
    {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a volume unit.";
      throw err;
    }

    this->model.setDefaultVolumeUnit(unit);
  }
  else if ("area" == node->getId())
  {
    ScaledBaseUnit unit = this->processUnit(node->getUnit(0));

    if (! unit.isAreaUnit())
    {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a area unit.";
      throw err;
    }

    this->model.setDefaultAreaUnit(unit);
  }
  else if ("length" == node->getId())
  {
    ScaledBaseUnit unit = this->processUnit(node->getUnit(0));

    if (! unit.isLengthUnit())
    {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " requires a length unit.";
      throw err;
    }

    this->model.setDefaultLengthUnit(unit);
  }
  else if ("time" == node->getId())
  {
    ScaledBaseUnit unit = this->processUnit(node->getUnit(0));

    if (! unit.isTimeUnit())
    {
      SemanticError err;
      err << "Redefinition of default unit " << node->getId() << " required time unit.";
      throw err;
    }

    this->model.setDefaultTimeUnit(unit);

  }
}


UnitDefinition *
Assembler::processUnitDefinition(libsbml::UnitDefinition *node)
{
  // Assemble ScaledUnits vector:
  std::list<ScaledBaseUnit> units;
  for (size_t i=0; i<node->getNumUnits(); i++)
  {
    units.push_back(this->processUnit(node->getUnit(0)));
  }

  // Assemble UnitDefinition:
  return new UnitDefinition(node->getId(), units);
}


ScaledBaseUnit
Assembler::processUnit(libsbml::Unit *unit)
{
  // Dispatch by base unit:
  switch (unit->getKind())
  {
  case libsbml::UNIT_KIND_AMPERE:
    return ScaledBaseUnit(ScaledBaseUnit::AMPERE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_AVOGADRO:
    return ScaledBaseUnit(ScaledBaseUnit::AVOGADRO, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_BECQUEREL:
    return ScaledBaseUnit(ScaledBaseUnit::BECQUEREL, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_CANDELA:
    return ScaledBaseUnit(ScaledBaseUnit::CANDELA, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_CELSIUS:
    return ScaledBaseUnit(ScaledBaseUnit::CELSIUS, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_COULOMB:
    return ScaledBaseUnit(ScaledBaseUnit::COULOMB, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_DIMENSIONLESS:
    return ScaledBaseUnit(ScaledBaseUnit::DIMENSIONLESS, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_FARAD:
    return ScaledBaseUnit(ScaledBaseUnit::FARAD, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_GRAM:
    return ScaledBaseUnit(ScaledBaseUnit::GRAM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_GRAY:
    return ScaledBaseUnit(ScaledBaseUnit::GRAY, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_HENRY:
    return ScaledBaseUnit(ScaledBaseUnit::HENRY, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_HERTZ:
    return ScaledBaseUnit(ScaledBaseUnit::HERTZ, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_ITEM:
    return ScaledBaseUnit(ScaledBaseUnit::ITEM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_JOULE:
    return ScaledBaseUnit(ScaledBaseUnit::JOULE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_KATAL:
    return ScaledBaseUnit(ScaledBaseUnit::KATAL, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_KELVIN:
    return ScaledBaseUnit(ScaledBaseUnit::KELVIN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_KILOGRAM:
    return ScaledBaseUnit(ScaledBaseUnit::KILOGRAM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_LITER:
  case libsbml::UNIT_KIND_LITRE:
    return ScaledBaseUnit(ScaledBaseUnit::LITRE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_LUMEN:
    return ScaledBaseUnit(ScaledBaseUnit::LUMEN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_LUX:
    return ScaledBaseUnit(ScaledBaseUnit::LUX, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_METER:
  case libsbml::UNIT_KIND_METRE:
    return ScaledBaseUnit(ScaledBaseUnit::METRE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_MOLE:
    return ScaledBaseUnit(ScaledBaseUnit::MOLE, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_NEWTON:
    return ScaledBaseUnit(ScaledBaseUnit::NEWTON, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_OHM:
    return ScaledBaseUnit(ScaledBaseUnit::OHM, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_PASCAL:
    return ScaledBaseUnit(ScaledBaseUnit::PASCAL, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_RADIAN:
    return ScaledBaseUnit(ScaledBaseUnit::RADIAN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_SECOND:
    return ScaledBaseUnit(ScaledBaseUnit::SECOND, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_SIEMENS:
    return ScaledBaseUnit(ScaledBaseUnit::SIEMENS, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_SIEVERT:
    return ScaledBaseUnit(ScaledBaseUnit::SIEVERT, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_STERADIAN:
    return ScaledBaseUnit(ScaledBaseUnit::STERADIAN, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_TESLA:
    return ScaledBaseUnit(ScaledBaseUnit::TESLA, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_VOLT:
    return ScaledBaseUnit(ScaledBaseUnit::VOLT, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_WATT:
    return ScaledBaseUnit(ScaledBaseUnit::WATT, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_WEBER:
    return ScaledBaseUnit(ScaledBaseUnit::WEBER, unit->getMultiplier(), unit->getScale(), unit->getExponentAsDouble());
  case libsbml::UNIT_KIND_INVALID:
    break;
  }

  InternalError err;
  err << "Unknown UNIT KIND: " << (unsigned int) unit->getKind();
  throw err;
}



Reaction *
Assembler::processReaction(libsbml::Reaction *node)
{
  if (! node->isSetKineticLaw())
  {
    SemanticError err;
    err << "In Reaction " << node->getId() << ": A reaction needs a KineticLaw!";
    throw err;
  }

  // Assemble kinetic law and reaction:
  Ast::KineticLaw *law = this->processKineticLaw(node->getKineticLaw());
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
    if (r->isSetStoichiometryMath())
    {
      expr = this->processExpression(r->getStoichiometryMath()->getMath());
    }
    else
    {
      expr = GiNaC::numeric(r->getStoichiometry());
    }

    reaction->addReactantStoichiometry(this->resolveSpecies(r->getSpecies()), expr);
  }


  // Add products:
  for (size_t i=0; i<node->getNumProducts(); i++)
  {
    libsbml::SpeciesReference *r = node->getProduct(i);
    GiNaC::ex expr;

    // If the product has a complex stochiometry:
    if (r->isSetStoichiometryMath())
    {
      expr = this->processExpression(r->getStoichiometryMath()->getMath());
    }
    else
    {
      expr = GiNaC::numeric(r->getStoichiometry());
    }

    reaction->addProductStoichiometry(this->resolveSpecies(r->getSpecies()), expr);
  }

  for (size_t i=0; i<node->getNumModifiers(); i++) {
    reaction->addModifier(this->resolveSpecies(node->getModifier(i)->getSpecies()));
  }

  return reaction;
}


KineticLaw *
Assembler::processKineticLaw(libsbml::KineticLaw *node)
{
  // Construct kinetic law from empty expression:
  Ast::KineticLaw *law = new Ast::KineticLaw(GiNaC::ex());

  // A KineticLaw may have its own parameters (local scope)
  this->pushScope(law);

  // Now populate Scope of KineticLaw with its parameters:
  for (size_t i=0; i<node->getNumParameters(); i++)
  {
    // Add new (local) parameter to KineticLaw
    law->addDefinition(this->processParameterDefinition(node->getParameter(i)));
  }

  // Construct kinetic law expression
  GiNaC::ex expr = this->processExpression(node->getMath());

  // Done
  this->popScope();

  // Set correct rate law
  law->setRateLaw(expr);

  return law;
}


GiNaC::ex
Assembler::processExpression(const libsbml::ASTNode *node)
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
    return this->processConstBoolean(node);

  case libsbml::AST_INTEGER:
    // Handle integer constants
    return this->processConstInteger(node);

  case libsbml::AST_REAL:
  case libsbml::AST_REAL_E:
  case libsbml::AST_RATIONAL:
    // Handle floating-point constants
    return this->processConstFloat(node);

  case libsbml::AST_PLUS:
  case libsbml::AST_MINUS:
  case libsbml::AST_TIMES:
  case libsbml::AST_DIVIDE:
  case libsbml::AST_POWER:
    // Handle arithmetic operators:
    return this->processArithmeticOperator(node);

  case libsbml::AST_NAME:
    // Handle variable symbols:
    return this->processSymbol(node);

  case libsbml::AST_NAME_TIME:
    // Handle "time" symbol
    return this->model.getTime();

  case libsbml::AST_FUNCTION:
    // Handle user-defined functions
    return this->processFunctionCall(node);

  case libsbml::AST_FUNCTION_EXP:
    // Handle exponential, make sure, GiNaC handles the constant properly:
    return GiNaC::exp(this->processExpression(node->getChild(0)));

  case libsbml::AST_FUNCTION_POWER:
    // Handle general power as function:
    return GiNaC::power(this->processExpression(node->getChild(0)),
                        this->processExpression(node->getChild(1)));

  case libsbml::AST_FUNCTION_ROOT:
    // Handle general root:
    return GiNaC::power(this->processExpression(node->getChild(1)),
                        1./this->processExpression(node->getChild(0)));

  case libsbml::AST_FUNCTION_LN:
    return GiNaC::log(this->processExpression(node->getChild(0)));

  case libsbml::AST_FUNCTION_ABS:
    return GiNaC::abs(this->processExpression(node->getChild(0)));

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
Assembler::processConstBoolean(const libsbml::ASTNode *node)
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
Assembler::processConstInteger(const libsbml::ASTNode *node)
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
Assembler::processConstFloat(const libsbml::ASTNode *node)
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
Assembler::processArithmeticOperator(const libsbml::ASTNode *node)
{
  switch(node->getType())
  {
  case libsbml::AST_PLUS:
    return this->processExpression(node->getLeftChild()) +
        this->processExpression(node->getRightChild());

  case libsbml::AST_MINUS:
    return this->processExpression(node->getLeftChild()) -
        this->processExpression(node->getRightChild());

  case libsbml::AST_TIMES:
    return this->processExpression(node->getLeftChild()) *
        this->processExpression(node->getRightChild());

  case libsbml::AST_DIVIDE:
    return this->processExpression(node->getLeftChild()) /
        this->processExpression(node->getRightChild());

  case libsbml::AST_POWER:
    return GiNaC::power(this->processExpression(node->getLeftChild()),
                        this->processExpression(node->getRightChild()));

  default:
    break;
  }

  InternalError err;
  err << "Unknown SBML arithmetic operator: " << (unsigned int) node->getType();
  throw err;
}


GiNaC::symbol
Assembler::processSymbol(const libsbml::ASTNode *node)
{
  // Check if node is a symbol (name):
  if (libsbml::AST_NAME != node->getType())
  {
    InternalError err;
    err << "Unknown SBML symbol type: " << (unsigned int) node->getType();
    throw err;
  }

  return this->resolveVariable(node->getName())->getSymbol();
}


GiNaC::ex
Assembler::processFunctionCall(const libsbml::ASTNode *node)
{
  // Check if node is a user-defined function-call:
  if (libsbml::AST_FUNCTION != node->getType())
  {
    InternalError err;
    err << "Unknwon SBML function call type: " << (unsigned int) node->getType();
    throw err;
  }

  // Get Function by name:
  Ast::FunctionDefinition *func = this->model.getFunction(node->getName());

  // Get function argument expressions:
  std::vector<GiNaC::ex> arguments(node->getNumChildren());
  for (size_t i=0; i<node->getNumChildren(); i++)
  {
    // construct Expression from each argument
    arguments[i] = this->processExpression(node->getChild(i));
  }

  // Finally return inlined function call:
  return func->inlineFunction(arguments);
}
