#include "unitdefinition.hh"
#include <cmath>
#include "exception.hh"


using namespace iNA;
using namespace iNA::Ast;

/** Tiny helper class to create static maps. Just ignore. */
template <typename T, typename U> class create_map
{
private:
  /** A temprary map. */
  std::map<T, U> m_map;
public:
  /** Constructor, generates the first entry in the map. Instantiates the temporary map. */
  create_map(const T& key, const U& val){ m_map[key] = val; }
  /** Adds an item to the map and returns it. */
  create_map<T, U>& operator()(const T& key, const U& val) { m_map[key] = val; return *this; }
  /** Cast operator, returns the map. */
  operator std::map<T, U>() { return m_map; }
};



/* ********************************************************************************************* *
 * Implementation of Unit
 * ********************************************************************************************* */
Unit::Unit(const std::map<Unit::BaseUnit, int> &units,
           double common_multiplier, int common_scale)
  : _common_multiplier(common_multiplier), _common_scale(common_scale), _units(units)
{
  // Pass...
}


Unit::Unit()
  : _common_multiplier(1.0), _common_scale(0), _units()
{
  // Pass...
}


Unit::Unit(Unit::BaseUnit unit, double multiplier, int scale, int exponent)
  : _common_multiplier(1.0), _common_scale(0), _units()
{
  _common_multiplier *= std::pow(multiplier, exponent);
  _common_scale      += scale * exponent;

  if (Unit::DIMENSIONLESS != unit) {
    this->_units[unit] = exponent;
  }
}

Unit::Unit(const Unit &other)
  : _common_multiplier(other._common_multiplier), _common_scale(other._common_scale), _units(other._units)
{
  // Pass...
}

const Unit &
Unit::operator =(const Unit &other) {
  // Override the scaled units:
  this->_units.clear();
  this->_units = other._units;
  this->_common_multiplier = other._common_multiplier;
  this->_common_scale = other._common_scale;

  return *this;
}


double
Unit::getMultiplier() const {
  return this->_common_multiplier;
}

double
Unit::getScale() const {
  return this->_common_scale;
}

size_t
Unit::size() const {
  return _units.size();
}


bool
Unit::isScaledBaseUnit() const {
  return ((0 == _units.size()) || (1 == _units.size()));
}

bool
Unit::isVariantOf(Unit::BaseUnit baseUnit, int expo) const {
  // A unit is dimension-less if there are no units:
  if (Unit::DIMENSIONLESS == baseUnit && 0 == this->_units.size()) {
    return true;
  }

  // If the unit consists of more than one base-unit:
  if (1 != this->_units.size()) {
    return false;
  }

  return this->hasVariantOf(baseUnit, expo);
}


bool
Unit::hasVariantOf(Unit::BaseUnit baseUnit, int expo) const {
  // Any unit has a dimensionless factor!
  if (Unit::DIMENSIONLESS == baseUnit) {
    return true;
  }

  // Seach for base unti
  std::map<Unit::BaseUnit, int>::const_iterator item = this->_units.find(baseUnit);
  if (this->_units.end() == item || item->second != expo) {
    return false;
  }

  return true;
}


bool
Unit::isSubstanceUnit() const {
  // Dimensionless is a valid substabce unit.
  if (0 == this->_units.size()) {
    return true;
  }

  return (1 == this->_units.size()) && this->hasSubstanceUnit();
}


bool
Unit::hasSubstanceUnit() const {
  return (hasVariantOf(Ast::Unit::MOLE) || hasVariantOf(Ast::Unit::ITEM) ||
          hasVariantOf(Ast::Unit::GRAM) || hasVariantOf(Ast::Unit::KILOGRAM));
}


bool
Unit::isConcentrationUnit() const {
  if (this->isVariantOf(Ast::Unit::DIMENSIONLESS, -1) ||
      this->isVariantOf(Ast::Unit::LITRE, -1) ||
      this->isVariantOf(Ast::Unit::METRE, -3)) {
    return false;
  }

  if (2 != this->_units.size()) {
    return false;
  }

  return this->hasSubstanceUnit() && (this->hasVariantOf(Ast::Unit::DIMENSIONLESS, -1) ||
                                      this->hasVariantOf(Ast::Unit::LITRE, -1) ||
                                      this->hasVariantOf(Ast::Unit::METRE, -3));
}


