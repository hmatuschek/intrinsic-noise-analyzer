#include "writer.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Parser::Sbml;



LIBSBML_CPP_NAMESPACE_QUALIFIER Model *
Writer::processModel(Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER SBMLDocument *sbml_doc)
{
  LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model = sbml_doc->createModel();
  // First, serialize unit definitions:
  processUnitDefinitions(model, sbml_model);
  // then, serialize parameter definitions:
  processParameters(model, sbml_model);
  // them serialize compartments
  processCompartments(model, sbml_model);
  // then serialise species:
  processSpeciesList(model, sbml_model);
  // then serialise reactions
  processReactions(model, sbml_model);

  // Set model id
  sbml_model->setId(model.getIdentifier());

  // Set model name if set
  if (model.hasName()) { sbml_model->setName(model.getName()); }

  // Done...
  return sbml_model;
}


void
Writer::processUnitDefinitions(Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model)
{
  // Redefine default units:
  LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *sbml_unit;
  sbml_unit = sbml_model->createUnitDefinition(); sbml_unit->setId("substance");
  processUnit(model.getSubstanceUnit().asScaledBaseUnit(), sbml_unit);

  sbml_unit = sbml_model->createUnitDefinition(); sbml_unit->setId("volume");
  processUnit(model.getVolumeUnit().asScaledBaseUnit(), sbml_unit);

  sbml_unit = sbml_model->createUnitDefinition(); sbml_unit->setId("area");
  processUnit(model.getAreaUnit().asScaledBaseUnit(), sbml_unit);

  sbml_unit = sbml_model->createUnitDefinition(); sbml_unit->setId("length");
  processUnit(model.getLengthUnit().asScaledBaseUnit(), sbml_unit);

  sbml_unit = sbml_model->createUnitDefinition(); sbml_unit->setId("time");
  processUnit(model.getTimeUnit().asScaledBaseUnit(), sbml_unit);


  for (Ast::Model::iterator item=model.begin(); item!=model.end(); item++) {
    // Skip non unit definitions:
    if (! Ast::Node::isUnitDefinition(*item)) { continue; }

    Ast::UnitDefinition *unit_def = static_cast<Ast::UnitDefinition *>(*item);
    LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *sbml_unit = sbml_model->createUnitDefinition();
    sbml_unit->setId(unit_def->getIdentifier());
    if (unit_def->hasName()) { sbml_unit->setName(unit_def->getName()); }
    processUnitDefinition(unit_def->getUnit(), sbml_unit);
  }
}


void
Writer::processUnitDefinition(const Ast::Unit &unit, LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *sbml_unit_def)
{
  if ( (1 != unit.getMultiplier()) || (0 != unit.getScale()) ) {
    processUnit(
          Ast::ScaledBaseUnit(
            Ast::ScaledBaseUnit::DIMENSIONLESS, unit.getMultiplier(), unit.getScale(), 1),
          sbml_unit_def);
  }

  for (Ast::Unit::iterator item=unit.begin(); item!=unit.end(); item++) {
    processUnit(Ast::ScaledBaseUnit(item->first, 1, 0, item->second), sbml_unit_def);
  }
}


