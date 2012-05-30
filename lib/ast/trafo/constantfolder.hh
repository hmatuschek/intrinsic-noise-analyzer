#ifndef __FLUC_COMPILER_AST_TRAFO_CONSTANTFOLDER_HH__
#define __FLUC_COMPILER_AST_TRAFO_CONSTANTFOLDER_HH__

#include "pass.hh"


namespace Fluc {
namespace Ast {
namespace Trafo {


/**
 * This compiler pass substitues all constant variable references with their value (if they have
 * one).
 *
 * @ingroup ast
 */
class ConstantSubstitution : public Pass
{
public:
  /**
   * Constructs a constant substitution pass with an empty substitution table. The substitution
   * table is then populated with substitutions when processing a @c Ast::Scope like @c Ast::Module
   * or @c Ast::KineticLaw with all constant variable definitions defined in the scope.
   */
  ConstantSubstitution();

  /**
   * Constructs a constant substitution pass with an initial set of substitutions. Anyway, if the
   * pass processes a @c Ast::Scope, the substitution table will be populated with substitions for
   * constant variables defined in that scope.
   */
  ConstantSubstitution(const GiNaC::exmap &table);

  /**
   * This callback will populate the substitution table with all constant variables (parameters).
   */
  virtual void handleScope(Ast::Scope *node);
};


}
}
}



#endif // CONSTANTFOLDER_HH
