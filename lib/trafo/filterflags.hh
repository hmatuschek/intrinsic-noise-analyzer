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
    NON_CONST    = 8,  ///< Handle non constant variables.
    ALL_CONST    = SPECIES | PARAMETERS | COMPARTMENTS, ///< Shortcut to handle all constant variables.
    ALL          = ALL_CONST | NON_CONST ///< Shortcut to handle all valiables (also non constant ones).
  } VariableFlags;
};


}
}
#endif // __INA_TRAFO_FILTERFLAGS_HH__
