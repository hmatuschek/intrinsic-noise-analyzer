#ifndef __INA_MODELS_PARTICLENUMBERSMIXIN_HH
#define __INA_MODELS_PARTICLENUMBERSMIXIN_HH

#include "constcompartmentmixin.hh"
#include "extensivespeciesmixin.hh"

namespace iNA {
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
protected:

  Ast::Unit concentrationUnit;

public:
  /**
   * Constructor.
   */
  ParticleNumbersMixin(BaseModel &base);

};

}
}


#endif // __INA_MODELS_PARTICLENUMBERSMIXIN_HH
