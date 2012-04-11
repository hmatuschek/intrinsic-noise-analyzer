#ifndef __FLUC_MODELS_LNAMIXIN_HH
#define __FLUC_MODELS_LNAMIXIN_HH


#include "basemodel.hh"
#include "prelnamixin.hh"
#include "conservationanalysismixin.hh"

namespace Fluc {
namespace Models {

/**
 * This mixin only folds the constants in reaction rates at the moment.
 *
 * @ingroup models
 */

class LNAMixin :
    public preLNAMixin,
    public ConservationAnalysisMixin
{

protected:

    /**
    *  \f$ \Omega \f$-vector for independent species
    */
    Eigen::VectorXex Omega_ind;

    /**
    *  \f$ \Omega \f$-vector for dependent species
    */
    Eigen::VectorXex Omega_dep;

public:

  /**
   * Constructor...
   */

  LNAMixin(BaseModel &base);

};


}
}


#endif