void
Writer::processUnit(const Ast::ScaledBaseUnit &unit, LIBSBML_CPP_NAMESPACE_QUALIFIER UnitDefinition *sbml_unit_def)
{
  LIBSBML_CPP_NAMESPACE_QUALIFIER Unit *sbml_unit = sbml_unit_def->createUnit();

  switch(unit.getBaseUnit()) {
  case Ast::ScaledBaseUnit::AMPERE:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_AMPERE);
    break;
  case Ast::ScaledBaseUnit::AVOGADRO:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_AVOGADRO);
    break;
  case Ast::ScaledBaseUnit::BECQUEREL:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_BECQUEREL);
    break;
  case Ast::ScaledBaseUnit::CANDELA:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_CANDELA);
    break;
  case Ast::ScaledBaseUnit::CELSIUS:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_CELSIUS);
    break;
  case Ast::ScaledBaseUnit::COULOMB:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_COULOMB);
    break;
  case Ast::ScaledBaseUnit::DIMENSIONLESS:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_DIMENSIONLESS);
    break;
  case Ast::ScaledBaseUnit::FARAD:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_FARAD);
    break;
  case Ast::ScaledBaseUnit::GRAM:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_GRAM);
    break;
  case Ast::ScaledBaseUnit::GRAY:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_GRAY);
    break;
  case Ast::ScaledBaseUnit::HENRY:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_HENRY);
    break;
  case Ast::ScaledBaseUnit::HERTZ:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_HERTZ);
    break;
  case Ast::ScaledBaseUnit::ITEM:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_ITEM);
    break;
  case Ast::ScaledBaseUnit::JOULE:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_JOULE);
    break;
  case Ast::ScaledBaseUnit::KATAL:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_KATAL);
    break;
  case Ast::ScaledBaseUnit::KELVIN:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_KELVIN);
    break;
  case Ast::ScaledBaseUnit::KILOGRAM:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_KILOGRAM);
    break;
  case Ast::ScaledBaseUnit::LITRE:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_LITRE);
    break;
  case Ast::ScaledBaseUnit::LUMEN:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_LUMEN);
    break;
  case Ast::ScaledBaseUnit::LUX:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_LUX);
    break;
  case Ast::ScaledBaseUnit::METRE:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_METRE);
    break;
  case Ast::ScaledBaseUnit::MOLE:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_MOLE);
    break;
  case Ast::ScaledBaseUnit::NEWTON:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_NEWTON);
    break;
  case Ast::ScaledBaseUnit::OHM:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_OHM);
    break;
  case Ast::ScaledBaseUnit::PASCAL:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_PASCAL);
    break;
  case Ast::ScaledBaseUnit::RADIAN:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_RADIAN);
    break;
  case Ast::ScaledBaseUnit::SECOND:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_SECOND);
    break;
  case Ast::ScaledBaseUnit::SIEMENS:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_SIEMENS);
    break;
  case Ast::ScaledBaseUnit::SIEVERT:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_SIEVERT);
    break;
  case Ast::ScaledBaseUnit::STERADIAN:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_STERADIAN);
    break;
  case Ast::ScaledBaseUnit::TESLA:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_TESLA);
    break;
  case Ast::ScaledBaseUnit::VOLT:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_VOLT);
    break;
  case Ast::ScaledBaseUnit::WATT:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_WATT);
    break;
  case Ast::ScaledBaseUnit::WEBER:
    sbml_unit->setKind(LIBSBML_CPP_NAMESPACE_QUALIFIER UNIT_KIND_WEBER);
    break;
  }

  sbml_unit->setMultiplier(unit.getMultiplier());
  sbml_unit->setScale(unit.getScale());
  sbml_unit->setExponent(unit.getExponent());
}


void
Writer::processFunctionDefinitions(Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model)
{
  for(Ast::Model::iterator item = model.begin(); item != model.end(); item++) {
    if (! Ast::Node::isFunctionDefinition(*item)) { continue; }
    Ast::FunctionDefinition *func = static_cast<Ast::FunctionDefinition *>(*item);
    LIBSBML_CPP_NAMESPACE_QUALIFIER FunctionDefinition *sbml_func = sbml_model->createFunctionDefinition();
    processFunctionDefinition(func, sbml_func, model);
  }
}

void
Writer::processFunctionDefinition(
  Ast::FunctionDefinition *func, LIBSBML_CPP_NAMESPACE_QUALIFIER FunctionDefinition *sbml_func, Ast::Model &model)
{
  LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *ast_func = new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_LAMBDA);
  sbml_func->setId(func->getIdentifier());

  // Assemble function arguments:
  for(size_t i=0; i<func->getNumArgs(); i++) {
    LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *arg = new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_NAME);
    arg->setName(func->getArgByIdx(i).get_name().c_str());
    ast_func->addChild(arg);
  }

  // Assemble function body:
  ast_func->addChild(processExpression(func->getBody(), model));
  sbml_func->setMath(ast_func);
}


void
Writer::processParameters(Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model)
{
  for (size_t i=0; i<model.numParameters(); i++) {
    Ast::Parameter *param = model.getParameter(i);
    LIBSBML_CPP_NAMESPACE_QUALIFIER Parameter *sbml_param = sbml_model->createParameter();
    processParameter(param, sbml_param);
    if (param->hasValue()) { processInitialValue(param, sbml_model, model); }
    if (param->hasRule()) { processRule(param, sbml_model, model); }
  }
}


