#ifndef __INA_TRAFO_EXPRVERIFICATION_HH__
#define __INA_TRAFO_EXPRVERIFICATION_HH__

#include "../ast/scope.hh"

namespace iNA {
namespace Trafo {

/** Checks if the given expression @c expr is well defined within the given scope. This means that
 * all referenced symbols within the expression are defined as variabled within the given scope. */
bool verifyExpr(const GiNaC::ex &expr, Ast::Scope *scope);

}
}


#endif // __INA_TRAFO_EXPRVERIFICATION_HH__
