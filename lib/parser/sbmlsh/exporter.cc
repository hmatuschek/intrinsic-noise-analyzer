#include "exporter.hh"
#include "ast/model.hh"
#include "exception.hh"

using namespace Fluc;

/* ********************************************************************************************* *
 * Forward declaration of helper functions.
 * ********************************************************************************************* */
void __process_model(Ast::Model &model, std::ostream &output);
std::string __get_base_unit_identifier(Ast::ScaledBaseUnit::BaseUnit);
void __process_unit_definitions(Ast::Model &model, std::ostream &output);
void __process_compartments(Ast::Model &model, std::ostream &output);
void __process_compartment(Ast::Compartment *comp, std::ostream &output);
void __process_species_list(Ast::Model &model, std::ostream &output);
void __process_species(Ast::Species *species, std::ostream &output);


/* Generates model header. */
void __process_model(Ast::Model &model, std::ostream &output) {
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
            std::pair<char, std::string>('s', __get_base_unit_identifier(unit.getBaseUnit())));
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
            std::pair<char, std::string>('t', __get_base_unit_identifier(unit.getBaseUnit())));
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
            std::pair<char, std::string>('v', __get_base_unit_identifier(unit.getBaseUnit())));
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
            std::pair<char, std::string>('l', __get_base_unit_identifier(unit.getBaseUnit())));
    }
  }

  // Serialize default units:
  if (0 < default_units.size()) { output << std::endl; }
  for (std::list<std::pair<char, std::string> >::iterator item = default_units.begin();
       item != default_units.end(); item++) {
    output << " " << item->first << "=" << item->second;
  }

  __process_unit_definitions(model, output);
  __process_compartments(model, output);
  __process_species_list(model, output);
}


/* Translates a Ast::ScaledBaseUnit::BaseUnit into its string identifier. */
std::string __get_base_unit_identifier(Ast::ScaledBaseUnit::BaseUnit unit)
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
void __process_unit_definitions(Ast::Model &model, std::ostream &output) {
  /// @bug There is no way to iterate over all user-defined Units in @c Ast::Model!
}

/* Handle compartments. */
void __process_compartments(Ast::Model &model, std::ostream &output) {
  if (0 == model.numCompartments()) return;

  output << std::endl << "@compartments";
  for (size_t i=0; i<model.numCompartments(); i++) {
    __process_compartment(model.getCompartment(i), temp);
  }
}

/* Handle compartment. */
void __process_compartment(Ast::Compartment *comp, std::ostream &output)
{
  ouput << std::endl << "  " << comp->getIdentifier();
  if (comp->hasValue()) { output << " = " << comp->getValue(); }
  if (comp->hasName())  { output << " \"" << comp->getName() << "\""; }
}

/* handle list of species. */
void __process_species_list(Ast::Model &model, std::ostream &output) {
  if (0 == model.numSpecies()) return;

  output << std::endl << "@species";
  for (size_t i=0; i<model.numSpecies(); i++) {
    __process_species(model.getSpecies(i), output);
  }
}

/* handle single species. */
void __process_species(Ast::Species *species, std::ostream &output) {
  output << std::endl << "  " << species->getCompartment()->getIdentifier() << ": ";
  if (species->getUnit().isSubstanceUnit()) { output << "[" << species->getIdentifier() << "] "; }
  else { output << species->getIdentifier() << " "; }
  output << " = " << species->getValue() << " ";
  if (species->hasOnlySubstanceUnits()) { output << "s"; }
  /// @bug There is no "species has boundary condition" flag.
  if (species->isConst()) { output << "c"; }
  if (species->hasName()) { output << " " << species->getName(); }
}