void
Writer::processParameter(Ast::Parameter *param, LIBSBML_CPP_NAMESPACE_QUALIFIER Parameter *sbml_param)
{
  sbml_param->setId(param->getIdentifier());
  if (param->hasName()) { sbml_param->setName(param->getName()); }
  sbml_param->setConstant(param->isConst());
}


void
Writer::processCompartments(Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model)
{
  for (size_t i=0; i<model.numCompartments(); i++) {
    Ast::Compartment *comp = model.getCompartment(i);
    LIBSBML_CPP_NAMESPACE_QUALIFIER Compartment *sbml_comp = sbml_model->createCompartment();
    processCompartment(comp, sbml_comp);
    if (comp->hasValue()) { processInitialValue(comp, sbml_model, model); }
    if (comp->hasRule())  { processRule(comp, sbml_model, model); }
  }
}


void
Writer::processCompartment(Ast::Compartment *comp, LIBSBML_CPP_NAMESPACE_QUALIFIER Compartment *sbml_comp)
{
  // Set ID,
  sbml_comp->setId(comp->getIdentifier());
  // name if set,
  if (comp->hasName()) { sbml_comp->setName(comp->getName()); }
  // if comp is constant,
  sbml_comp->setConstant(comp->isConst());
  // and the spacial dimension
  switch (comp->getDimension()) {
  case Ast::Compartment::POINT:  sbml_comp->setSpatialDimensions(unsigned(0));
  case Ast::Compartment::LINE:   sbml_comp->setSpatialDimensions(unsigned(1));
  case Ast::Compartment::AREA:   sbml_comp->setSpatialDimensions(unsigned(2));
  case Ast::Compartment::VOLUME: sbml_comp->setSpatialDimensions(unsigned(3));
  }
}


void
Writer::processSpeciesList(Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model)
{
  for (size_t i=0; i<model.numSpecies(); i++) {
    Ast::Species *species = model.getSpecies(i);
    LIBSBML_CPP_NAMESPACE_QUALIFIER Species *sbml_species = sbml_model->createSpecies();
    sbml_species->setHasOnlySubstanceUnits(model.speciesHasSubstanceUnits());
    processSpecies(species, sbml_species);

    sbml_species->setCompartment(species->getCompartment()->getIdentifier());
    if (species->hasValue()) { processInitialValue(species, sbml_model, model); }
    if (species->hasRule())  { processRule(species, sbml_model, model); }
  }
}


void
Writer::processSpecies(Ast::Species *species, LIBSBML_CPP_NAMESPACE_QUALIFIER Species *sbml_species)
{
  sbml_species->setId(species->getIdentifier());
  if (species->hasName()) {sbml_species->setName(species->getName()); }
  sbml_species->setConstant(species->isConst());
}


void
Writer::processReactions(Ast::Model &model, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model)
{
  for (size_t i=0; i<model.numReactions(); i++) {
    Ast::Reaction *reac = model.getReaction(i);
    LIBSBML_CPP_NAMESPACE_QUALIFIER Reaction *sbml_reac = sbml_model->createReaction();
    processReaction(reac, sbml_reac, model);
  }
}

void
Writer::processReaction(Ast::Reaction *reac, LIBSBML_CPP_NAMESPACE_QUALIFIER Reaction *sbml_reac, Ast::Model &model)
{
  // Handle name and id of reactions:
  sbml_reac->setId(reac->getIdentifier());
  if (reac->hasName()) { sbml_reac->setName(reac->getName()); }

  // Process reactants:
  for (Ast::Reaction::iterator item = reac->reacBegin(); item != reac->reacEnd(); item++) {
    LIBSBML_CPP_NAMESPACE_QUALIFIER SpeciesReference *sbml_r = sbml_reac->createReactant();
    sbml_r->setSpecies(item->first->getIdentifier());
    LIBSBML_CPP_NAMESPACE_QUALIFIER StoichiometryMath *sbml_r_m = sbml_r->createStoichiometryMath();
    sbml_r_m->setMath(processExpression(item->second, model));
  }
  // Process products:
  for (Ast::Reaction::iterator item = reac->prodBegin(); item != reac->prodEnd(); item++) {
    LIBSBML_CPP_NAMESPACE_QUALIFIER SpeciesReference *sbml_p = sbml_reac->createProduct();
    sbml_p->setSpecies(item->first->getIdentifier());
    LIBSBML_CPP_NAMESPACE_QUALIFIER StoichiometryMath *sbml_p_m = sbml_p->createStoichiometryMath();
    sbml_p_m->setMath(processExpression(item->second, model));
  }
  // Process modifiers:
  for (Ast::Reaction::mod_iterator item = reac->modBegin(); item != reac->modEnd(); item++) {
    LIBSBML_CPP_NAMESPACE_QUALIFIER ModifierSpeciesReference *sbml_r = sbml_reac->createModifier();
    sbml_r->setSpecies((*item)->getIdentifier());
  }
  // process kinetic law:
  LIBSBML_CPP_NAMESPACE_QUALIFIER KineticLaw *sbml_law = sbml_reac->createKineticLaw();
  processKineticLaw(reac->getKineticLaw(), sbml_law, model);
}

