#include "parameter.hh"

using namespace Fluc;
using namespace Fluc::Ast;


Parameter::Parameter(const std::string &id, const Unit &unit, bool is_const)
  : VariableDefinition(Node::PARAMETER_DEFINITION, id, unit, is_const)
{
  // Pass...
}


Parameter::Parameter(const std::string &id, const GiNaC::ex &init_val, const Unit &unit, bool is_const)
  : VariableDefinition(Node::PARAMETER_DEFINITION, id, init_val, unit, "", is_const)
{
  // Pass...
}
