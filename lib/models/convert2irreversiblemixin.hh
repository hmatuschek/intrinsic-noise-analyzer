#ifndef __FLUC_MODELS_CONVERTTOIRREVERSIBLEREACTIONS_HH__
#define __FLUC_MODELS_CONVERTTOIRREVERSIBLEREACTIONS_HH__

#include "ast/ast.hh"

namespace Fluc {
namespace Models {


/**
 * Checks if the model is not defined as reversible.
 *
 * @ingroup models
 */
class Convert2IrreversibleMixin
{
public:
  /**
   * Constructor, converts to irreversible reactions.
   */
  Convert2IrreversibleMixin(Ast::Model &model);

};


}
}

#endif // ASSERTIRREVERSIBLEREACTIONS_HH