void
Writer::processKineticLaw(Ast::KineticLaw *law, LIBSBML_CPP_NAMESPACE_QUALIFIER KineticLaw *sbml_law, Ast::Model &model)
{
  // Handle local paramerers:
  for (Ast::KineticLaw::iterator item=law->begin(); item != law->end(); item++) {
    if (! Ast::Node::isParameter(*item)) {
      ExportError err;
      err << "Can not export KineticLaw to SBML, only parameter definitions are allowed "
          << "in a KineticLaw definition.";
      throw err;
    }

    Ast::Parameter *param = static_cast<Ast::Parameter *>(*item);
    LIBSBML_CPP_NAMESPACE_QUALIFIER Parameter *sbml_param = sbml_law->createParameter();
    sbml_param->setId(param->getIdentifier());
    if (param->hasName()) {sbml_param->setName(param->getName()); }
    if (! hasDefaultUnit(param, model)) {
      sbml_param->setUnits(getUnitIdentifier(param, model));
    }
    if (param->hasValue()) {
      if (! GiNaC::is_a<GiNaC::numeric>(param->getValue()) ) {
        ExportError err;
        err << "Can only set numeric value for intial value of local parameter "
            << param->getIdentifier();
        throw err;
      }
      sbml_param->setValue(GiNaC::ex_to<GiNaC::numeric>(param->getValue()).to_double());
    }
    if (param->hasRule()) {
      ExportError err;
      err << "Can not export local parameter definition " << param->getIdentifier() << " to SBML:"
          << " Can not define rule for paramter.";
      throw err;
    }
  }

  // process law:
  sbml_law->setMath(processExpression(law->getRateLaw(), model));
}


void
Writer::processInitialValue(
  Ast::VariableDefinition *var, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model, Ast::Model &model)
{
  LIBSBML_CPP_NAMESPACE_QUALIFIER InitialAssignment *init = sbml_model->createInitialAssignment();
  init->setSymbol(var->getIdentifier());
  init->setMath(processExpression(var->getValue(), model));
}


void
Writer::processRule(Ast::VariableDefinition *var, LIBSBML_CPP_NAMESPACE_QUALIFIER Model *sbml_model, Ast::Model &model)
{
  if (Ast::Node::isAssignmentRule(var->getRule())) {
    Ast::AssignmentRule *rule = static_cast<Ast::AssignmentRule *>(var->getRule());
    LIBSBML_CPP_NAMESPACE_QUALIFIER AssignmentRule *sbml_rule = sbml_model->createAssignmentRule();
    sbml_rule->setVariable(var->getIdentifier());
    sbml_rule->setMath(processExpression(rule->getRule(), model));
  } else if (Ast::Node::isRateRule(var->getRule())) {
    Ast::RateRule * rule = static_cast<Ast::RateRule *>(var->getRule());
    LIBSBML_CPP_NAMESPACE_QUALIFIER RateRule *sbml_rule = sbml_model->createRateRule();
    sbml_rule->setVariable(var->getIdentifier());
    sbml_rule->setMath(processExpression(rule->getRule(), model));
  } else {
    ExportError err;
    err << "Can not export model " << sbml_model->getName()
        << ": Unknown rule type for variable " << var->getIdentifier();
    throw err;
  }
}


bool
Writer::hasDefaultUnit(Ast::Parameter *var, Ast::Model &model)
{
  return var->getUnit().isExactlyDimensionless();
}


