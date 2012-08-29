#ifndef __FLUC_AST_UNITDEFINITION_HH__
#define __FLUC_AST_UNITDEFINITION_HH__

#include "definition.hh"
#include <list>


namespace Fluc {
namespace Ast {


/**
 * Elementary transformation (mostly scaling) of a base unit.
 *
 * @ingroup ast
 */
class ScaledBaseUnit
{
public:
  /**
   * This enum holds all pre-defined base-units defined by SBML.
   */
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


protected:
  /** Holds a weak reference to the unit. */
  BaseUnit unit;
  /** Holds the multiplier of unit. */
  double multiplier;
  /** Holds the scale of the unit. */
  int scale;
  /** Holds the exponent of the unit. */
  int exponent;


public:
  /** Default constructor, equivalent to call: ScaledBaseUnit(DIMENSIONLESS, 1, 0, 1). */
  ScaledBaseUnit();

  /** Constructs a unit from a base-unit and scaling as
   *
   * \f[ u = \left(multiplier\cdot 10^{scale}\cdot u_b\right)^{exponent} \f] */
  ScaledBaseUnit(BaseUnit unit, double multiplier, int scale, int exponent);

  /** Trivial constructor from base unit, equivalent to ScaledBaseUnit(unit, 1, 0, 1). */
  explicit ScaledBaseUnit(BaseUnit unit);

  /** Copy constructor. */
  ScaledBaseUnit(const ScaledBaseUnit &other);

  /** Implements the assignment operator for scaled units. */
  const ScaledBaseUnit &operator =(const ScaledBaseUnit &other);

  /** Returns the multiplier of unit. */
  double getMultiplier() const;

  /** Returns the scale of unit. */
  int getScale() const;

  /** Retunrs the exponent. */
  int getExponent() const;

  /** Returns the base-unit. */
  ScaledBaseUnit::BaseUnit getBaseUnit() const;

  /** Aka. is exponent == 1? Means if the unit is a linear scaling of a base unit. */
  bool isLinScaling() const;

  /** Returns true, if the ScaledUnit is a proper unit for substances.
   * I.e., if the unit is a linear scaling of either: @c ScaledUnit::MOLE, @c ScaledUnit::ITEM,
   * @c ScaledUnit::GRAM, @c ScaledUnit::KILOGRAM or @c ScaledUnit::DIMENSIONLESS. */
  bool isSubstanceUnit() const;

  /** Returns true, if the ScaledUnit is a proper unit for volumes.
   * I.e. if the unit is a linear scaling of @c ScaledUnit::LITRE or @c ScaledUnit::DIMENSIONLESS
   * or if it is a qubic scaling of @c ScaledUnit::METRE. */
  bool isVolumeUnit() const;

  /** Returns true, if the ScaledUnit is a proper unit for areas.
   * I.e., if the unit is a linear scaling of @c ScaledUnit::DIMENSIONLESS or a quadratic scaling
   * of @c ScaledUnit::METRE. */
  bool isAreaUnit() const;

  /** Returns true, if the ScaledUnit is a proper unit for length.
   * i.e. if the unit is a linear scaling of @c ScaledUnit::METRE or
   * @c ScaledUnit::DIMENSIONLESS. */
  bool isLengthUnit() const;

  /** Returns true, if the ScaledUnit is a proper unit of for time.
   * I.e. if the unit is a linear scaling of @c ScaledUnit::TIME or @c ScaledUnit::DIMENSIONLESS. */
  bool isTimeUnit() const;

