#include "exporter.hh"
#include "ast/model.hh"
#include "ast/reaction.hh"
#include "exception.hh"
#include "exporter.hh"
#include <fstream>


using namespace Fluc;
using namespace Fluc::Parser::Sbmlsh;


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
  processEventList(model, output);
}


/* Serialize model header and default units. */
void
Writer::processModelHeader(Ast::Model &model, std::ostream &output)
{
  /// @bug @c Ast::Model has no identifier field, used "model" as identifier here.
  output << "@model:3.1.1 = " << "model";
  if (model.hasName()) output << " \"" << model.getName() << "\"";
  std::list< std::pair<char, std::string> > default_units;

  // Handle default substance unit:
  {
    Ast::ScaledBaseUnit unit = model.getDefaultSubstanceUnit().asScaledBaseUnit();
    if (! ((1 == unit.getExponent()) && (1 == unit.getMultiplier()) && (0 == unit.getScale())) ) {
      std::stringstream str;
      str << "Can not export model as SBML-SH: Can not define unit "; unit.dump(str);
      str << " as default substance unit.";
      throw SemanticError(str.str());
    }

    if (unit.getBaseUnit() != Ast::ScaledBaseUnit::MOLE) {
      default_units.push_back(
            std::pair<char, std::string>('s', getBaseUnitIdentifier(unit.getBaseUnit())));
    }
  }

  // Handle default time unit.
  {
    Ast::ScaledBaseUnit unit = model.getDefaultTimeUnit().asScaledBaseUnit();
    if (! ((1 == unit.getExponent()) && (1 == unit.getMultiplier()) && (0 == unit.getScale())) ) {
      std::stringstream str;
      str << "Can not export model as SBML-SH: Can not define unit "; unit.dump(str);
      str << " as default time unit.";
      throw SemanticError(str.str());
    }

    if (unit.getBaseUnit() != Ast::ScaledBaseUnit::SECOND) {
      default_units.push_back(
            std::pair<char, std::string>('t', getBaseUnitIdentifier(unit.getBaseUnit())));
    }
  }

  // Handle default volume unit.
  {
    Ast::ScaledBaseUnit unit = model.getDefaultVolumeUnit().asScaledBaseUnit();
    if (! ((1 == unit.getExponent()) && (1 == unit.getMultiplier()) && (0 == unit.getScale())) ) {
      std::stringstream str;
      str << "Can not export model as SBML-SH: Can not define unit "; unit.dump(str);
      str << " as default volume unit.";
      throw SemanticError(str.str());
    }

    if (unit.getBaseUnit() != Ast::ScaledBaseUnit::LITRE) {
      default_units.push_back(
            std::pair<char, std::string>('v', getBaseUnitIdentifier(unit.getBaseUnit())));
    }
  }

  /// @bug There is no way to handle proper area units!

  // Handle default length unit.
  {
    Ast::ScaledBaseUnit unit = model.getDefaultLengthUnit().asScaledBaseUnit();
    if (! ((1 == unit.getExponent()) && (1 == unit.getMultiplier()) && (0 == unit.getScale())) ) {
      std::stringstream str;
      str << "Can not export model as SBML-SH: Can not define unit "; unit.dump(str);
      str << " as default length unit.";
      throw SemanticError(str.str());
    }

    if (unit.getBaseUnit() != Ast::ScaledBaseUnit::METRE) {
      default_units.push_back(
            std::pair<char, std::string>('l', getBaseUnitIdentifier(unit.getBaseUnit())));
    }
  }

  // Serialize default units:
  if (0 < default_units.size()) { output << std::endl; }
  for (std::list<std::pair<char, std::string> >::iterator item = default_units.begin();
       item != default_units.end(); item++) {
    output << " " << item->first << "=" << item->second;
  }
}


