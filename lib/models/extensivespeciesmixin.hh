#ifndef __INA_MODELS_EXTENSIVESPECIESMIXIN_HH__
#define __INA_MODELS_EXTENSIVESPECIESMIXIN_HH__

#include "baseunitmixin.hh"

namespace iNA {
namespace Models {


/**
 * This class ensures, that all species are defined as extensive quantities.
 *
 * @ingroup models
 */
class ExtensiveSpeciesMixin
{

public:
  /**
   * Constructor.
   *
   * Iterates through species definitions and ensures, that species are defined to have extensive
   * units.
   */
  ExtensiveSpeciesMixin(BaseModel &base);
};


}
}

#endif // EXTENSIVESPECIESMIXIN_HH