bool
Unit::isVolumeUnit() const {
  return isVariantOf(Ast::Unit::LITRE, 1) ||
      isVariantOf(Ast::Unit::METRE, 3) ||
      isVariantOf(Ast::Unit::DIMENSIONLESS);
}

bool
Unit::isAreaUnit() const {
  return isVariantOf(Ast::Unit::METRE, 2) ||
      isVariantOf(Ast::Unit::DIMENSIONLESS);
}

bool
Unit::isLengthUnit() const {
  return isVariantOf(Ast::Unit::METRE, 1) ||
      isVariantOf(Ast::Unit::DIMENSIONLESS);
}

bool
Unit::isTimeUnit() const {
  return isVariantOf(Ast::Unit::SECOND, 1) ||
      isVariantOf(Ast::Unit::DIMENSIONLESS);
}

bool
Unit::isDimensionless() const
{
  return 0 == this->_units.size();
}


bool
Unit::isExactlyDimensionless() const {
  return isDimensionless() && (1 == _common_multiplier) && (0 == _common_scale);
}

void
Unit::asScaledBaseUnit(Unit::BaseUnit &unit, double &multiplier, int &scale, int &exponent) const {
  // Handle dimensionless unit explicitly:
  if (this->isDimensionless()) {
    unit = Unit::DIMENSIONLESS; multiplier = 1; scale = 0; exponent = 1; return;
  }

  if (1 != this->_units.size()) {
    InternalError err;
    err << "Cannot construct a scaled base-unit from non-atomic unit: "
        << this->dump();
    throw err;
  }

  // unpack
  unit = _units.begin()->first;
  exponent   = _units.begin()->second;
  scale      = int(_common_scale/exponent);
  multiplier = std::exp(std::log(_common_multiplier)/exponent);
}

Unit::iterator
Unit::begin() const {
  return _units.begin();
}

Unit::iterator
Unit::end() const {
  return _units.end();
}


void
Unit::dump(std::ostream &str, bool tex) const
{

  // return standard dump
  if(!tex)
  {
    if (1 != this->_common_multiplier || 0 != this->_common_scale) {
      str << this->_common_multiplier << "e" << this->_common_scale;
    }

    if (0 < this->_units.size())
    {
      if (1 != this->_common_multiplier || 0 != this->_common_scale)
        str << "(";

      std::map<Unit::BaseUnit, int>::const_iterator iter = this->_units.begin();
      for (size_t i=0; i<this->_units.size()-1; i++, iter++)
      {
        str << Unit::baseUnitRepr(iter->first);
        if (1 != iter->second)
          str << "^"<<iter->second;
        str << " * ";
      }

      str << Unit::baseUnitRepr(iter->first);
      if (1 != iter->second)
        str << "^"<<iter->second;
    }


    if (1 != this->_common_multiplier || 0 != this->_common_scale)
      str << ")";

    return;
  }

  // otherwise dump TeX
  str << "$";

  // catch arbitrary units
  if(isDimensionless()) str << "a.u.";

  // catch times which refer explictly to units of minutes, hours or days
  if (isVariantOf(Unit::SECOND))
  {
    float fac = _common_multiplier;
    fac *= (0 != _common_scale) ? std::pow(10.,_common_scale) : 1;

    if( fac==86400 ) { str << "d$"; return; }
    else if ( fac==3600 ) { str << "h$"; return; }
    else if ( fac==60 ) { str << "min$"; return; }
  }

  // first dump the scale
  if (1 != this->_common_multiplier) {
    str << this->_common_multiplier << "\times";
  }

  if (0 != this->_common_scale) {
    str << "10^{" << this->_common_scale << "}";
  }

  // then the units
  if (0 < this->_units.size())
  {
    std::ostringstream numerator,denominator;
    size_t nnum=0,nden=0;

    if (1 != this->_common_multiplier || 0 != this->_common_scale)
      str << "(";

    std::map<Unit::BaseUnit, int>::const_iterator iter = this->_units.begin();
    for (size_t i=0; i<this->_units.size(); i++, iter++)
    {
      if (0 < iter->second)
      {
        numerator << Unit::baseUnitRepr(iter->first);
        if (iter->second!=1)
          numerator << "^{"<<(iter->second)<<"}";
        numerator << " ";
        nnum++;
      }
      else if(0 > iter->second)
      {
        denominator << Unit::baseUnitRepr(iter->first);
        if (iter->second!=-1)
          denominator << "^{"<<-(iter->second)<<"}";
        denominator << " ";
      }
    }

    nden = _units.size()-nnum;

    if(nnum>0) {
      if (nnum>1) str << "(";
      str << numerator.str().substr(0, numerator.str().size()-1 );
      if (nnum>1) str << ")";
    } else {
      str<< "1";
    }

    if (nden>0) {
       str << "/";
       if (nden>1) str << "(";
       str << denominator.str().substr(0, denominator.str().size()-1 );
       if (nden>1) str << ")";
    }

    if (1 != this->_common_multiplier || 0 != this->_common_scale)
        str << ")";
  }
  str << "$";
}