/* Translates a Ast::ScaledBaseUnit::BaseUnit into its string identifier. */
std::string
Writer::getBaseUnitIdentifier(Ast::ScaledBaseUnit::BaseUnit unit)
{
  switch (unit) {
  case Ast::ScaledBaseUnit::AMPERE: return "ampere";
  case Ast::ScaledBaseUnit::AVOGADRO: return "avogadro";
  case Ast::ScaledBaseUnit::BECQUEREL: return "becquerel";
  case Ast::ScaledBaseUnit::CANDELA: return "candela";
  case Ast::ScaledBaseUnit::CELSIUS: return "celsius";
  case Ast::ScaledBaseUnit::COULOMB: return "coulomb";
  case Ast::ScaledBaseUnit::DIMENSIONLESS: return "dimensionless";
  case Ast::ScaledBaseUnit::FARAD: return "farad";
  case Ast::ScaledBaseUnit::GRAM: return "gram";
  case Ast::ScaledBaseUnit::GRAY: return "gray";
  case Ast::ScaledBaseUnit::HENRY: return "henry";
  case Ast::ScaledBaseUnit::HERTZ: return "herz";
  case Ast::ScaledBaseUnit::ITEM: return "item";
  case Ast::ScaledBaseUnit::JOULE: return "joule";
  case Ast::ScaledBaseUnit::KATAL: return "katal";
  case Ast::ScaledBaseUnit::KELVIN: return "kelvin";
  case Ast::ScaledBaseUnit::KILOGRAM: return "kilogram";
  case Ast::ScaledBaseUnit::LITRE: return "litre";
  case Ast::ScaledBaseUnit::LUMEN: return "lumen";
  case Ast::ScaledBaseUnit::LUX: return "lux";
  case Ast::ScaledBaseUnit::METRE: return "metre";
  case Ast::ScaledBaseUnit::MOLE: return "mole";
  case Ast::ScaledBaseUnit::NEWTON: return "newton";
  case Ast::ScaledBaseUnit::OHM: return "ohm";
  case Ast::ScaledBaseUnit::PASCAL: return "pascal";
  case Ast::ScaledBaseUnit::RADIAN: return "radian";
  case Ast::ScaledBaseUnit::SECOND: return "second";
  case Ast::ScaledBaseUnit::SIEMENS: return "siemens";
  case Ast::ScaledBaseUnit::SIEVERT: return "sievert";
  case Ast::ScaledBaseUnit::STERADIAN: return "sterdian";
  case Ast::ScaledBaseUnit::TESLA: return "tesla";
  case Ast::ScaledBaseUnit::VOLT: return "volt";
  case Ast::ScaledBaseUnit::WATT: return "watt";
  case Ast::ScaledBaseUnit::WEBER: return "weber";
  }

  return "dimensionless";
}


/* handle unit definitions in given model. */
void
Writer::processUnitDefinitions(Ast::Model &model, std::ostream &output)
{
  std::list<std::string> units;
  std::stringstream temp;

  for(Ast::Model::iterator item = model.begin(); item != model.end(); item++) {
    // skip non unit definitions
    if (! Ast::Node::isUnitDefinition(*item)) { continue; }
    // get unit-definition
    Ast::UnitDefinition *unit = static_cast<Ast::UnitDefinition *>(*item);
    // clear stream
    temp.str("");
    // process unit definition into temp stream:
    processUnitDefinition(unit, temp);
    // Append definition to list of unit definitions:
    units.push_back(temp.str());
  }

  // Assemble unit definition section if there are some definitions:
  if (0 < units.size()) {
    output << std::endl << "@units";
    for (std::list<std::string>::iterator row=units.begin(); row!=units.end(); row++) {
      output << *row;
    }
  }
}

void
Writer::processUnitDefinition(Ast::UnitDefinition *unit_def, std::ostream &output)
{
  std::list<std::string> units;
  std::stringstream temp;

  const Ast::Unit &unit = unit_def->getUnit();
  if ( (1 != unit.getMultiplier()) || (0 != unit.getScale())) {
    processScaledUnit(Ast::ScaledBaseUnit(
                        Ast::ScaledBaseUnit::DIMENSIONLESS, unit.getMultiplier(),
                        unit.getScale(), 1),
                      temp);
    units.push_back(temp.str()); temp.str("");
  }

  // process scaled base unit of unit:
  for (Ast::Unit::iterator it=unit.begin(); it != unit.end(); it++) {
    processScaledUnit(Ast::ScaledBaseUnit(it->first, 1, 0, it->second), temp);
    units.push_back(temp.str()); temp.str();
  }

  // Serialize
  output << std::endl << " " << unit_def->getIdentifier() << " = ";
  std::list<std::string>::iterator item = units.begin();
  if (0 < units.size()) {
    for (size_t i=0; i<(units.size()-1); i++, item++) {
      output << *item << "; ";
    }
    output << *item;
  }
}

