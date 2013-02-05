#ifndef __INA_TRAFO_MAKEPARAMGLOBAL_HH__
#define __INA_TRAFO_MAKEPARAMGLOBAL_HH__

#include <ast/model.hh>


namespace iNA {
namespace Trafo {

/** This function moves the selected local paramter of the given reaction into the global scope.
 * This function is needed if a parameter scan over a local parameter is needed.
 * @note The identifier of the paramter may be changed if its identifier is already used in the
 *       global scope.
 * @param param Specifies the local parameter.
 * @param react Specifies the reaction, the paramter is defined in.
 * @param model Specifies the model the reaction is defined in.
 * @throws SymbolError if the parameter or reaction can not be found in the given model.
 * @ingroup trafo */
void makeParameterGlobal(const std::string &param, const std::string &reac, Ast::Model &model);

}
}
#endif // MAKEPARAMGLOBAL_HH
