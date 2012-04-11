#ifndef __FLUC_MODELS_INTENSIVESPECIESMIXIN_HH__
#define __FLUC_MODELS_INTENSIVESPECIESMIXIN_HH__

#include "basemodel.hh"

namespace Fluc {
namespace Models {


/**
 * This class ensures, that all species are defined as intensive quantities.
 *
 * @ingroup models
 */
class IntensiveSpeciesMixin
{
public:
  /**
   * Constructor.
   *
   * Iterates through species definitions and ensures, that species are defined to have intensive
   * units.
   */
  IntensiveSpeciesMixin(BaseModel &base);
};


}
}

#endif // EXTENSIVESPECIESMIXIN_HH
