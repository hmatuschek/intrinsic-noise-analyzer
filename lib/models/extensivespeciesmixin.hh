#ifndef __FLUC_MODELS_EXTENSIVESPECIESMIXIN_HH__
#define __FLUC_MODELS_EXTENSIVESPECIESMIXIN_HH__

#include "baseunitmixin.hh"

namespace Fluc {
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
