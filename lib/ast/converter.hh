#ifndef __FLUC_MODELS_CONVERTTOIRREVERSIBLEREACTIONS_HH__
#define __FLUC_MODELS_CONVERTTOIRREVERSIBLEREACTIONS_HH__

#include "ast/ast.hh"

namespace iNA {
namespace Ast {


/**
 * Checks if the model is not defined as reversible.
 *
 * @ingroup models
 */
class Convert2Irreversible
{

 Model &model;

public:
  /**
   * Constructor, converts to irreversible reactions.
   */
  Convert2Irreversible(Ast::Model &model);

  void process();

};


}
}

#endif // ASSERTIRREVERSIBLEREACTIONS_HH