std::string
Unit::dump(bool tex) const {
  std::stringstream buffer;
  dump(buffer, tex);
  return buffer.str();
}


bool
Unit::operator ==(const Unit &other) const
{
  if(this->_common_multiplier != other._common_multiplier || this->_common_scale != other._common_scale)
  {
    return false;
  }

  if (this->_units.size() != other._units.size())
    return false;

  // Left compare base-units with exponents:
  for (std::map<Unit::BaseUnit, int>::const_iterator iter = this->_units.begin();
       iter != this->_units.end(); iter++)
  {
    std::map<Unit::BaseUnit, int>::const_iterator item = other._units.find(iter->first);
    if (other._units.end() == item)
      return false;

    if (iter->second != item->second)
      return false;
  }

  return true;
}


bool
Unit::operator !=(const Unit &other) const {
  return ! ( (*this) == other );
}


Unit
Unit::operator *(const Unit &other) const
{
  // Copy product of own scaled base-units
  std::map<Unit::BaseUnit, int> units(this->_units);

  // add scaled units of other:
  for (std::map<Unit::BaseUnit, int>::const_iterator iter = other._units.begin();
       iter != other._units.end(); iter++)
  {
    std::map<Unit::BaseUnit, int>::iterator item = units.find(iter->first);

    if (other._units.end() != item)
    {
      units[iter->first] = units[iter->first] + iter->second;
      if (0 == units[iter->first])
      {
        units.erase(iter->first);
      }
    }
    else
    {
      units[iter->first] = iter->second;
    }
  }

  // Done.
  return Unit(units, this->_common_multiplier * other._common_multiplier,
              this->_common_scale + other._common_scale);
}


Unit
Unit::operator /(const Unit &other) const
{
  // Copy product of own scaled base-units
  std::map<Unit::BaseUnit, int> units(this->_units);

  // add scaled units of other:
  for (std::map<Unit::BaseUnit, int>::const_iterator iter = other._units.begin();
       iter != other._units.end(); iter++)
  {
    std::map<Unit::BaseUnit, int>::iterator item = units.find(iter->first);

    if (other._units.end() != item) {
      units[iter->first] = units[iter->first] - iter->second;
      if (0 == units[iter->first]) {
        units.erase(iter->first);
      }
    } else {
      units[iter->first] = -iter->second;
    }
  }

  // Done.
  return Unit(units, this->_common_multiplier / other._common_multiplier,
              this->_common_scale - other._common_scale);
}


bool
Unit::operator <(const Unit &other) const {
  // Compare by multiplier
  if (this->_common_multiplier < other._common_multiplier) { return true; }
  if (this->_common_multiplier > other._common_multiplier) { return false; }
  // Compare by scale
  if (this->_common_scale < other._common_scale) { return true; }
  if (this->_common_scale > other._common_scale) { return false; }
  // Compare by number of base units
  if (this->_units.size() < other._units.size()) { return true; }
  if (this->_units.size() > other._units.size()) { return false; }
  // Compare scaled base units:
  std::map<Unit::BaseUnit, int>::const_iterator my_unit = _units.begin();
  std::map<Unit::BaseUnit, int>::const_iterator other_unit = other._units.begin();
  for (; my_unit!=_units.end(); my_unit++, other_unit++) {
    // Compare base unit
    if (my_unit->first < other_unit->first) { return true; }
    if (my_unit->first > other_unit->first) { return false; }
    // if units are equal -> compare exponents
    if (my_unit->second < other_unit->second) { return true; }
    if (my_unit->second > other_unit->second) { return false; }
  }
  // Units are equal -> return false
  return false;
}

