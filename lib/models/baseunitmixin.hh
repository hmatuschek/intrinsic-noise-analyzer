#ifndef BASEUNIT_HH
#define BASEUNIT_HH

#include "basemodel.hh"

namespace iNA {
namespace Models {

/**
 * Defines the units of the model.
 *
 * This Mixin defines a consistent conversion between substance and concentration units.
 * @ todo define the AVOGADRO constant self-consistent within this module!!!
 *
 * @ingroup models
 */
class BaseUnitMixin
{
private:
  /**
   * Holds the substance unit of the model.
   */
  Ast::Unit substanceUnit;

  /**
   * Holds the concentration unit of the model.
   */
  Ast::Unit concentrationUnit;

  /**
   * Holds the compartment unit of the model.
   */
  Ast::Unit compartmentUnit;

  /**
   * Holds the time unit of the model.
   */
  Ast::Unit timeUnit;

public:
  /**
   * Constructor
   */
  BaseUnitMixin(BaseModel &base);

  /**
   * Returns the substance unit of the model.
   */
  const Ast::Unit &getSubstanceUnit() const;

  /**
   * Returns the concentration unit of the model.
   */
  const Ast::Unit &getConcentrationUnit() const;

  /**
   * Returns the compartment unit of the model.
   */
  const Ast::Unit &getCompartmentUnit() const;

  /**
   * Returns the time unit of the model.
   */
  const Ast::Unit &getTimeUnit() const;

  /**
   * Returns multiplier of substance unit.
   */
  double getMultiplier();

};

}
}


#endif // BASEUNIT_HH
