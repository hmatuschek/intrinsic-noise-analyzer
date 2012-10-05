#ifndef BASEUNIT_HH
#define BASEUNIT_HH

#include "basemodel.hh"

namespace iNA {
namespace Models {

/**
 * Defines the units of the model.
 *
 * This Mixin defines a consistent conversion between substance and concentration units.
 * @todo define the AVOGADRO constant self-consistent within this module!!!
 *
 * @ingroup models
 */
class BaseUnitMixin
{
public:
  /**
   * Constructor
   */
  BaseUnitMixin(BaseModel &base);
};

}
}


#endif // BASEUNIT_HH
