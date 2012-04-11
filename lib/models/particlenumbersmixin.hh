#ifndef __FLUC_MODELS_PARTICLENUMBERSMIXIN_HH
#define __FLUC_MODELS_PARTICLENUMBERSMIXIN_HH

#include "constcompartmentmixin.hh"
#include "extensivespeciesmixin.hh"

namespace Fluc {
namespace Models {

/**
 * Mixin to convert mol units to particle numbers.
 *
 * @ingroup models
 */
class ParticleNumbersMixin :
    public ConstCompartmentMixin,
    public ExtensiveSpeciesMixin
{
public:
  /**
   * Constructor.
   */
  ParticleNumbersMixin(BaseModel &base);
};

}
}


#endif // __FLUC_MODELS_PARTICLENUMBERSMIXIN_HH
