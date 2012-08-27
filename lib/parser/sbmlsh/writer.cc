#include "writer.hh"
#include "ast/model.hh"
#include "ast/reaction.hh"
#include "exception.hh"
#include "parser/expr/writer.hh"
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

  // Process default substance unit first:
  temp.str(""); temp << std::endl << " substance = ";
  processScaledUnit(model.getDefaultSubstanceUnit().asScaledBaseUnit(), temp);
  units.push_back(temp.str());

  // Process default volume unit:
  temp.str(""); temp << std::endl << " volume = ";
  processScaledUnit(model.getDefaultVolumeUnit().asScaledBaseUnit(), temp);
  units.push_back(temp.str());

  // Process default area unit:
  temp.str(""); temp << std::endl << " area = ";
  processScaledUnit(model.getDefaultAreaUnit().asScaledBaseUnit(), temp);
  units.push_back(temp.str());

  // Process default volume unit:
  temp.str(""); temp << std::endl << " length = ";
  processScaledUnit(model.getDefaultLengthUnit().asScaledBaseUnit(), temp);
  units.push_back(temp.str());

  // Process default volume unit:
  temp.str(""); temp << std::endl << " time = ";
  processScaledUnit(model.getDefaultTimeUnit().asScaledBaseUnit(), temp);
  units.push_back(temp.str());

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
    output << std::endl;
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
    Parser::Expr::Writer::write(comp->getValue(), scope, output);
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
  if (species->getUnit().isSubstanceUnit()) { output << species->getIdentifier() << " "; }
  else {output << "[" << species->getIdentifier() << "] "; }
  output << " = "; Parser::Expr::Writer::write(species->getValue(), model, output); output << " ";
  if (species->getUnit().isSubstanceUnit()) { output << "s"; }
  /// @bug There is no "species has boundary condition" flag.
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
  Parser::Expr::Writer::write(param->getValue(), model, output);
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
    Parser::Expr::Writer::write(rule->getRule(), model, output);
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
  output << std::endl;
}

void
Writer::processReaction(Ast::Reaction *reac, std::ostream &output)
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
  for (Ast::Reaction::iterator item = reac->reacBegin(); item != reac->reacEnd(); item++)
  {
    if (! GiNaC::is_a<GiNaC::numeric>(item->second))
    {
      ExportError err;
      err << "Can not export reaction " << reac->getIdentifier() << " as SBML-SH: "
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
  for (Ast::Reaction::iterator item = reac->prodBegin(); item != reac->prodEnd(); item++)
  {
    if (! GiNaC::is_a<GiNaC::numeric>(item->second))
    {
      ExportError err;
      err << "Can not export reaction " << reac->getIdentifier() << " as SBML-SH: "
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
  for (Ast::Reaction::mod_iterator item = reac->modBegin(); item != reac->modEnd(); item++)
  {
    modifiers.push_back((*item)->getIdentifier());
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
  Parser::Expr::Writer::write(law->getRateLaw(), *law, output);

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
      output << param->getIdentifier() << "=";
      Parser::Expr::Writer::write(param->getValue(), *law, output);
      output << ", ";
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
    output << param->getIdentifier() << "=";
    Parser::Expr::Writer::write(param->getValue(), *law, output);
  }
}


/* Serialize events... (haha!)*/
void
Writer::processEventList(Ast::Model &model, std::ostream &output)
{
  /// @todo Implement serialization of event, as soon as they are supported by the @c Ast::Model.
}
