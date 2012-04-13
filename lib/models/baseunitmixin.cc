#include "math.hh"
#include "baseunitmixin.hh"

using namespace Fluc;
using namespace Fluc::Models;

BaseUnitMixin::BaseUnitMixin(BaseModel &base)
{
  // check if there is at least one species and compartment defined:
  if(base.numSpecies()<1)
    throw InternalError("No Species defined. This errror should not have happened.");
  if(base.numCompartments()<1)
    throw InternalError("No Compartment defined. This errror should not have happened.");

  this->concentrationUnit = base.getSpecies(0)->getUnit();
  this->substanceUnit = base.getSpecies(0)->getUnit();
  this->compartmentUnit = base.getCompartment(0)->getUnit();

  //Ast::Unit compUnit = base.getCompartment(0)->getUnit();

  this->timeUnit = base.getDefaultTimeUnit();

  // loop over all compartments and check if their units are consistent

  for (size_t i=0; i<base.numCompartments();i++)
  {
      if(!(base.getCompartment(i)->getUnit()==this->compartmentUnit))
      {
      SemanticError err;
      err << "Compartments all must have same unit";
      throw err;
      }
  }

  // loop over all species and check if units are consistently defined

  for (size_t i=0; i<base.numSpecies(); i++)
  {

    if (this->compartmentUnit.isDimensionless() && !(base.getSpecies(i)->hasOnlySubstanceUnits()))
        throw InternalError("The definition of dimensionless volumes requires all species to be defined by substance units only.");

    if (!(base.getSpecies(i)->getUnit()==this->substanceUnit))
    {
      SemanticError err;
      std::stringstream str; this->substanceUnit.dump(str);
      err << "All species must be defined within either substance or concentration units:"
          << " Common unit of model is " << str.str()
          << " But species " << base.getSpecies(i)->getIdentifier() << " has unit ";
      str.str(""); base.getSpecies(i)->getUnit().dump(str);
      err << str.str() << ".";
      throw err;
    }
  }

  if (this->substanceUnit.isSubstanceUnit())
  {
    if(this->substanceUnit.isVariantOf(Ast::ScaledBaseUnit::MOLE))
    {
      // and resolve this for propensities
      for (size_t i=0; i<base.propensities.size(); i++)
      {
        base.propensities[i] = base.propensities[i]*(this->getMultiplier()*constants::AVOGADRO);
      }
    }
    else if(this->substanceUnit.isVariantOf(Ast::ScaledBaseUnit::ITEM))
    {
      // ...pass
    }
  }
  else if (this->substanceUnit.isConcentrationUnit())
  {
    // set substance unit
    this->substanceUnit = this->substanceUnit*this->compartmentUnit;

    // if true, defined in molar conc
    if (this->substanceUnit.hasVariantOf(Ast::ScaledBaseUnit::MOLE))
    {
      // and resolve this for propensities
      for (size_t i=0; i<base.propensities.size(); i++)
          base.propensities[i] = base.propensities[i]*(this->getMultiplier()*constants::AVOGADRO);
    }

    // if true, define in particle number concentrations
    if (this->substanceUnit.hasVariantOf(Ast::ScaledBaseUnit::ITEM))
    {
      // and resolve this for propensities
      for (size_t i=0; i<base.propensities.size(); i++)
          base.propensities[i] = base.propensities[i]*(this->getMultiplier());

    }

  }

  // define a concentration unit for dimensionless volumes

  if(this->compartmentUnit.isDimensionless()) this->concentrationUnit = this->substanceUnit.dimensionless();

}

const Ast::Unit &
BaseUnitMixin::getSubstanceUnit() const
{
  return this->substanceUnit;
}

const Ast::Unit &
BaseUnitMixin::getConcentrationUnit() const
{
  return this->concentrationUnit;
}

const Ast::Unit &
BaseUnitMixin::getCompartmentUnit() const
{
  return this->compartmentUnit;
}

const Ast::Unit &
BaseUnitMixin::getTimeUnit() const
{
  return this->timeUnit;
}

double
BaseUnitMixin::getMultiplier()
{
  return this->substanceUnit.getMultiplier() * std::pow(10, this->substanceUnit.getScale());
}
