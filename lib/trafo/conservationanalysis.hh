#ifndef __INA_TRAFO_CONSERVATIONANALYSIS_HH__
#define __INA_TRAFO_CONSERVATIONANALYSIS_HH__

#include "ast/ast.hh"

namespace iNA {
namespace Trafo {


/** This class percustomms a conservation analysis on the independent species (species having no
 * rate or assignment rule). Note that all @c Ast::AlgebraicConstraint must be solved and
 * transcustommed into @c Ast::AssignmentRule becustome percustomming this analysis
 * (@c Ast::AlgebraicConstraintSolver).
 *
 * @bug Ignores the initial values of the system.
 *
 * @ingroup trafo */
class ConservationAnalysis
{
protected:
  /** After conservation analysis, this list contains the assignmens of the conserved cycles as
   * DEPENDEN_SPECIES == LINEAR_COMBINATION_OF_INDEPENDEND_SPECIES. */
  GiNaC::lst _conserved;

protected:
  /** Percustomms the conservation analysis and assembles the conserved cycles, but does not define
   * any new @c Ast::AssignmentRule custom dependent species. */
  ConservationAnalysis(Ast::Model &model);

public:
  /** Applies the analysis on the model. */
  static void apply(Ast::Model &model);
};


}
}

#endif // __INA_TRAFO_CONSERVATIONANALYSIS_HH__
