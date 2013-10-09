#include "writer.hh"
#include "ast/model.hh"
#include "ast/reaction.hh"
#include "exception.hh"
#include "parser/expr/parser.hh"
#include <fstream>


using namespace iNA;
using namespace iNA::Parser::Sbmlsh;


/* ******************************************************************************************** *
 * Implementation of helper functions...
 * ******************************************************************************************** */
/* Dispatcher. */
void
Writer::processModel(Ast::Model &model, std::ostream &output) {
  processModelHeader(model, output);
  processUnitDefinitions(model, output);
  processCompartments(model, output);
  processSpeciesList(model, output);
  processParameterList(model, output);
  processReactionList(model, output);
}


/* Serialize model header and default units. */
void
Writer::processModelHeader(Ast::Model &model, std::ostream &output)
{
  output << "@model:3.1.1 = " << model.getIdentifier();
  if (model.hasName()) output << " \"" << model.getName() << "\"";
  std::list< std::pair<char, std::string> > default_units;

  // Serialize default units:
  if (0 < default_units.size()) { output << std::endl; }
  for (std::list<std::pair<char, std::string> >::iterator item = default_units.begin();
       item != default_units.end(); item++) {
    output << " " << item->first << "=" << item->second;
  }
  output << std::endl;
}


/* Translates a Ast::ScaledBaseUnit::BaseUnit into its string identifier. */
std::string
Writer::getBaseUnitIdentifier(Ast::Unit::BaseUnit unit)
{
  switch (unit) {
  case Ast::Unit::AMPERE: return "ampere";
  case Ast::Unit::AVOGADRO: return "avogadro";
  case Ast::Unit::BECQUEREL: return "becquerel";
  case Ast::Unit::CANDELA: return "candela";
  case Ast::Unit::CELSIUS: return "celsius";
  case Ast::Unit::COULOMB: return "coulomb";
  case Ast::Unit::DIMENSIONLESS: return "dimensionless";
  case Ast::Unit::FARAD: return "farad";
  case Ast::Unit::GRAM: return "gram";
  case Ast::Unit::GRAY: return "gray";
  case Ast::Unit::HENRY: return "henry";
  case Ast::Unit::HERTZ: return "herz";
  case Ast::Unit::ITEM: return "item";
  case Ast::Unit::JOULE: return "joule";
  case Ast::Unit::KATAL: return "katal";
  case Ast::Unit::KELVIN: return "kelvin";
  case Ast::Unit::KILOGRAM: return "kilogram";
  case Ast::Unit::LITRE: return "litre";
  case Ast::Unit::LUMEN: return "lumen";
  case Ast::Unit::LUX: return "lux";
  case Ast::Unit::METRE: return "metre";
  case Ast::Unit::MOLE: return "mole";
  case Ast::Unit::NEWTON: return "newton";
  case Ast::Unit::OHM: return "ohm";
  case Ast::Unit::PASCAL: return "pascal";
  case Ast::Unit::RADIAN: return "radian";
  case Ast::Unit::SECOND: return "second";
  case Ast::Unit::SIEMENS: return "siemens";
  case Ast::Unit::SIEVERT: return "sievert";
  case Ast::Unit::STERADIAN: return "sterdian";
  case Ast::Unit::TESLA: return "tesla";
  case Ast::Unit::VOLT: return "volt";
  case Ast::Unit::WATT: return "watt";
  case Ast::Unit::WEBER: return "weber";
  }

  return "dimensionless";
}


