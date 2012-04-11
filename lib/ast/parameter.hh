#ifndef __FLUC_AST_PARAMETER_HH__
#define __FLUC_AST_PARAMETER_HH__

#include "variabledefinition.hh"

namespace Fluc {
namespace Ast {


/**
 * Represents a parameter. A parameter is an extension of the @c Ast::VariableDefinition
 * class that has an optional display name.
 *
 * @ingroup ast
 */
class Parameter : public VariableDefinition
{
public:
  /**
   * Minimal constructor.
   *
   * @param id Specifies the unique identifier of the parameter.
   * @param unit Specifies the unit of the paramter.
   * @param is_const Specifies if the parameter is constant.
   */
  Parameter(const std::string &id, const Unit &unit, bool is_const=false);

  /**
   * Constructor.
   *
   * @param id Specifies the unique identifier of the parameter.
   * @param init_val Specifies the initial value of the parameter.
   * @param unit Specifies the unit of the paramter.
   * @param is_const Specifies if the parameter is constant.
   */
  Parameter(const std::string &id, const GiNaC::ex &init_val, const Unit &unit, bool is_const=false);
};


}
}

#endif // PARAMETER_HH
