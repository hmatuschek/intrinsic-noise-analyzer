#include "module.hh"
#include "exception.hh"
#include "reaction.hh"
#include "functiondefinition.hh"


using namespace Fluc;
using namespace Fluc::Ast;


Module::Module(const std::string &name)
  : Scope(false), name(name), time_symbol("t"),
    substance_unit(ScaledBaseUnit(ScaledBaseUnit::MOLE, 1, 0, 1)),
    volume_unit(ScaledBaseUnit(ScaledBaseUnit::LITRE, 1, 0, 1)),
    area_unit(ScaledBaseUnit(ScaledBaseUnit::METRE, 1, 0, 2)),
    length_unit(ScaledBaseUnit(ScaledBaseUnit::METRE, 1, 0, 1)),
    time_unit(ScaledBaseUnit(ScaledBaseUnit::SECOND, 1, 0, 1))
{
  // Populate pre-defined units.
  this->predefined_units["ampere"] = Unit(ScaledBaseUnit(ScaledBaseUnit::AMPERE, 1, 0, 1));
  this->predefined_units["becquerel"] = Unit(ScaledBaseUnit(ScaledBaseUnit::BECQUEREL, 1, 0, 1));
  this->predefined_units["candela"] = Unit(ScaledBaseUnit(ScaledBaseUnit::CANDELA, 1, 0, 1));
  this->predefined_units["coulomb"] = Unit(ScaledBaseUnit(ScaledBaseUnit::COULOMB, 1, 0, 1));
  this->predefined_units["dimensionless"] = Unit(ScaledBaseUnit(ScaledBaseUnit::DIMENSIONLESS, 1, 0, 1));
  this->predefined_units["farad"] = Unit(ScaledBaseUnit(ScaledBaseUnit::FARAD, 1, 0, 1));
  this->predefined_units["gram"] = Unit(ScaledBaseUnit(ScaledBaseUnit::GRAM, 1, 0, 1));
  this->predefined_units["katal"] = Unit(ScaledBaseUnit(ScaledBaseUnit::KATAL, 1, 0, 1));
  this->predefined_units["gray"] = Unit(ScaledBaseUnit(ScaledBaseUnit::GRAY, 1, 0, 1));
  this->predefined_units["kelvin"] = Unit(ScaledBaseUnit(ScaledBaseUnit::KELVIN, 1, 0, 1));
  this->predefined_units["henry"] = Unit(ScaledBaseUnit(ScaledBaseUnit::HENRY, 1, 0, 1));
  this->predefined_units["kilogram"] = Unit(ScaledBaseUnit(ScaledBaseUnit::KILOGRAM, 1, 0, 1));
  this->predefined_units["hertz"] = Unit(ScaledBaseUnit(ScaledBaseUnit::HERTZ, 1, 0, 1));
  this->predefined_units["litre"] = Unit(ScaledBaseUnit(ScaledBaseUnit::LITRE, 1, 0, 1));
  this->predefined_units["item"] = Unit(ScaledBaseUnit(ScaledBaseUnit::ITEM, 1, 0, 1));
  this->predefined_units["lumen"] = Unit(ScaledBaseUnit(ScaledBaseUnit::LUMEN, 1, 0, 1));
  this->predefined_units["joule"] = Unit(ScaledBaseUnit(ScaledBaseUnit::JOULE, 1, 0, 1));
  this->predefined_units["lux"] = Unit(ScaledBaseUnit(ScaledBaseUnit::LUX, 1, 0, 1));
  this->predefined_units["metre"] = Unit(ScaledBaseUnit(ScaledBaseUnit::METRE, 1, 0, 1));
  this->predefined_units["mole"] = Unit(ScaledBaseUnit(ScaledBaseUnit::MOLE, 1, 0, 1));
  this->predefined_units["newton"] = Unit(ScaledBaseUnit(ScaledBaseUnit::NEWTON, 1, 0, 1));
  this->predefined_units["ohm"] = Unit(ScaledBaseUnit(ScaledBaseUnit::OHM, 1, 0, 1));
  this->predefined_units["pascal"] = Unit(ScaledBaseUnit(ScaledBaseUnit::PASCAL, 1, 0, 1));
  this->predefined_units["radian"] = Unit(ScaledBaseUnit(ScaledBaseUnit::RADIAN, 1, 0, 1));
  this->predefined_units["second"] = Unit(ScaledBaseUnit(ScaledBaseUnit::SECOND, 1, 0, 1));
  this->predefined_units["watt"] = Unit(ScaledBaseUnit(ScaledBaseUnit::WATT, 1, 0, 1));
  this->predefined_units["siemens"] = Unit(ScaledBaseUnit(ScaledBaseUnit::SIEMENS, 1, 0, 1));
  this->predefined_units["weber"] = Unit(ScaledBaseUnit(ScaledBaseUnit::WEBER, 1, 0, 1));
  this->predefined_units["sievert"] = Unit(ScaledBaseUnit(ScaledBaseUnit::SIEVERT, 1, 0, 1));
  this->predefined_units["steradian"] = Unit(ScaledBaseUnit(ScaledBaseUnit::STERADIAN, 1, 0, 1));
  this->predefined_units["tesla"] = Unit(ScaledBaseUnit(ScaledBaseUnit::TESLA, 1, 0, 1));
  this->predefined_units["volt"] = Unit(ScaledBaseUnit(ScaledBaseUnit::VOLT, 1, 0, 1));
}