/* handle unit definitions in given model. */
void
Writer::processUnitDefinitions(Ast::Model &model, std::ostream &output)
{
  std::list<std::string> units;
  std::stringstream temp;

  // Process default substance unit first:
  temp.str(""); processUnitDefinition("substance", model.getSubstanceUnit(), temp);
  units.push_back(temp.str());

  // Process default volume unit:
  temp.str(""); processUnitDefinition("volume", model.getVolumeUnit(), temp);
  units.push_back(temp.str());

  // Process default area unit:
  temp.str(""); processUnitDefinition("area", model.getAreaUnit(), temp);
  units.push_back(temp.str());

  // Process default length unit:
  temp.str(""); processUnitDefinition("length", model.getLengthUnit(), temp);
  units.push_back(temp.str());

  // Process default time unit:
  temp.str(""); processUnitDefinition("time", model.getTimeUnit(), temp);
  units.push_back(temp.str());

  // Assemble unit definition section if there are some definitions:
  if (0 < units.size()) {
    output << std::endl << "@units";
    for (std::list<std::string>::iterator row=units.begin(); row!=units.end(); row++) {
      output << *row;
    }
    output << std::endl;
  }
}

void
Writer::processUnitDefinition(const std::string &id, const Ast::Unit &unit, std::ostream &output)
{
  std::list<std::string> units;
  std::stringstream temp;

  if (unit.isScaledBaseUnit()) {
    // If the unit can be expressed in terms of a single scaled base unit -> write compact format
    Ast::Unit::BaseUnit bunit; double multiplier=1; int scale=0, exponent=1;
    unit.asScaledBaseUnit(bunit, multiplier, scale, exponent);
    processScaledUnit(bunit, multiplier, scale, exponent, temp);
    units.push_back(temp.str()); temp.str("");
  } else {
    // If unit is formed in terms of a product of scaled base units:
    if ( (1 != unit.getMultiplier()) || (0 != unit.getScale())) {
      processScaledUnit(
            Ast::Unit::DIMENSIONLESS, unit.getMultiplier(), unit.getScale(), 1, temp);
      units.push_back(temp.str()); temp.str("");
    }
    // process scaled base units of unit:
    for (Ast::Unit::iterator it=unit.begin(); it != unit.end(); it++) {
      processScaledUnit(it->first, 1, 0, it->second, temp);
      units.push_back(temp.str()); temp.str();
    }
  }

  // Serialize
  output << std::endl << " " << id << " = ";
  std::list<std::string>::iterator item = units.begin();
  if (0 < units.size()) {
    for (size_t i=0; i<(units.size()-1); i++, item++) {
      output << *item << "; ";
    }
    output << *item;
  }
}

void
Writer::processScaledUnit(Ast::Unit::BaseUnit unit, double multiplier, int scale, int exponent, std::ostream &output)
{
  std::list<std::string> modifier; std::stringstream temp;
  if (1 != multiplier) { temp << "m=" << multiplier; modifier.push_back(temp.str()); temp.str(""); }
  if (0 != scale) { temp << "s=" << scale; modifier.push_back(temp.str()); temp.str(""); }
  if (1 != exponent) { temp << "e=" << exponent; modifier.push_back(temp.str()); temp.str(""); }

  output << Writer::getBaseUnitIdentifier(unit);
  if (0 < modifier.size()) {
    output << ": ";
    std::list<std::string>::iterator item = modifier.begin();
    for (size_t i=0; i<(modifier.size()-1); i++, item++) {
      output << *item << ", ";
    }
    output << *item;
  }
}


/* Handle compartments. */
void
Writer::processCompartments(Ast::Model &model, std::ostream &output) {
  if (0 == model.numCompartments()) return;

  output << std::endl << "@compartments";
  for (size_t i=0; i<model.numCompartments(); i++) {
    processCompartment(model.getCompartment(i), model, output);
  }
  output << std::endl;
}

/* Handle compartment. */
void
Writer::processCompartment(Ast::Compartment *comp, const Ast::Model &scope, std::ostream &output)
{
  output << std::endl << " " << comp->getIdentifier();

  if (comp->hasValue()) {
    output << " = ";
    Parser::Expr::serializeExpression(comp->getValue(), output, &scope);
  }

  if (comp->hasName())  { output << " \"" << comp->getName() << "\""; }
}

/* handle list of species. */
void
Writer::processSpeciesList(Ast::Model &model, std::ostream &output) {
  if (0 == model.numSpecies()) return;

  output << std::endl << "@species";
  for (size_t i=0; i<model.numSpecies(); i++) {
    processSpecies(model.getSpecies(i), model, output);
  }
  output << std::endl;
}

