#ifndef __INA_TRAFO_REVERSIBLEREACTIONCONVERTER_HH__
#define __INA_TRAFO_REVERSIBLEREACTIONCONVERTER_HH__

#include "ast/ast.hh"

namespace Fluc {
namespace Trafo {


/**
 * This class implements a simple algorithm to create two irreversible reactions from a single
 * reversible one.
 * @ingroup trafo
 */
class ReversibleReactionConverter
{
public:
  /** Performs the conversion on the given model. */
  static void apply(Ast::Model &model);
};


}
}

#endif // __INA_TRAFO_REVERSIBLEREACTIONCONVERTER_HH__