#include "species.hh"
#include "exception.hh"

using namespace Fluc;
using namespace Fluc::Ast;


Species::Species(const std::string &id, const Unit &unit, Compartment *compartment, bool is_const)
  : VariableDefinition(Node::SPECIES_DEFINITION, id, unit, is_const),
    compartment(compartment)
{
  // Check if unit is a proper substance or densitity unit:
  if (! unit.isConcentrationUnit() && ! unit.isSubstanceUnit())
  {
    SemanticError err;
    std::stringstream str; unit.dump(str);
    err << "Can not define species " << id << " unit "
        << str.str() << " which is not a substance or density unit.";
    throw err;
  }

  if (unit.isSubstanceUnit()) {
    this->substance_units = true;
  } else {
    this->substance_units = false;
  }
}


Species::Species(const std::string &id, const GiNaC::ex &init_val, const Unit &unit,
                 bool only_subst_units,
                 Compartment *compartment, const std::string &name, bool is_const)
  : VariableDefinition(Node::SPECIES_DEFINITION, id, init_val, unit, name, is_const),
    compartment(compartment), substance_units(only_subst_units)
{
  // Check if unit is a proper substance or densitity unit:
  if (! unit.isConcentrationUnit() && ! unit.isSubstanceUnit())
  {
    SemanticError err;
    std::stringstream str; unit.dump(str);
    err << "Can not define species " << id << " unit "
        << str.str() << " which is not a substance or density unit.";
    throw err;
  }

  if (substance_units && ! (unit.isDimensionless() || unit.isSubstanceUnit())) {
    SemanticError err;
    std::stringstream str; unit.dump(str);
    err << "Can not define species " << id << " with unit "
        << str.str() << " and define the species to have only substance units.";
    throw err;
  }
}


Compartment *
Species::getCompartment()
{
  return this->compartment;
}

bool
Species::hasOnlySubstanceUnits()
{
 return this->substance_units;
}