void
Writer::processScaledUnit(const Ast::ScaledBaseUnit &unit, std::ostream &output)
{
  std::list<std::string> modifier; std::stringstream temp;
  if (1 != unit.getMultiplier()) {
    temp << "m=" << unit.getMultiplier(); modifier.push_back(temp.str()); temp.str("");
  }
  if (0 != unit.getScale()) {
    temp << "s=" << unit.getScale(); modifier.push_back(temp.str()); temp.str("");
  }
  if (1 != unit.getExponent()) {
    temp << "e=" << unit.getExponent(); modifier.push_back(temp.str()); temp.str("");
  }

  output << Writer::getBaseUnitIdentifier(unit.getBaseUnit());
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
    processCompartment(model.getCompartment(i), output);
  }
}

/* Handle compartment. */
void
Writer::processCompartment(Ast::Compartment *comp, std::ostream &output)
{
  output << std::endl << "  " << comp->getIdentifier();
  if (comp->hasValue()) { output << " = " << comp->getValue(); }
  if (comp->hasName())  { output << " \"" << comp->getName() << "\""; }
}

/* handle list of species. */
void
Writer::processSpeciesList(Ast::Model &model, std::ostream &output) {
  if (0 == model.numSpecies()) return;

  output << std::endl << "@species";
  for (size_t i=0; i<model.numSpecies(); i++) {
    processSpecies(model.getSpecies(i), output);
  }
}

/* handle single species. */
void
Writer::processSpecies(Ast::Species *species, std::ostream &output) {
  output << std::endl << "  " << species->getCompartment()->getIdentifier() << ": ";
  if (species->getUnit().isSubstanceUnit()) { output << "[" << species->getIdentifier() << "] "; }
  else { output << species->getIdentifier() << " "; }
  output << " = " << species->getValue() << " ";
  if (species->hasOnlySubstanceUnits()) { output << "s"; }
  /// @bug There is no "species has boundary condition" flag.
  if (species->isConst()) { output << "c"; }
  if (species->hasName()) { output << " " << species->getName(); }
}


/* Handle parameter definition list. */
void
Writer::processParameterList(Ast::Model &model, std::ostream &output)
{
  if (0 == model.numParameters()) return;
  output << std::endl << "@parameters";

  for (size_t i=0; i<model.numParameters(); i++) {
    processParameter(model.getParameter(i), output);
  }
}

/* Serialize single paramter definition. */
void
Writer::processParameter(Ast::Parameter *param, std::ostream &output)
{
  output << std::endl << param->getIdentifier() << "=" << param->getValue();
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
      processRule(var, temp); rules.push_back(temp.str()); temp.str("");
    }
  }

  // Serialize...
  output << std::endl << "@rules";
  for (std::list<std::string>::iterator iter=rules.begin(); iter!=rules.end(); iter++) {
    output << std::endl << "  " << *iter;
  }
}

void
Writer::processRule(Ast::VariableDefinition *var, std::ostream &output)
{
  if (Ast::Node::isAssignmentRule(var->getRule())) {
    Ast::AssignmentRule *rule = static_cast<Ast::AssignmentRule *>(var->getRule());
    output << var->getIdentifier() << " = " << rule->getRule();
  } else if (Ast::Node::isRateRule(var->getRule())) {
    Ast::RateRule *rule = static_cast<Ast::RateRule *>(var->getRule());
    output << "@rate: " << var->getIdentifier() << " = " << rule->getRule();
  }
}


/* Handle list of reactions. */
void
Writer::processReactionList(Ast::Model &model, std::ostream &output)
{
  if (0 == model.numReactions()) { return; }
  output << std::endl << "@reactions";
  for (size_t i=0; i<model.numReactions(); i++) {
    processReaction(model.getReaction(i), output);
    processKineticLaw(model.getReaction(i)->getKineticLaw(), output);
  }
}

