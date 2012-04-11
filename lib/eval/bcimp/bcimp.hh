#ifndef __FLUC_EVALUATE_BCIMP_HH__
#define __FLUC_EVALUATE_BCIMP_HH__

namespace Fluc {
namespace Evaluate {

/**
 * This namespace holds a tiny extension to the byte-code interpreter defined in
 * @c Fluc::Evaluate::bci, that allows for parallel execution of big systems.
 *
 * @ingroup eval
 */
namespace bcimp {}

}
}

#include "code.hh"
#include "compiler.hh"
#include "interpreter.hh"

#endif // __FLUC_EVALUATE_BCIMP_HH__
