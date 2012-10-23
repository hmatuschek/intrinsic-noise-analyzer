#ifndef __FLUC_AST_COMPARTMENT_HH__
#define __FLUC_AST_COMPARTMENT_HH__

#include "variabledefinition.hh"

namespace iNA {
namespace Ast {


/**
 * Represents a compartment, that may hold @c Ast::Species. "Holding" a species does not imply any
 * ownership but defines a semantic relationship between species and compartments.
 *
 * @ingroup ast
 */
class Compartment : public VariableDefinition
{
public:
  /** Defines the possible spacial dimensions of a compartment. */
  typedef enum {
    POINT,         /// < dimension 0
    LINE,          /// < dimension 1
    AREA,          /// < dimension 2
    VOLUME         /// < dimension 3
  } SpatialDimension;

  /** Visitor class for compartments. */
  class Visitor { public: virtual void visit(const Compartment *comp) = 0; };

  /** Operator class for compartments. */
  class Operator { public: virtual void act(Compartment *comp) = 0; };

protected:
  /** Holds the spatial dimension of the compartment. */
  SpatialDimension dimension;

public:
  /**
   * Minimal constructor for a compartment.
   *
   * @param id Specifies the unique identifier of the compartment.
   * @param dim Specifies the spacial dimension of the compartment.
   * @param is_const Specifies if the compartment size is constant or not.
   */
  Compartment(const std::string &id, SpatialDimension dim, bool _is_const=false);

  /**
   * Constructor for a compartment.
   *
   * @param id Specifies the unique identifier of the compartment.
   * @param init_val Specifies the initial volume of the compartment.
   * @param dim Specifies the spatial dimension of the compartment.
   * @param is_const Specifies if the compartment size is constant or not.
   */
  Compartment(const std::string &id, const GiNaC::ex &init_val,
              SpatialDimension dim, bool _is_const=false);

  /** Returns the spacial dimension of the compartment. */
  SpatialDimension getDimension() const;

  /** Handles a visitor for the compartment. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator on the compartment. */
  virtual void apply(Ast::Operator &op);

};


}
}

#endif // COMPARTMENT_HH