Module::~Module()
{
  // Free constaints:
  for (constraintIterator iter = this->constraints.begin();
       iter != this->constraints.end(); iter++)
  {
    delete *iter;
  }
}


bool
Module::hasName() const
{
  return this->name.size() != 0;
}


const std::string &
Module::getName() const
{
  return this->name;
}


void
Module::setName(const std::string &name)
{
  this->name = name;
}


void
Module::addDefinition(Definition *def)
{
  // First, add definition to Scope:
  Scope::addDefinition(def);

  // If definition is a variable definition:
  if (Node::isVariableDefinition(def))
  {
    Ast::VariableDefinition *var = static_cast<Ast::VariableDefinition *>(def);
    this->symbol_table[var->getSymbol()] = var;
  }
}


size_t
Module::getNumConstraints()
{
  return this->constraints.size();
}


void
Module::addConstraint(Constraint *constraint)
{
  this->constraints.push_back(constraint);
}


Module::constraintIterator
Module::constraintBegin()
{
  return this->constraints.begin();
}


Module::constraintIterator
Module::constraintEnd()
{
  return this->constraints.end();
}


GiNaC::symbol
Module::getTime()
{
  return this->time_symbol;
}


const Unit &
Module::getDefaultSubstanceUnit() const
{
  return this->substance_unit;
}

void
Module::setDefaultSubstanceUnit(const ScaledBaseUnit &unit)
{
  if (! unit.isSubstanceUnit())
  {
    InternalError err;
    err << "Can not reset substance unit with non-substance unit type.";
    throw err;
  }

  this->substance_unit = unit;
}


const Unit &
Module::getDefaultVolumeUnit() const
{
  return this->volume_unit;
}

void
Module::setDefaultVolumeUnit(const ScaledBaseUnit &unit)
{
  if (! unit.isVolumeUnit())
  {
    InternalError err;
    err << "Can not reset volume unit with non-volume unit.";
    throw err;
  }

  this->volume_unit = unit;
}


const Unit &
Module::getDefaultAreaUnit() const
{
  return this->area_unit;
}

void
Module::setDefaultAreaUnit(const ScaledBaseUnit &unit)
{
  if (! unit.isAreaUnit())
  {
    InternalError err;
    err << "Can not reset area unit with non-area unit.";
    throw err;
  }

  this->area_unit = area_unit;
}


