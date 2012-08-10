#ifndef __FLUC_AST_SPECIESDEFINITION_HH__
#define __FLUC_AST_SPECIESDEFINITION_HH__

#include "variabledefinition.hh"
#include "compartment.hh"


namespace Fluc {
namespace Ast {

/**
 * A specialization of @c Ast::VariableDefinition that represents a species definition.
 *
 * This class holds some more information about the species definition than the
 * @c Ast::VariableDefinition. It links the variable to the compartment, the species live in as well
 * as the unit-definition, the species is measured in.
 *
 * @ingroup ast
 */
class Species : public VariableDefinition
{
public:
  /** Visitor class for species. */
  class Visitor { public: virtual void visit(const Species *var) = 0; };
  /** Operator class for species. */
  class Operator { public: virtual void act(Species *var) = 0; };

protected:
  /** Holds a weak reference to the compartment, the species lives in. */
  Compartment *compartment;

  /** Boolean which declares substrate units.
   * @todo Is this flag actually needed? We handle that with units aren't we?
   */
  bool substance_units;


public:
  /**
   * Minimal constructor for a species definition.
   *
   * @param id Specifies the unique identifier for the species.
   * @param unit Specifies the unit, the species is measured in (a substance or density unit).
   * @param compartment Specifies the compartment, the species lives in.
   * @param is_const Specifies if the species amount
   */
  Species(const std::string &id, const Unit &unit, Compartment *compartment, bool is_const=false);

  /**
   * Full constructor for a species definition.
   *
   * @param id Specifies the unique identifier for the species.
   * @param init_val Specifies the initial value of the species.
   * @param unit Specifies the unit, the species is measured in (a substance or density unit).
   * @param compartment Specifies the compartment, the species lives in.
   * @param name Specifies display name of the species.
   * @param is_const Specifies if the species amount
   */
  Species(const std::string &id, const GiNaC::ex &init_val, const Unit &unit, bool only_substance_units,
          Compartment *compartment, const std::string &name, bool is_const);

  /** Returns the compartment, the species lives in. */
  Compartment *getCompartment();

  /** Returns a const reference to the compartment. */
  const Compartment *getCompartment() const;

  /** (Re-) Sets the compartment. */
  void setCompartment(Compartment *compartment);

  /** Returns whether the species has been defined in units of substance. */
  bool hasOnlySubstanceUnits();

  /** Handles a visitor for the species. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator on the species. */
  virtual void apply(Ast::Operator &op);
};


}
}

#endif // SPECIESDEFINITION_HH