void
Writer::processReaction(Ast::Reaction *reac, std::ostream &output)
{
  std::stringstream temp;

  if (reac->isReversible()) { output << std::endl << "@rr"; }
  else { output << std::endl << "@r"; }
  output << " " << reac->getIdentifier();
  if (reac->hasName()) { output << " " << reac->getName(); }

  std::list<std::string> reactants;
  std::list<std::string> products;
  std::list<std::string> modifiers;

  // assemble list of reactants
  for (Ast::Reaction::iterator item = reac->reacBegin(); item != reac->reacEnd(); item++)
  {
    if ((! GiNaC::is_a<GiNaC::numeric>(item->second)) ||
        (! GiNaC::is_pos_integer(GiNaC::ex_to<GiNaC::numeric>(item->second))))
    {
      ExportError err;
      err << "Can not export reaction " << reac->getIdentifier() << " as SBML-SH: "
          << "Stoichiometry (" << item->second
          << ") for reactant " << item->first->getIdentifier() << " is not an integer.";
      throw err;
    }

    temp.str("");
    if (1 != item->second) { temp << item->second << " "; }
    temp << item->first->getIdentifier();
    reactants.push_back(temp.str());
  }

  // assemble list of products
  for (Ast::Reaction::iterator item = reac->prodBegin(); item != reac->prodEnd(); item++)
  {
    if ((! GiNaC::is_a<GiNaC::numeric>(item->second)) ||
        (! GiNaC::is_pos_integer(GiNaC::ex_to<GiNaC::numeric>(item->second))))
    {
      ExportError err;
      err << "Can not export reaction " << reac->getIdentifier() << " as SBML-SH: "
          << "Stoichiometry (" << item->second
          << ") for product " << item->first->getIdentifier() << " is not an integer.";
      throw err;
    }

    temp.str("");
    if (1 != item->second) { temp << item->second << " "; }
    temp << item->first->getIdentifier();
    products.push_back(temp.str());
  }

  // Assemble list of modifiers
  for (Ast::Reaction::mod_iterator item = reac->modBegin(); item != reac->modEnd(); item++)
  {
    modifiers.push_back((*item)->getIdentifier());
  }

  // Check if there is only one modifier:
  if (1 != modifiers.size()) {
    ExportError err;
    err << "Can not export reaction " << reac->getIdentifier() << " as SBML-SH: "
        << "There are more than one modifiers for this reaction.";
    throw err;
  }

  output << std::endl;
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
  if (1 == modifiers.size()) {
    output << " : " << modifiers.front();
  }
}

void
Writer::processKineticLaw(Ast::KineticLaw *law, std::ostream &output)
{
  output << std::endl << law->getRateLaw();

  // Serialize local parameters
  if (0 < law->numParameters())
  {
    size_t i=0; Ast::Parameter *param = 0;
    output << ": ";

    for (; i<(law->numParameters()-1); i++) {
      param = law->getParameter(i);
      if (! param->isConst()) {
        ExportError err;
        err << "Can not export kinetic-law to SBML-SH: The local parameter "
            << param->getIdentifier() << " is not constant.";
        throw err;
      }

      if (! param->hasValue()) {
        ExportError err;
        err << "Can not export kinetic-law to SBML-SH: The local parameter "
            << param->getIdentifier() << " has no value.";
        throw err;
      }

      param = law->getParameter(i);
      output << param->getIdentifier() << "=" << param->getValue() << ", ";
    }

    param = law->getParameter(i);
    if (! param->isConst()) {
      ExportError err;
      err << "Can not export kinetic-law to SBML-SH: The local parameter "
          << param->getIdentifier() << " is not constant.";
      throw err;
    }

    if (! param->hasValue()) {
      ExportError err;
      err << "Can not export kinetic-law to SBML-SH: The local parameter "
          << param->getIdentifier() << " has no value.";
      throw err;
    }

    param = law->getParameter(i);
    output << param->getIdentifier() << "=" << param->getValue();
  }
}


/* Serialize events... (haha!)*/
void
Writer::processEventList(Ast::Model &model, std::ostream &output)
{
  /// @todo Implement serialization of event, as soon as they are supported by the @c Ast::Model.
}