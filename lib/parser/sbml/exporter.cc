#include "exporter.hh"

using namespace Fluc;
using namespace Fluc::Parser::Sbml;



libsbml::Model *
Writer::processModel(Ast::Model &model, libsbml::SBMLDocument *sbml_doc)
{
  libsbml::Model *sbml_model = sbml_doc->createModel();
  /// @bug Ast::Model has no identifier!
  if (model.hasName()) { sbml_model->setName(model.getName()); }

  processUnitDefinitions(model, sbml_model);
  return sbml_model;
}


void
Writer::processUnitDefinitions(Ast::Model &model, libsbml::Model *sbml_model)
{
  for (Ast::Model::iterator item=model.begin(); item!=model.end(); item++) {
    // Skip non unit definitions:
    if (! Ast::Node::isUnitDefinition(*item)) { continue; }

    Ast::UnitDefinition *unit_def = static_cast<Ast::UnitDefinition *>(*item);
    libsbml::UnitDefinition *sbml_unit = sbml_model->createUnitDefinition();
    sbml_unit->setId(unit_def->getIdentifier());
    if (unit_def->hasName()) { sbml_unit->setName(unit_def->getName()); }
    processUnitDefinition(unit_def->getUnit(), sbml_unit);
  }
}


void
Writer::processUnitDefinition(const Ast::Unit &unit, libsbml::UnitDefinition *sbml_unit_def)
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
Writer::processUnit(const Ast::ScaledBaseUnit &unit, libsbml::UnitDefinition *sbml_unit_def)
{
  libsbml::Unit *sbml_unit = sbml_unit_def->createUnit();

  switch(unit.getBaseUnit()) {
  case Ast::ScaledBaseUnit::AMPERE:
    sbml_unit->setKind(libsbml::UNIT_KIND_AMPERE);
    break;
  case Ast::ScaledBaseUnit::AVOGADRO:
    sbml_unit->setKind(libsbml::UNIT_KIND_AVOGADRO);
    break;
  case Ast::ScaledBaseUnit::BECQUEREL:
    sbml_unit->setKind(libsbml::UNIT_KIND_BECQUEREL);
    break;
  case Ast::ScaledBaseUnit::CANDELA:
    sbml_unit->setKind(libsbml::UNIT_KIND_CANDELA);
    break;
  case Ast::ScaledBaseUnit::CELSIUS:
    sbml_unit->setKind(libsbml::UNIT_KIND_CELSIUS);
    break;
  case Ast::ScaledBaseUnit::COULOMB:
    sbml_unit->setKind(libsbml::UNIT_KIND_COULOMB);
    break;
  case Ast::ScaledBaseUnit::DIMENSIONLESS:
    sbml_unit->setKind(libsbml::UNIT_KIND_DIMENSIONLESS);
    break;
  case Ast::ScaledBaseUnit::FARAD:
    sbml_unit->setKind(libsbml::UNIT_KIND_FARAD);
    break;
  case Ast::ScaledBaseUnit::GRAM:
    sbml_unit->setKind(libsbml::UNIT_KIND_GRAM);
    break;
  case Ast::ScaledBaseUnit::GRAY:
    sbml_unit->setKind(libsbml::UNIT_KIND_GRAY);
    break;
  case Ast::ScaledBaseUnit::HENRY:
    sbml_unit->setKind(libsbml::UNIT_KIND_HENRY);
    break;
  case Ast::ScaledBaseUnit::HERTZ:
    sbml_unit->setKind(libsbml::UNIT_KIND_HERTZ);
    break;
  case Ast::ScaledBaseUnit::ITEM:
    sbml_unit->setKind(libsbml::UNIT_KIND_ITEM);
    break;
  case Ast::ScaledBaseUnit::JOULE:
    sbml_unit->setKind(libsbml::UNIT_KIND_JOULE);
    break;
  case Ast::ScaledBaseUnit::KATAL:
    sbml_unit->setKind(libsbml::UNIT_KIND_KATAL);
    break;
  case Ast::ScaledBaseUnit::KELVIN:
    sbml_unit->setKind(libsbml::UNIT_KIND_KELVIN);
    break;
  case Ast::ScaledBaseUnit::KILOGRAM:
    sbml_unit->setKind(libsbml::UNIT_KIND_KILOGRAM);
    break;
  case Ast::ScaledBaseUnit::LITRE:
    sbml_unit->setKind(libsbml::UNIT_KIND_LITRE);
    break;
  case Ast::ScaledBaseUnit::LUMEN:
    sbml_unit->setKind(libsbml::UNIT_KIND_LUMEN);
    break;
  case Ast::ScaledBaseUnit::LUX:
    sbml_unit->setKind(libsbml::UNIT_KIND_LUX);
    break;
  case Ast::ScaledBaseUnit::METRE:
    sbml_unit->setKind(libsbml::UNIT_KIND_METRE);
    break;
  case Ast::ScaledBaseUnit::MOLE:
    sbml_unit->setKind(libsbml::UNIT_KIND_MOLE);
    break;
  case Ast::ScaledBaseUnit::NEWTON:
    sbml_unit->setKind(libsbml::UNIT_KIND_NEWTON);
    break;
  case Ast::ScaledBaseUnit::OHM:
    sbml_unit->setKind(libsbml::UNIT_KIND_OHM);
    break;
  case Ast::ScaledBaseUnit::PASCAL:
    sbml_unit->setKind(libsbml::UNIT_KIND_PASCAL);
    break;
  case Ast::ScaledBaseUnit::RADIAN:
    sbml_unit->setKind(libsbml::UNIT_KIND_RADIAN);
    break;
  case Ast::ScaledBaseUnit::SECOND:
    sbml_unit->setKind(libsbml::UNIT_KIND_SECOND);
    break;
  case Ast::ScaledBaseUnit::SIEMENS:
    sbml_unit->setKind(libsbml::UNIT_KIND_SIEMENS);
    break;
  case Ast::ScaledBaseUnit::SIEVERT:
    sbml_unit->setKind(libsbml::UNIT_KIND_SIEVERT);
    break;
  case Ast::ScaledBaseUnit::STERADIAN:
    sbml_unit->setKind(libsbml::UNIT_KIND_STERADIAN);
    break;
  case Ast::ScaledBaseUnit::TESLA:
    sbml_unit->setKind(libsbml::UNIT_KIND_TESLA);
    break;
  case Ast::ScaledBaseUnit::VOLT:
    sbml_unit->setKind(libsbml::UNIT_KIND_VOLT);
    break;
  case Ast::ScaledBaseUnit::WATT:
    sbml_unit->setKind(libsbml::UNIT_KIND_WATT);
    break;
  case Ast::ScaledBaseUnit::WEBER:
    sbml_unit->setKind(libsbml::UNIT_KIND_WEBER);
    break;
  }

  sbml_unit->setMultiplier(unit.getMultiplier());
  sbml_unit->setScale(unit.getScale());
  sbml_unit->setExponent(unit.getExponent());
}