/* handle single species. */
void
Writer::processSpecies(Ast::Species *species, const Ast::Model &model, std::ostream &output) {
  output << std::endl << " " << species->getCompartment()->getIdentifier() << ": ";
  if (model.speciesHaveSubstanceUnits()) { output << species->getIdentifier() << " "; }
  else {output << "[" << species->getIdentifier() << "] "; }
  output << " = "; Parser::Expr::serializeExpression(species->getValue(), output, &model); output << " ";
  if (model.speciesHaveSubstanceUnits()) { output << "s"; }
  if (species->isConst()) { output << "c"; }
  if (species->hasName()) { output << " \"" << species->getName() << "\""; }
}


/* Handle parameter definition list. */
void
Writer::processParameterList(Ast::Model &model, std::ostream &output)
{
  if (0 == model.numParameters()) return;
  output << std::endl << "@parameters";

  for (size_t i=0; i<model.numParameters(); i++) {
    processParameter(model.getParameter(i), model, output);
  }
  output << std::endl;
}

/* Serialize single paramter definition. */
void
Writer::processParameter(Ast::Parameter *param, const Ast::Model &model, std::ostream &output)
{
  output << std::endl << " " << param->getIdentifier() << "=";
  Parser::Expr::serializeExpression(param->getValue(), output, &model);
  if (! param->isConst()) { output << " v"; }
  if (param->hasName()) { output << " \"" << param->getName() << "\""; }
}


/* Handle list of rules. */
void
Writer::processRuleList(Ast::Model &model, std::ostream &output)
{
  std::list<std::string> rules; std::stringstream temp;

  // Iterate over variable definitions:
  for (Ast::Model::iterator item=model.begin(); item!=model.end(); item++) {
    if (! Ast::Node::isVariableDefinition(*item)) { continue; }
    Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(*item);
    if (var->hasRule()) {
      processRule(var, model, temp); rules.push_back(temp.str()); temp.str("");
    }
  }

  // Serialize...
  output << std::endl << "@rules";
  for (std::list<std::string>::iterator iter=rules.begin(); iter!=rules.end(); iter++) {
    output << std::endl << "  " << *iter;
  }
  output << std::endl;
}

void
Writer::processRule(Ast::VariableDefinition *var, const Ast::Model &model, std::ostream &output)
{
  if (Ast::Node::isAssignmentRule(var->getRule())) {
    Ast::AssignmentRule *rule = static_cast<Ast::AssignmentRule *>(var->getRule());
    output << var->getIdentifier() << " = " << rule->getRule();
  } else if (Ast::Node::isRateRule(var->getRule())) {
    Ast::RateRule *rule = static_cast<Ast::RateRule *>(var->getRule());
    output << "@rate: " << var->getIdentifier() << " = ";
    Parser::Expr::serializeExpression(rule->getRule(), output, &model);
  }
}


/* Handle list of reactions. */
void
Writer::processReactionList(Ast::Model &model, std::ostream &output)
{
  if (0 == model.numReactions()) { return; }
  output << std::endl << "@reactions";
  for (size_t i=0; i<model.numReactions(); i++) {
    processReaction(model.getReaction(i), model, output);
    processKineticLaw(model.getReaction(i)->getKineticLaw(), output);
    output << std::endl;
  }
}