Unit
Unit::pow(int exponent) const {
  double common_multiplier = std::pow(this->_common_multiplier, exponent);
  int    common_scale = this->_common_scale * exponent;
  std::map<Unit::BaseUnit, int> units;
  std::map<Unit::BaseUnit, int>::const_iterator item = _units.begin();
  for (; item!=_units.end(); item++) {
    int exp = item->second * exponent;
    if (0 != exp) { units[item->first] = exp; }
  }
  return Unit(units, common_multiplier, common_scale);
}

Unit
Unit::dimensionless(double multiplier, int scale)
{
  return Unit(Unit::DIMENSIONLESS, multiplier, scale, 1);
}


std::string
Unit::baseUnitRepr(Unit::BaseUnit base)
{
  switch (base)
  {
  case Unit::DIMENSIONLESS: return "a.u.";
  case Unit::MOLE: return "mol";
  case Unit::ITEM: return "#";
  case Unit::GRAM: return "g";
  case Unit::KILOGRAM: return "kg";
  case Unit::LITRE: return "l";
  case Unit::METRE: return "m";
  case Unit::SECOND: return "s";
  default: break;
  }

  return "?";
}

std::string
Unit::baseUnitName(Unit::BaseUnit base) {
  return Unit::_unit_to_name[base];
}

Unit::BaseUnit
Unit::baseUnitByName(const std::string &name) {
  return Unit::_name_to_unit[name];
}

bool
Unit::isBaseUnitName(const std::string &name) {
  return Unit::_name_to_unit.end() != Unit::_name_to_unit.find(name);
}


std::map<Unit::BaseUnit, std::string> Unit::_unit_to_name = \
create_map<Unit::BaseUnit, std::string> (Unit::AMPERE, "ampere") \
(Unit::BECQUEREL, "becquerel") (Unit::CANDELA, "candela") \
(Unit::COULOMB, "coulomb") (Unit::DIMENSIONLESS, "dimensionless") \
(Unit::FARAD, "farad") (Unit::GRAM, "gram") (Unit::KATAL, "katal") \
(Unit::GRAY, "gray") (Unit::KELVIN, "kelvin") (Unit::HENRY, "henry") \
(Unit::KILOGRAM, "kilogram") (Unit::HERTZ, "herz") \
(Unit::LITRE, "litre") (Unit::ITEM, "item") (Unit::LUMEN, "lumen") \
(Unit::JOULE, "joule") (Unit::LUX, "lux") (Unit::METRE, "metre") \
(Unit::MOLE, "mole") (Unit::NEWTON, "newton") (Unit::OHM, "ohm") \
(Unit::PASCAL, "pascal") (Unit::RADIAN, "radian") \
(Unit::SECOND, "second") (Unit::WATT, "watt") \
(Unit::SIEMENS, "siemens") (Unit::WEBER, "weber") \
(Unit::SIEVERT, "sievert") (Unit::STERADIAN, "steradian") \
(Unit::TESLA, "tesla") (Unit::VOLT, "volt");


std::map<std::string, Unit::BaseUnit> Unit::_name_to_unit = \
create_map<std::string, Unit::BaseUnit> ("ampere", Unit::AMPERE) \
("becquerel", Unit::BECQUEREL) ("candela", Unit::CANDELA) \
("coulomb", Unit::COULOMB) ("dimensionless", Unit::DIMENSIONLESS) \
("farad", Unit::FARAD) ("gram", Unit::GRAM) ("katal", Unit::KATAL) \
("gray", Unit::GRAY) ("kelvin", Unit::KELVIN) ("henry", Unit::HENRY) \
("kilogram", Unit::KILOGRAM) ("herz", Unit::HERTZ) \
("litre", Unit::LITRE) ("item", Unit::ITEM) ("lumen", Unit::LUMEN) \
("joule", Unit::JOULE) ("lux", Unit::LUX) ("metre", Unit::METRE) \
("mole", Unit::MOLE) ("newton", Unit::NEWTON) ("ohm", Unit::OHM) \
("pascal", Unit::PASCAL) ("radian", Unit::RADIAN) \
("second", Unit::SECOND) ("watt", Unit::WATT) \
("siemens", Unit::SIEMENS) ("weber", Unit::WEBER) \
("sievert", Unit::SIEVERT) ("steradian", Unit::STERADIAN) \
("tesla", Unit::TESLA) ("volt", Unit::VOLT);