void
Writer::processFunctionDefinitions(Ast::Model &model, libsbml::Model *sbml_model)
{
  for(Ast::Model::iterator item = model.begin(); item != model.end(); item++) {
    if (! Ast::Node::isFunctionDefinition(*item)) { continue; }
    Ast::FunctionDefinition *func = static_cast<Ast::FunctionDefinition *>(*item);
    libsbml::FunctionDefinition *sbml_func = sbml_model->createFunctionDefinition();
    processFunctionDefinition(func, sbml_func);
  }
}

void
Writer::processFunctionDefinition(Ast::FunctionDefinition *func, libsbml::FunctionDefinition *sbml_func)
{
  libsbml::ASTNode *ast_func = new libsbml::ASTNode(libsbml::AST_LAMBDA);
  sbml_func->setId(func->getIdentifier());

  // Assemble function arguments:
  for(size_t i=0; i<func->getNumArgs(); i++) {
    libsbml::ASTNode *arg = new libsbml::ASTNode(libsbml::AST_NAME);
    arg->setName(func->getArgByIdx(i).get_name().c_str());
    ast_func->addChild(arg);
  }

  // Assemble function body:
  ast_func->addChild(processExpression(func->getBody()));
  sbml_func->setMath(ast_func);
}


void
Writer::processParameters(Ast::Model &model, libsbml::Model *sbml_model)
{
  for (size_t i=0; i<model.numParameters(); i++) {
    Ast::Parameter *param = model.getParameter(i);
    libsbml::Parameter *sbml_param = sbml_model->createParameter();
    processParameter(param, sbml_param);
    if (param->hasValue()) { processInitialValue(param, sbml_model); }
    if (param->hasRule()) { processRule(param, sbml_model); }
  }
}


void
Writer::processParameter(Ast::Parameter *param, libsbml::Parameter *sbml_param)
{
  sbml_param->setId(param->getIdentifier());
  if (param->hasName()) { sbml_param->setName(param->getName()); }
  sbml_param->setConstant(param->isConst());
}


void
Writer::processInitialValue(Ast::VariableDefinition *var, libsbml::Model *sbml_model)
{
  libsbml::InitialAssignment *init = sbml_model->createInitialAssignment();
  init->setSymbol(var->getIdentifier());
  init->setMath(processExpression(var->getValue()));
}


void
Writer::processRule(Ast::VariableDefinition *var, libsbml::Model *model)
{
  if (Ast::Node::isAssignmentRule(var->getRule())) {
    Ast::AssignmentRule *rule = static_cast<Ast::AssignmentRule *>(var->getRule());
  } else if (Ast::Node::isRateRule(var->getRule())) {
    Ast::RateRule * rule = static_cast<Ast::RateRule *>(var->getRule());
  } else {
    ExportError err;
    err << "Can not export model " << model->getName()
        << ": Unknown rule type for variable " << var->getIdentifier();
    throw err;
  }
}

libsbml::ASTNode *
Writer::processExpression(GiNaC::ex)
{
  return 0;
}
