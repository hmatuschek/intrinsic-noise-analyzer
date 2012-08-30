#ifndef __INA_TRAFO_FILTERFLAGS_HH__
#define __INA_TRAFO_FILTERFLAGS_HH__

namespace iNA {
namespace Trafo {


/** A namespace for filter flags.
 * @ingroup trafo */
class Filter {
public:
  /** Enumerates the filter flags for variables. */
  typedef enum {
    NONE         = 0,  ///< Filter all elements, process none.
    SPECIES      = 1,  ///< Handle species.
    PARAMETERS   = 2,  ///< Handle parameters.
    COMPARTMENTS = 4,  ///< Handle compartments.
    ALL          = SPECIES | PARAMETERS | COMPARTMENTS ///< Shortcut to handle all variables.
  } VariableFlags;
};


}
}
#endif // __INA_TRAFO_FILTERFLAGS_HH__
