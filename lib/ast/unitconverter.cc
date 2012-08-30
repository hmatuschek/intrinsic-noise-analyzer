#include "unitconverter.hh"

using namespace iNA;
using namespace iNA::Ast;


UnitConverter::Error::Error() : Exception() { }
UnitConverter::Error::Error(const std::string &message) : Exception(message) { }
UnitConverter::Error::Error(const Error &other) : Exception(other) { }


Unit
UnitConverter::reduceToSI(ScaledBaseUnit::BaseUnit base_unit, int exponent)
{
  // Default if it is already a base unit:
  Unit result = ScaledBaseUnit(base_unit, 1, 0, exponent);

  /// @todo Perform much more conversion here!!!

  // Litre -> 10-3 m^3
  if (ScaledBaseUnit::LITRE == base_unit) {
    result = ScaledBaseUnit(ScaledBaseUnit::METRE, 1, 0, 3*exponent);
    result = result * Unit::dimensionless(1, -3*exponent);
  }

  return result;
}


Unit
UnitConverter::reduceToSI(const Unit &unit) {
  Unit result;

  for (Unit::iterator item=unit.begin(); item!=unit.end(); item++) {
    result = result * reduceToSI(item->first, item->second);
  }

  result = result * Unit::dimensionless(unit.getMultiplier(), unit.getScale());
  return result;
}


GiNaC::ex
UnitConverter::conversionFactor(const Unit &a, const Unit &b) throw (UnitConverter::Error)
{
  // Reduce units a & b to SI
  Unit base_a = reduceToSI(a), base_b = reduceToSI(b);
  // Obtain the factor (should be dimensionless)
  Unit factor = base_b/base_a;

  // Check unit of factor:
  if (! factor.isDimensionless()) {
    std::stringstream buffer;
    buffer << "Can not convert unit "; a.dump(buffer); buffer << " into unit "; b.dump(buffer);
    throw UnitConverter::Error(buffer.str());
  }

  // Return factor:
  return factor.getMultiplier() * GiNaC::pow(10, factor.getScale());
}

