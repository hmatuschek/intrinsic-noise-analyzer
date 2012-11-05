#ifndef __INA_AST_UNITCONVERTER_HH__
#define __INA_AST_UNITCONVERTER_HH__

#include "unitdefinition.hh"
#include "../exception.hh"


namespace iNA {
namespace Ast {

/** A trivial class to tranlate a value given in unit A, into the same value in unit B. */
class UnitConverter
{
public:
  /** Defines the unit conversion error class, to be thrown if a unit can not be converted. */
  class Error : public iNA::Exception {
  public:
    /** Empty constructor. */
    Error();
    /** Constructor with message. */
    Error(const std::string &message);
    /** Copy constructor. */
    Error(const Error &other);
  };

public:
  /** This function tries to calculate the conversion factor custom a value given in Unit A into the
   * same value in unit B. The conversion factor is returned as a numerical GiNaC value. This
   * factor must be applied to the value.
   * @throws UnitConverter::Error If the units can not be converted into each other. */
  static GiNaC::ex conversionFactor(const Unit &a, const Unit &b) throw (UnitConverter::Error);

protected:
  /** Returns the conversion factor if a & b can reduce. */
  static Unit reduceToSI(const Unit &unit);
  /** Returns the conversion factor if a & b can reduce. */
  static Unit reduceToSI(ScaledBaseUnit::BaseUnit base_unit, int exponent);
};

}
}

#endif // UNITCONVERTER_HH