  /** Dums a string representation of the unit into the given stream. */
  void dump(std::ostream &str) const;


public:
  /** Helper function to get a short string representation of a base-unit. */
  static std::string baseUnitRepr(ScaledBaseUnit::BaseUnit base);
  /** Helper function to get the name of the base-unit as defined by SBML. */
  static std::string baseUnitName(ScaledBaseUnit::BaseUnit base);
  /** Helper function to get a base-unit by its name as defined by SBML. */
  static ScaledBaseUnit::BaseUnit baseUnitByName(const std::string &name);
  /** Returns true if the given name names a base unit. */
  static bool isBaseUnitName(const std::string &name);

private:
  /** Static unit->name table. */
  static std::map<ScaledBaseUnit::BaseUnit, std::string> _unit_to_name;
  /** Static name->unit table. */
  static std::map<std::string, ScaledBaseUnit::BaseUnit> _name_to_unit;
};




/** Represents an unit, composed as a product of scaled base-units (@c ScaledBaseUnit).
 *
 * @ingroup ast
 */
class Unit
{
public:
  /** Iterator over scaled base units of the unit. */
  typedef std::map<ScaledBaseUnit::BaseUnit, int>::const_iterator iterator;

protected:
  /** The common multiplier of all scaled base-units. */
  double common_multiplier;
  /** The common scale of all scaled base-units. */
  double common_scale;
  /** This list represents the product of scaled base-units, that build the actual unit. */
  std::map<ScaledBaseUnit::BaseUnit, int> units;


protected:
  /** Hidden constructor. Avoids direct instantiation. */
  Unit(const std::map<ScaledBaseUnit::BaseUnit, int> &units,
       double common_multiplier, int common_scale);


public:
  /** Default constructor (dimensionless). */
  Unit();
  /** Constructs a unit. */
  Unit(const std::list<ScaledBaseUnit> &units);
  /** Constructs a simple unit as a scaled variant of a base-unit. */
  Unit(const ScaledBaseUnit &base);
  /** Copy constructor. */
  Unit(const Unit &other);

  /** Assignment. */
  const Unit &operator =(const Unit &other);
  /** Returns true if units are equal. */
  bool operator ==(const Unit &other) const;
  /** Implements simple unit manipulations. */
  Unit operator *(const Unit &other) const;
  /** Implements simple unit manipulations. */
  Unit operator /(const Unit &other) const;

  /** Returns the common multiplier of the unit. */
  double getMultiplier() const;
  /** Returns the common scale of the unit. */
  double getScale() const;

  /** Returns true, if the unit is a scaled variant of the given base-unit with given exponent. */
  bool isVariantOf(ScaledBaseUnit::BaseUnit baseUnit, int expo = 1.0) const;
  /** Retunrs true, if the unit contains a variant of the given base-unit with given exponent. */
  bool hasVariantOf(ScaledBaseUnit::BaseUnit baseUnit, int expo = 1.0) const;
  /** Retruns true, if the unit is a linear scaled substance unit. */
  bool isSubstanceUnit() const;
  /** Returns true, if one of the unit-factors is a linear scaling of a substance unit. */
  bool hasSubstanceUnit() const;
  /** Returns true, if the unit is a linear scaled substance unit divied by a linear scaled
   * volume unit. */
  bool isConcentrationUnit() const;
  /** Retunrs true, if the unit is dimensionless. */
  bool isDimensionless() const;
  /** Retunrs true if the unit is dimensionless, has multiplier 1 and scale 0. */
  bool isExactlyDimensionless() const;

  /** Dumps the unit into the given stream. */
  void dump(std::ostream &str, bool html=false) const;

  /** Returns the unit as a scaled base unit.
   * @note This method only returns a proper @c ScaledUnit instance if the unit consists of a
   * single scaled base unit. Otherwise it throws a @c InternalError exception. */
  ScaledBaseUnit asScaledBaseUnit() const;

  /** Iterator pointing to the first scaled base unit of the unit. */
  iterator begin() const;
  /** Iterator pointing right after the last scaled base unit of this unit. */
  iterator end() const;


public:
  /** Constructs a dimensionless unit. */
  static Unit dimensionless(double multiplier=1.0, int scale=0);
};



/**
 * This class defines a unit as a product of scaled base units.
 *
 * @ingroup
 */
class UnitDefinition : public Definition
{
public:
  /** Visitor class for unit definitions. */
  class Visitor { public: virtual void visit(const UnitDefinition *unit) = 0; };
  /** Operator class for unit definitions. */
  class Operator { public: virtual void act(UnitDefinition *unit) = 0; };

protected:
  /** Holds the unit being defined. */
  Unit unit;

public:
  /** Constructs a unit definition from scaled base units. */
  UnitDefinition(const std::string &identifier, std::list<ScaledBaseUnit> units);

  /** Constructs a unit definition from a unit. */
  UnitDefinition(const std::string &identifier, const Unit &unit);

  /** Returns true, if the unit is a linear scaled variant of the given base-unit. */
  bool isVariantOf(ScaledBaseUnit::BaseUnit baseUnit);

  /** Returns the unit being defined. */
  const Unit &getUnit() const;

  /** Simply dumps the unit-definition. */
  virtual void dump(std::ostream &str);

  /** Handles a visitor for the unit definition. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator on the unit definition. */
  virtual void apply(Ast::Operator &op);
};


}
}

#endif // UNITDEFINITION_HH
