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
Unit::Unit(const std::map<ScaledBaseUnit::BaseUnit, int> &units,
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


Unit::Unit(const std::list<ScaledBaseUnit> &units)
  : _common_multiplier(1.0), _common_scale(0), _units()
{
  std::list<ScaledBaseUnit>::const_iterator iter = units.begin();
  for (; iter != units.end(); iter++) {
    _common_multiplier *= std::pow(iter->getMultiplier(), iter->getExponent());
    _common_scale      += iter->getScale() * iter->getExponent();

    // Skip DIMENSIONLESS
    if (ScaledBaseUnit::DIMENSIONLESS == iter->getBaseUnit()) {
      continue;
    }

    // Check if there is already this base-unit in units:
    std::map<ScaledBaseUnit::BaseUnit, int>::iterator item;
    if (this->_units.end() != (item = this->_units.find(iter->getBaseUnit()))) {
      this->_units[iter->getBaseUnit()] = this->_units[iter->getBaseUnit()] + iter->getExponent();
      if (0 == this->_units[iter->getBaseUnit()]) {
        this->_units.erase(iter->getBaseUnit());
      }
    } else {
      this->_units[iter->getBaseUnit()] = iter->getExponent();
    }
  }
}

Unit::Unit(const ScaledBaseUnit &base)
  : _common_multiplier(1.0), _common_scale(0), _units()
{
  this->_common_multiplier *= std::pow(base.getMultiplier(), base.getExponent());
  this->_common_scale      += base.getScale() * base.getExponent();

  if (ScaledBaseUnit::DIMENSIONLESS != base.getBaseUnit()) {
    this->_units[base.getBaseUnit()] = base.getExponent();
  }
}

Unit::Unit(ScaledBaseUnit::BaseUnit unit, double multiplier, int scale, int exponent)
  : _common_multiplier(1.0), _common_scale(0), _units()
{
  _common_multiplier *= std::pow(multiplier, exponent);
  _common_scale      += scale * exponent;

  if (ScaledBaseUnit::DIMENSIONLESS != unit) {
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
Unit::isVariantOf(ScaledBaseUnit::BaseUnit baseUnit, int expo) const {
  // A unit is dimension-less if there are no units:
  if (ScaledBaseUnit::DIMENSIONLESS == baseUnit && 0 == this->_units.size()) {
    return true;
  }

  // If the unit consists of more than one base-unit:
  if (1 != this->_units.size()) {
    return false;
  }

  return this->hasVariantOf(baseUnit, expo);
}


bool
Unit::hasVariantOf(ScaledBaseUnit::BaseUnit baseUnit, int expo) const {
  // Any unit has a dimensionless factor!
  if (ScaledBaseUnit::DIMENSIONLESS == baseUnit) {
    return true;
  }

  // Seach for base unti
  std::map<ScaledBaseUnit::BaseUnit, int>::const_iterator item = this->_units.find(baseUnit);
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
  return (hasVariantOf(Ast::ScaledBaseUnit::MOLE) || hasVariantOf(Ast::ScaledBaseUnit::ITEM) ||
          hasVariantOf(Ast::ScaledBaseUnit::GRAM) || hasVariantOf(Ast::ScaledBaseUnit::KILOGRAM));
}


bool
Unit::isConcentrationUnit() const {
  if (this->isVariantOf(Ast::ScaledBaseUnit::DIMENSIONLESS, -1) ||
      this->isVariantOf(Ast::ScaledBaseUnit::LITRE, -1) ||
      this->isVariantOf(Ast::ScaledBaseUnit::METRE, -3)) {
    return false;
  }

  if (2 != this->_units.size()) {
    return false;
  }

  return this->hasSubstanceUnit() && (this->hasVariantOf(Ast::ScaledBaseUnit::DIMENSIONLESS, -1) ||
                                      this->hasVariantOf(Ast::ScaledBaseUnit::LITRE, -1) ||
                                      this->hasVariantOf(Ast::ScaledBaseUnit::METRE, -3));
}


bool
Unit::isVolumeUnit() const {
  return isVariantOf(Ast::ScaledBaseUnit::LITRE, 1) ||
      isVariantOf(Ast::ScaledBaseUnit::METRE, 3) ||
      isVariantOf(Ast::ScaledBaseUnit::DIMENSIONLESS);
}

bool
Unit::isAreaUnit() const {
  return isVariantOf(Ast::ScaledBaseUnit::METRE, 2) ||
      isVariantOf(Ast::ScaledBaseUnit::DIMENSIONLESS);
}

bool
Unit::isLengthUnit() const {
  return isVariantOf(Ast::ScaledBaseUnit::METRE, 1) ||
      isVariantOf(Ast::ScaledBaseUnit::DIMENSIONLESS);
}

bool
Unit::isTimeUnit() const {
  return isVariantOf(Ast::ScaledBaseUnit::SECOND, 1) ||
      isVariantOf(Ast::ScaledBaseUnit::DIMENSIONLESS);
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


ScaledBaseUnit
Unit::asScaledBaseUnit() const {
  // Handle dimensionless unit explicitly:
  if (this->isDimensionless()) {
    return ScaledBaseUnit(ScaledBaseUnit::DIMENSIONLESS, 1, 0, 1);
  }

  if (1 != this->_units.size()) {
    InternalError err;
    err << "Cannot construct a scaled base-unit from non-atomic unit: "
        << this->dump();
    throw err;
  }

  ScaledBaseUnit::BaseUnit base_unit = this->_units.begin()->first;
  int    exponent   = this->_units.begin()->second;
  int    scale      = int(this->_common_scale/exponent);
  double multiplier = std::exp(std::log(this->_common_multiplier)/exponent);;
  return ScaledBaseUnit(base_unit, multiplier, scale, exponent);
}

void
Unit::asScaledBaseUnit(ScaledBaseUnit::BaseUnit &unit, double &multiplier, int &scale, int &exponent) const {
  // Handle dimensionless unit explicitly:
  if (this->isDimensionless()) {
    unit = ScaledBaseUnit::DIMENSIONLESS; multiplier = 1; scale = 0; exponent = 1; return;
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

      std::map<ScaledBaseUnit::BaseUnit, int>::const_iterator iter = this->_units.begin();
      for (size_t i=0; i<this->_units.size()-1; i++, iter++)
      {
        str << ScaledBaseUnit::baseUnitRepr(iter->first);
        if (1 != iter->second)
          str << "^"<<iter->second;
        str << " * ";
      }

      str << ScaledBaseUnit::baseUnitRepr(iter->first);
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
  if (isVariantOf(ScaledBaseUnit::SECOND))
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

    std::map<ScaledBaseUnit::BaseUnit, int>::const_iterator iter = this->_units.begin();
    for (size_t i=0; i<this->_units.size(); i++, iter++)
    {
      if (0 < iter->second)
      {
        numerator << ScaledBaseUnit::baseUnitRepr(iter->first);
        if (iter->second!=1)
          numerator << "^{"<<(iter->second)<<"}";
        numerator << " ";
        nnum++;
      }
      else if(0 > iter->second)
      {
        denominator << ScaledBaseUnit::baseUnitRepr(iter->first);
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
  for (std::map<ScaledBaseUnit::BaseUnit, int>::const_iterator iter = this->_units.begin();
       iter != this->_units.end(); iter++)
  {
    std::map<ScaledBaseUnit::BaseUnit, int>::const_iterator item = other._units.find(iter->first);
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
  std::map<ScaledBaseUnit::BaseUnit, int> units(this->_units);

  // add scaled units of other:
  for (std::map<ScaledBaseUnit::BaseUnit, int>::const_iterator iter = other._units.begin();
       iter != other._units.end(); iter++)
  {
    std::map<ScaledBaseUnit::BaseUnit, int>::iterator item = units.find(iter->first);

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
  std::map<ScaledBaseUnit::BaseUnit, int> units(this->_units);

  // add scaled units of other:
  for (std::map<ScaledBaseUnit::BaseUnit, int>::const_iterator iter = other._units.begin();
       iter != other._units.end(); iter++)
  {
    std::map<ScaledBaseUnit::BaseUnit, int>::iterator item = units.find(iter->first);

    if (other._units.end() != item)
    {
      units[iter->first] = units[iter->first] - iter->second;
      if (0 == units[iter->first])
      {
        units.erase(iter->first);
      }
    }
    else
    {
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
  std::map<ScaledBaseUnit::BaseUnit, int>::const_iterator my_unit = _units.begin();
  std::map<ScaledBaseUnit::BaseUnit, int>::const_iterator other_unit = other._units.begin();
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
  std::map<ScaledBaseUnit::BaseUnit, int> units;
  std::map<ScaledBaseUnit::BaseUnit, int>::const_iterator item = _units.begin();
  for (; item!=_units.end(); item++) {
    int exp = item->second * exponent;
    if (0 != exp) { units[item->first] = exp; }
  }
  return Unit(units, common_multiplier, common_scale);
}

Unit
Unit::dimensionless(double multiplier, int scale)
{
  return Unit(ScaledBaseUnit(ScaledBaseUnit::DIMENSIONLESS, multiplier, scale, 1));
}




/* ********************************************************************************************* *
 * Implementation of ScaledUnit
 * ********************************************************************************************* */
ScaledBaseUnit::ScaledBaseUnit()
  : unit(DIMENSIONLESS), multiplier(1), scale(0), exponent(1) {
  // Done...
}

ScaledBaseUnit::ScaledBaseUnit(BaseUnit unit, double multiplier, int scale, int exponent)
  : unit(unit), multiplier(multiplier), scale(scale), exponent(exponent) {
  // Done...
}

ScaledBaseUnit::ScaledBaseUnit(BaseUnit unit)
  : unit(unit), multiplier(1), scale(0), exponent(1) {
  // Done...
}

ScaledBaseUnit::ScaledBaseUnit(const ScaledBaseUnit &other)
  : unit(other.unit), multiplier(other.multiplier), scale(other.scale), exponent(other.exponent) {
  // Done...
}


const ScaledBaseUnit &
ScaledBaseUnit::operator =(const ScaledBaseUnit &other)
{
  this->unit = other.unit;
  this->multiplier = other.multiplier;
  this->scale = other.scale;
  this->exponent = other.exponent;

  return *this;
}


double
ScaledBaseUnit::getMultiplier() const
{
  return this->multiplier;
}


int
ScaledBaseUnit::getScale() const
{
  return this->scale;
}


int
ScaledBaseUnit::getExponent() const
{
  return this->exponent;
}


ScaledBaseUnit::BaseUnit
ScaledBaseUnit::getBaseUnit() const
{
  return this->unit;
}


bool
ScaledBaseUnit::isLinScaling() const
{
  return 1 == this->exponent;
}


bool
ScaledBaseUnit::isSubstanceUnit() const
{
  if (! this->isLinScaling())
  {
    return false;
  }

  switch (this->unit)
  {
  case ScaledBaseUnit::MOLE:
  case ScaledBaseUnit::ITEM:
    return true;

    // SBML defines the follwoing units as "substance units": WE DO NOT!
  case ScaledBaseUnit::GRAM:
  case ScaledBaseUnit::KILOGRAM:
  case ScaledBaseUnit::DIMENSIONLESS:
    return false;

  default:
    break;
  }

  return false;
}


bool
ScaledBaseUnit::isVolumeUnit() const
{
  if (this->isLinScaling())
  {
    switch (this->unit)
    {
    case ScaledBaseUnit::LITRE:
      return true;

    // We now allow a dimensionless volume:
    case ScaledBaseUnit::DIMENSIONLESS:
      return true;

    default:
      break;
    }
  }
  else if (3. == this->exponent && ScaledBaseUnit::METRE == this->unit)
  {
    return true;
  }

  return false;
}


bool
ScaledBaseUnit::isAreaUnit() const
{
  if (2. == this->exponent && ScaledBaseUnit::METRE == this->unit) {
    return true;
  }

  return false;
}


bool
ScaledBaseUnit::isLengthUnit() const
{
  if (! this->isLinScaling())
  {
    return false;
  }

  switch (this->unit)
  {
  case ScaledBaseUnit::METRE:
    return true;

  case ScaledBaseUnit::DIMENSIONLESS:
    return false;

  default:
    break;
  }

  return false;
}


bool
ScaledBaseUnit::isTimeUnit() const
{
  if (! this->isLinScaling())
  {
    return false;
  }

  switch (this->unit)
  {
  case ScaledBaseUnit::SECOND:
    return true;

  case ScaledBaseUnit::DIMENSIONLESS:
    return false;

  default:
    break;
  }

  return false;
}


void
ScaledBaseUnit::dump(std::ostream &str) const
{
  str << "(" << this->multiplier << "e" << this->scale << " * "
      << ScaledBaseUnit::baseUnitRepr(this->unit)
      << ")**" << this->exponent;
}


std::string
ScaledBaseUnit::baseUnitRepr(ScaledBaseUnit::BaseUnit base)
{
  switch (base)
  {
  case ScaledBaseUnit::DIMENSIONLESS: return "a.u.";
  case ScaledBaseUnit::MOLE: return "mol";
  case ScaledBaseUnit::ITEM: return "#";
  case ScaledBaseUnit::GRAM: return "g";
  case ScaledBaseUnit::KILOGRAM: return "kg";
  case ScaledBaseUnit::LITRE: return "l";
  case ScaledBaseUnit::METRE: return "m";
  case ScaledBaseUnit::SECOND: return "s";
  default: break;
  }

  return "?";
}

std::string
ScaledBaseUnit::baseUnitName(ScaledBaseUnit::BaseUnit base) {
  return ScaledBaseUnit::_unit_to_name[base];
}

ScaledBaseUnit::BaseUnit
ScaledBaseUnit::baseUnitByName(const std::string &name) {
  return ScaledBaseUnit::_name_to_unit[name];
}

bool
ScaledBaseUnit::isBaseUnitName(const std::string &name) {
  return ScaledBaseUnit::_name_to_unit.end() != ScaledBaseUnit::_name_to_unit.find(name);
}


std::map<ScaledBaseUnit::BaseUnit, std::string> ScaledBaseUnit::_unit_to_name = \
create_map<ScaledBaseUnit::BaseUnit, std::string> (ScaledBaseUnit::AMPERE, "ampere") \
(ScaledBaseUnit::BECQUEREL, "becquerel") (ScaledBaseUnit::CANDELA, "candela") \
(ScaledBaseUnit::COULOMB, "coulomb") (ScaledBaseUnit::DIMENSIONLESS, "dimensionless") \
(ScaledBaseUnit::FARAD, "farad") (ScaledBaseUnit::GRAM, "gram") (ScaledBaseUnit::KATAL, "katal") \
(ScaledBaseUnit::GRAY, "gray") (ScaledBaseUnit::KELVIN, "kelvin") (ScaledBaseUnit::HENRY, "henry") \
(ScaledBaseUnit::KILOGRAM, "kilogram") (ScaledBaseUnit::HERTZ, "herz") \
(ScaledBaseUnit::LITRE, "litre") (ScaledBaseUnit::ITEM, "item") (ScaledBaseUnit::LUMEN, "lumen") \
(ScaledBaseUnit::JOULE, "joule") (ScaledBaseUnit::LUX, "lux") (ScaledBaseUnit::METRE, "metre") \
(ScaledBaseUnit::MOLE, "mole") (ScaledBaseUnit::NEWTON, "newton") (ScaledBaseUnit::OHM, "ohm") \
(ScaledBaseUnit::PASCAL, "pascal") (ScaledBaseUnit::RADIAN, "radian") \
(ScaledBaseUnit::SECOND, "second") (ScaledBaseUnit::WATT, "watt") \
(ScaledBaseUnit::SIEMENS, "siemens") (ScaledBaseUnit::WEBER, "weber") \
(ScaledBaseUnit::SIEVERT, "sievert") (ScaledBaseUnit::STERADIAN, "steradian") \
(ScaledBaseUnit::TESLA, "tesla") (ScaledBaseUnit::VOLT, "volt");


std::map<std::string, ScaledBaseUnit::BaseUnit> ScaledBaseUnit::_name_to_unit = \
create_map<std::string, ScaledBaseUnit::BaseUnit> ("ampere", ScaledBaseUnit::AMPERE) \
("becquerel", ScaledBaseUnit::BECQUEREL) ("candela", ScaledBaseUnit::CANDELA) \
("coulomb", ScaledBaseUnit::COULOMB) ("dimensionless", ScaledBaseUnit::DIMENSIONLESS) \
("farad", ScaledBaseUnit::FARAD) ("gram", ScaledBaseUnit::GRAM) ("katal", ScaledBaseUnit::KATAL) \
("gray", ScaledBaseUnit::GRAY) ("kelvin", ScaledBaseUnit::KELVIN) ("henry", ScaledBaseUnit::HENRY) \
("kilogram", ScaledBaseUnit::KILOGRAM) ("herz", ScaledBaseUnit::HERTZ) \
("litre", ScaledBaseUnit::LITRE) ("item", ScaledBaseUnit::ITEM) ("lumen", ScaledBaseUnit::LUMEN) \
("joule", ScaledBaseUnit::JOULE) ("lux", ScaledBaseUnit::LUX) ("metre", ScaledBaseUnit::METRE) \
("mole", ScaledBaseUnit::MOLE) ("newton", ScaledBaseUnit::NEWTON) ("ohm", ScaledBaseUnit::OHM) \
("pascal", ScaledBaseUnit::PASCAL) ("radian", ScaledBaseUnit::RADIAN) \
("second", ScaledBaseUnit::SECOND) ("watt", ScaledBaseUnit::WATT) \
("siemens", ScaledBaseUnit::SIEMENS) ("weber", ScaledBaseUnit::WEBER) \
("sievert", ScaledBaseUnit::SIEVERT) ("steradian", ScaledBaseUnit::STERADIAN) \
("tesla", ScaledBaseUnit::TESLA) ("volt", ScaledBaseUnit::VOLT);

