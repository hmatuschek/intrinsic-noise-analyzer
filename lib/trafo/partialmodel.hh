#ifndef __INA_TRAFO_PARTIALMODEL_HH__
#define __INA_TRAFO_PARTIALMODEL_HH__

namespace Fluc {
namespace Trafo {


/** This class manages a permutation and link matrix, such that all species defined in a model
 * are separated into a set of independent species and a set of (dependent) species that
 * are defined as linear combinations of the independent species (the link matrix).
 * @ingroup trafo */
class PartialModel
{
public:
  PartialModel();
};


}
}

#endif