std::string
Writer::getUnitIdentifier(Ast::Parameter *var, Ast::Model &model)
{
  if (! var->getUnit().isExactlyDimensionless()) {
    return "dimensionless";
  }

  return model.getUnitDefinition(var->getUnit())->getIdentifier();
}


LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *
Writer::processExpression(GiNaC::ex expression, Ast::Model &model)
{
  return SBMLExpressionAssembler::process(expression, model);
}



/* ******************************************************************************************** *
 * Implementation of expression assembler:
 * ******************************************************************************************** */
SBMLExpressionAssembler::SBMLExpressionAssembler(Ast::Model &model) : _stack(), _model(model) { }

void
SBMLExpressionAssembler::visit(const GiNaC::numeric &value)
{
  _stack.push_back(new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_REAL));
  _stack.back()->setValue(value.to_double());
}

void
SBMLExpressionAssembler::visit(const GiNaC::symbol &symbol)
{
  if (_model.getTime() == symbol) {
    _stack.push_back(new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_NAME_TIME));
  } else {
    _stack.push_back(new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_NAME));
    _stack.back()->setName(symbol.get_name().c_str());
  }
}

void
SBMLExpressionAssembler::visit(const GiNaC::add &sum)
{
  for (size_t i=0; i<sum.nops(); i++) {
    sum.op(i).accept(*this);
  }

  // Then assemble the sum on the stack:
  for (size_t i=1; i<sum.nops(); i++) {
    LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *rhs = _stack.back(); _stack.pop_back();
    LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *lhs = _stack.back(); _stack.pop_back();
    _stack.push_back(new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_PLUS));
    _stack.back()->addChild(lhs);
    _stack.back()->addChild(rhs);
  }
}

void
SBMLExpressionAssembler::visit(const GiNaC::mul &prod)
{
  for (size_t i=0; i<prod.nops(); i++) {
    prod.op(i).accept(*this);
  }

  // Then assemble the sum on the stack:
  for (size_t i=1; i<prod.nops(); i++) {
    LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *rhs = _stack.back(); _stack.pop_back();
    LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *lhs = _stack.back(); _stack.pop_back();
    _stack.push_back(new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_TIMES));
    _stack.back()->addChild(lhs);
    _stack.back()->addChild(rhs);
  }
}

void
SBMLExpressionAssembler::visit(const GiNaC::power &pow)
{
  // handle basis
  pow.op(0).accept(*this);
  // handle exponent
  pow.op(1).accept(*this);

  LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *exponent = _stack.back(); _stack.pop_back();
  LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *basis = _stack.back(); _stack.pop_back();
  _stack.push_back(new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_POWER));
  _stack.back()->addChild(basis); _stack.back()->addChild(exponent);
}

void
SBMLExpressionAssembler::visit(const GiNaC::function &function)
{
  if (function.get_serial() == GiNaC::abs_SERIAL::serial) {
    LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *arg = _stack.back(); _stack.pop_back();
    _stack.push_back(new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_ABS));
    _stack.back()->addChild(arg);
    return;
  } else if (function.get_serial() == GiNaC::exp_SERIAL::serial) {
    LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *arg = _stack.back(); _stack.pop_back();
    _stack.push_back(new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_EXP));
    _stack.back()->addChild(arg);
    return;
  } else if (function.get_serial() == GiNaC::log_SERIAL::serial) {
    LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *arg = _stack.back(); _stack.pop_back();
    _stack.push_back(new LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode(LIBSBML_CPP_NAMESPACE_QUALIFIER AST_FUNCTION_LN));
    _stack.back()->addChild(arg);
    return;
  }

  ExportError err;
  err << "Can not export function call " << function << " to SBML, unknown function.";
  throw err;
}

void
SBMLExpressionAssembler::visit(const GiNaC::basic &basic)
{
  ExportError err;
  err << "Can not export expression " << basic << " to SBML, unknown expression type.";
  throw err;
}


LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *
SBMLExpressionAssembler::process(GiNaC::ex expression, Ast::Model &model) {
  SBMLExpressionAssembler ass(model); expression.accept(ass);

  LIBSBML_CPP_NAMESPACE_QUALIFIER ASTNode *res = ass._stack.back(); ass._stack.pop_back();
  if (! res->isWellFormedASTNode()) {
    ExportError err;
    err << "Can not export expression: " << expression << " to SBML, something went wrong: "
        << " resulting expression is not well formed.";
    throw err;
  }

  return res;
}