void
Writer::processReaction(Ast::Reaction *reac, const Ast::Model &model, std::ostream &output)
{
  std::stringstream temp;

  if (reac->isReversible()) { output << std::endl << " @rr="; }
  else { output << std::endl << " @r="; }
  output << " " << reac->getIdentifier();
  if (reac->hasName()) { output << " \"" << reac->getName()<<"\""; }

  std::list<std::string> reactants;
  std::list<std::string> products;
  std::list<std::string> modifiers;

  // assemble list of reactants
  for (Ast::Reaction::iterator item = reac->reactantsBegin(); item != reac->reactantsEnd(); item++)
  {
    if (! GiNaC::is_a<GiNaC::numeric>(item->second))
    {
      ExportError err;
      err << "Can not export reaction " << reac->getIdentifier() << " as SBML-sh: "
          << "Stoichiometry (" << GiNaC::ex_to<GiNaC::numeric>(item->second).to_double()
          << ") for reactant " << item->first->getIdentifier() << " is not an integer.";
      throw err;
    }

    temp.str("");
    if (1 != item->second) { temp << GiNaC::ex_to<GiNaC::numeric>(item->second).to_double() << " "; }
    temp << item->first->getIdentifier();
    reactants.push_back(temp.str());
  }

  // assemble list of products
  for (Ast::Reaction::iterator item = reac->productsBegin(); item != reac->productsEnd(); item++)
  {
    if (! GiNaC::is_a<GiNaC::numeric>(item->second))
    {
      ExportError err;
      err << "Can not export reaction " << reac->getIdentifier() << " as SBML-sh: "
          << "Stoichiometry (" << item->second
          << ") for product " << item->first->getIdentifier() << " is not an integer.";
      throw err;
    }

    temp.str("");
    if (1 != item->second) { temp << GiNaC::ex_to<GiNaC::numeric>(item->second).to_double() << " "; }
    temp << item->first->getIdentifier();
    products.push_back(temp.str());
  }

  // Assemble list of modifiers
  for (size_t i=0; i<model.numSpecies(); i++) {
    Ast::Species *species = model.getSpecies(i);
    // Check if species is a modifier of the reaction
    if (reac->isModifier(species)) { modifiers.push_back(species->getIdentifier()); }
  }

  output << std::endl << "  ";
  // Serialize reactants:
  if (0 < reactants.size()) {
    std::list<std::string>::iterator item = reactants.begin();
    for (size_t i=0; i<(reactants.size()-1); i++, item++) {
      output << *item << " + ";
    }
    output << *item;
  }

  output << " -> ";
  // Serialize products:
  if (0 < products.size()) {
    std::list<std::string>::iterator item = products.begin();
    for (size_t i=0; i<(products.size()-1); i++, item++) {
      output << *item << " + ";
    }
    output << *item;
  }

  // Serialize modifier
  if (0 < modifiers.size()) {
    output << " : ";
    std::list<std::string>::iterator mod = modifiers.begin();
    for (size_t i=0; i<(modifiers.size()-1); i++, mod++) {
      output << *mod << ", ";
    }
    output << *mod;
  }
}

void
Writer::processKineticLaw(Ast::KineticLaw *law, std::ostream &output)
{
  output << std::endl << "  ";
  Parser::Expr::serializeExpression(law->getRateLaw(), output, law);

  // Serialize local parameters
  if (0 < law->numParameters())
  {
    size_t i=0; Ast::Parameter *param = 0;
    output << ": ";

    for (; i<(law->numParameters()-1); i++) {
      param = law->getParameter(i);
      if (! param->isConst()) {
        ExportError err;
        err << "Can not export kinetic-law to SBML-sh: The local parameter "
            << param->getIdentifier() << " is not constant.";
        throw err;
      }

      if (! param->hasValue()) {
        ExportError err;
        err << "Can not export kinetic-law to SBML-sh: The local parameter "
            << param->getIdentifier() << " has no value.";
        throw err;
      }

      param = law->getParameter(i);
      output << param->getIdentifier() << "=";
      Parser::Expr::serializeExpression(param->getValue(), output, law);
      output << ", ";
    }

    param = law->getParameter(i);
    if (! param->isConst()) {
      ExportError err;
      err << "Can not export kinetic-law to SBML-sh: The local parameter "
          << param->getIdentifier() << " is not constant.";
      throw err;
    }

    if (! param->hasValue()) {
      ExportError err;
      err << "Can not export kinetic-law to SBML-sh: The local parameter "
          << param->getIdentifier() << " has no value.";
      throw err;
    }

    param = law->getParameter(i);
    output << param->getIdentifier() << "=";
    Parser::Expr::serializeExpression(param->getValue(), output, law);
  }
}

