#ifndef __FLUC_AST_UNITDEFINITION_HH__
#define __FLUC_AST_UNITDEFINITION_HH__

#include "definition.hh"
#include <list>


namespace iNA {
namespace Ast {


/** Represents an unit, composed as a product of scaled base-units (@c Unit::BaseUnit) i.e.
 * \f[
 *  U = m\cdot 10^s\cdot \prod_i^ N u_i^{e_i}\,,
 * \f]
 * where @c m and @c s represent the common multiplier and scale and \f$u_i\f$, \f$e_i\f$ represents
 * the \f$N\f$ pairs of @c BaseUnit and exponent that from the unit \f$U\f$. A dimensionless unit is
 * defined with \f$N=0\f$.
 * @ingroup ast */
class Unit
{
public:
  /** This enum holds all pre-defined base-units defined by SBML. */
  typedef enum
  {
    AMPERE,               ///< The ampere unit.
    AVOGADRO,             ///< The unit dimensionless multiplied by the numerical value of Avogadro's constant.
    BECQUEREL,            ///< The becquerel unit.
    CANDELA,              ///< The candela unit.
    CELSIUS,              ///< The Celsius unit.
    COULOMB,              ///< The coulomb unit.
    DIMENSIONLESS,        ///< A pseudo-unit indicating a dimensionless quantity.
    FARAD,                ///< The farad unit.
    GRAM,                 ///< The gram unit.
    GRAY,                 ///< The gray unit.
    HENRY,                ///< The henry unit.
    HERTZ,                ///< The hertz unit.
    ITEM,                 ///< A pseudo-unit representing a single "thing".
    JOULE,                ///< The joule unit.
    KATAL,                ///< The katal unit.
    KELVIN,               ///< The kelvin unit.
    KILOGRAM,             ///< The kilogram unit.
    LITRE,                ///< The litre unit.
    LUMEN,                ///< The lumen unit.
    LUX,                  ///< The lux unit.
    METRE,                ///< The metre unit.
    MOLE,                 ///< The mole unit.
    NEWTON,               ///< The newton unit.
    OHM,                  ///< The ohm unit.
    PASCAL,               ///< The pascal unit.
    RADIAN,               ///< The radian unit.
    SECOND,               ///< The second unit.
    SIEMENS,              ///< The siemens unit.
    SIEVERT,              ///< The sievert unit.
    STERADIAN,            ///< The steradian unit.
    TESLA,                ///< The tesla unit.
    VOLT,                 ///< The volt unit.
    WATT,                 ///< The watt unit.
    WEBER                 ///< The weber unit.
  } BaseUnit;

  /** Iterator over scaled base units of the unit. */
  typedef std::map<Unit::BaseUnit, int>::const_iterator iterator;


protected:
  /** Hidden constructor. Avoids direct instantiation. */
  Unit(const std::map<Unit::BaseUnit, int> &_units,
       double common_multiplier, int common_scale);

public:
  /** Default constructor (dimensionless). */
  Unit();
  /** Constructs a unit from the given base-unit of the form \f$ U = (m\cdot 10^s\cdot u)^ e\f$,
   * where @c m represents the given multiplier, @c s the scale @c u the base unit and @c e the
   * exponent. Please note that this representation differs from the one used to represent the
   * complete unit. */
  Unit(Unit::BaseUnit unit, double multiplier=1.0, int scale=0, int exponent=1);
  /** Copy constructor. */
  Unit(const Unit &other);

  /** Assignment. */
  const Unit &operator =(const Unit &other);
  /** Returns true if units are equal. */
  bool operator ==(const Unit &other) const;
  /** Returns true if units are not equal. */
  bool operator !=(const Unit &other) const;
  /** Implements simple unit manipulations. */
  Unit operator *(const Unit &other) const;
  /** Implements simple unit manipulations. */
  Unit operator /(const Unit &other) const;
  /** Implements a lexicographic ordering of the unit. */
  bool operator <(const Unit &other) const;
  /** Implements the integer exponent of units. */
  Unit pow(int exponent) const;

  /** Returns the common multiplier of the unit. */
  double getMultiplier() const;
  /** Returns the common scale of the unit. */
  double getScale() const;

  /** Returns the number of scaled base units that form this unit. Note, for a dimensionless unit
   * this figure is 0. */
  size_t size() const;

  /** Returns @c true if the unit is a scaled base unit. */
  bool isScaledBaseUnit() const;
  /** Returns true, if the unit is a scaled variant of the given base-unit with given exponent. */
  bool isVariantOf(Unit::BaseUnit baseUnit, int expo = 1.0) const;
  /** Retunrs true, if the unit contains a variant of the given base-unit with given exponent. */
  bool hasVariantOf(Unit::BaseUnit baseUnit, int expo = 1.0) const;
  /** Returns true, if the unit is a linear scaled substance unit. */
  bool isSubstanceUnit() const;
  /** Returns true, if one of the unit-factors is a linear scaling of a substance unit. */
  bool hasSubstanceUnit() const;
  /** Returns true, if the unit is a linear scaled substance unit divied by a linear scaled
   * volume unit. */
  bool isConcentrationUnit() const;
  /** Returns true, if the unit is a linear scaled volume unit. */
  bool isVolumeUnit() const;
  /** Retunrs true if the unit is a area unit. */
  bool isAreaUnit() const;
  /** Returns true if the unit is a length unit. */
  bool isLengthUnit() const;
  /** Returns true if the unit is a time unit. */
  bool isTimeUnit() const;
  /** Retunrs true, if the unit is dimensionless. */
  bool isDimensionless() const;
  /** Retunrs true if the unit is dimensionless, has multiplier 1 and scale 0. */
  bool isExactlyDimensionless() const;

  /** Dumps the unit into the given stream. */
  void dump(std::ostream &str, bool tex=false) const;
  /** Dumps the unit as a string. */
  std::string dump(bool html=false) const;

  /** Returns the unit as a scaled base unit.
   * @note This method only returns a proper @c ScaledUnit instance if the unit consists of a
   * single scaled base unit. Otherwise it throws a @c InternalError exception. */
  void asScaledBaseUnit(Unit::BaseUnit &unit, double &multiplier, int &scale, int &exponent) const;

  /** Iterator pointing to the first scaled base unit of the unit. */
  iterator begin() const;
  /** Iterator pointing right after the last scaled base unit of this unit. */
  iterator end() const;

public:
  /** Constructs a dimensionless unit. */
  static Unit dimensionless(double multiplier=1.0, int scale=0);

  /** Helper function to get a short string representation of a base-unit. */
  static std::string baseUnitRepr(Unit::BaseUnit base);
  /** Helper function to get the name of the base-unit as defined by SBML. */
  static std::string baseUnitName(Unit::BaseUnit base);
  /** Helper function to get a base-unit by its name as defined by SBML. */
  static Unit::BaseUnit baseUnitByName(const std::string &name);
  /** Returns true if the given name names a base unit. */
  static bool isBaseUnitName(const std::string &name);

protected:
  /** The common multiplier of all scaled base-units. */
  double _common_multiplier;
  /** The common scale of all scaled base-units. */
  double _common_scale;
  /** This list represents the product of scaled base-units, that build the actual unit. */
  std::map<Unit::BaseUnit, int> _units;

private:
  /** Static unit->name table. */
  static std::map<Unit::BaseUnit, std::string> _unit_to_name;
  /** Static name->unit table. */
  static std::map<std::string, Unit::BaseUnit> _name_to_unit;

};


}
}

#endif // UNITDEFINITION_HH
