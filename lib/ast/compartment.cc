#include "compartment.hh"
#include "exception.hh"

using namespace Fluc;
using namespace Fluc::Ast;


Compartment::Compartment(const std::string &id, const Unit &unit, SpatialDimension dim, bool is_const)
  : VariableDefinition(Node::COMPARTMENT_DEFINITION, id, unit, is_const), dimension(dim)
{
  /// \todo Check if unit fits to spacial dimension.
  if (Compartment::POINT == dim && ! unit.isDimensionless())
  {
    SemanticError err;
    err << "Can not construct compartment " << id
        << ": Spatial dimension is 0 but unit is ";
    unit.dump(err);
    err << ", which is not dimensionless.";
    throw err;
  }
}

Compartment::Compartment(const std::string &id, const GiNaC::ex &init_val, const Unit &unit,
                         SpatialDimension dim, bool is_const)
  : VariableDefinition(Node::COMPARTMENT_DEFINITION, id, init_val, unit, "", is_const),
    dimension(dim)
{
  /// \todo Check if unit fits to spacial dimension.
  if (Compartment::POINT == dim && ! unit.isDimensionless())
  {
    SemanticError err;
    err << "Can not construct compartment " << id
        << ": Spatial dimension is 0 but unit is ";
    unit.dump(err);
    err << ", which is not dimensionless.";
    throw err;
  }
}

Compartment::SpatialDimension
Compartment::getDimension() const
{
  return this->dimension;
}