const Unit &
Module::getDefaultLengthUnit() const
{
  return this->length_unit;
}

void
Module::setDefaultLengthUnit(const ScaledBaseUnit &unit)
{
  if (! unit.isLengthUnit())
  {
    InternalError err;
    err << "Can not replace length unit with non-length unit.";
    throw err;
  }

  this->length_unit = unit;
}


const Unit &
Module::getDefaultTimeUnit() const
{
  return this->time_unit;
}

void
Module::setDefaultTimeUnit(const ScaledBaseUnit &unit)
{
  if (! unit.isTimeUnit())
  {
    InternalError err;
    err << "Can not replace time unit with non-time unit.";
    throw err;
  }

  this->time_unit = unit;
}


const Unit &
Module::getUnit(const std::string &name)
{
  // Check if name is one of the defaults:
  if ("substance" == name) {
    return this->substance_unit;
  } else if ("volume" == name) {
    return this->volume_unit;
  } else if ("area" == name) {
    return this->area_unit;
  } else if ("length" == name) {
    return this->length_unit;
  } else if ("time" == name) {
    return this->time_unit;
  }

  // Check if name is one of the pre-defined units:
  std::map<std::string, Unit>::iterator item;
  if (this->predefined_units.end() != (item  = this->predefined_units.find(name)))
  {
    return item->second;
  }

  // Otherwise search for a definition:
  return this->getUnitDefinition(name)->getUnit();
}


UnitDefinition *
Module::getUnitDefinition(const std::string &identifier)
{
  Definition *def = this->getDefinition(identifier);

  if (! Node::isUnitDefinition(def))
  {
    SymbolError err;
    err << "Symbol " << identifier << " does not name a unit-definition in this module.";
    throw err;
  }

  return static_cast<Ast::UnitDefinition *>(def);
}


bool
Module::hasVariable(const GiNaC::symbol &symbol)
{
  return this->symbol_table.end() != this->symbol_table.find(symbol);
}


bool
Module::hasVariable(const std::string &identifier)
{
  return this->hasDefinition(identifier) && Node::isVariableDefinition(this->getDefinition(identifier));
}


VariableDefinition *
Module::getVariable(const std::string &identifier)
{
  Ast::Definition *def = this->getDefinition(identifier);

  if (! Node::isVariableDefinition(def))
  {
    SymbolError err;
    err << "Symbol " << identifier << " does not name a variable.";
    throw err;
  }

  return static_cast<Ast::VariableDefinition *>(def);
}


VariableDefinition *
Module::getVariable(const GiNaC::symbol &symbol)
{
  std::map<GiNaC::symbol, VariableDefinition *, GiNaC::ex_is_less>::iterator item
      = this->symbol_table.find(symbol);

  if (this->symbol_table.end() == item)
  {
    SymbolError err;
    err << "Symbol " << symbol << " does not name a variable.";
    throw err;
  }

  return item->second;
}


Reaction *
Module::getReaction(const std::string &identifier)
{
  Ast::Definition *def = this->getDefinition(identifier);

  if (! Node::isReactionDefinition(def))
  {
    SymbolError err;
    err << "There is no reaction with identifier " << identifier << " defined in this module.";
    throw err;
  }

  return static_cast<Ast::Reaction *>(def);
}


FunctionDefinition *
Module::getFunction(const std::string &identifier)
{
  Ast::Definition *def = this->getDefinition(identifier);

  if (! Node::isFunctionDefinition(def))
  {
    SymbolError err;
    err << "Symbol " << identifier << " does not name a function definition in this module.";
    throw err;
  }

  return static_cast<Ast::FunctionDefinition *>(def);
}



bool
Module::isExplTimeDep(const GiNaC::ex &expression)
{
  //fixed this todo: return 0 != GiNaC::diff(expression, this->time_symbol, 1);
  return expression.has(this->time_symbol);
}
