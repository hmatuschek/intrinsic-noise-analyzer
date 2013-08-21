#include "math.hh"
#include "baseunitmixin.hh"
#include "exception.hh"


using namespace iNA;
using namespace iNA::Models;

BaseUnitMixin::BaseUnitMixin(BaseModel &base)
{
  // Get common multiplier of substance unit:
  double multiplier = base.getSubstanceUnit().getMultiplier();
  multiplier *= std::pow(10., base.getSubstanceUnit().getScale());

  // ensure unit of propensities is item/time:
  for (size_t i=0; i<base.propensities.size(); i++) {
    if (base.getSubstanceUnit().hasVariantOf(Ast::Unit::MOLE)) {
      base.propensities[i] *= multiplier*constants::AVOGADRO;
    } else if (base.getSubstanceUnit().hasVariantOf(Ast::Unit::ITEM)) {
      base.propensities[i] *= multiplier;
    }
  }
}
