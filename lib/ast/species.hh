#ifndef __FLUC_AST_SPECIESDEFINITION_HH__
#define __FLUC_AST_SPECIESDEFINITION_HH__

#include "variabledefinition.hh"
#include "compartment.hh"


namespace iNA {
namespace Ast {

/**
 * A specialization of @c Ast::VariableDefinition that represents a species definition.
 *
 * This class holds some more incustommation about the species definition than the
 * @c Ast::VariableDefinition. It links the variable to the compartment, the species live in as well
 * as the unit-definition, the species is measured in.
 *
 * @ingroup ast
 */
class Species : public VariableDefinition
{
public:
  /** Visitor class custom species. */
  class Visitor { public: virtual void visit(const Species *var) = 0; };
  /** Operator class custom species. */
  class Operator { public: virtual void act(Species *var) = 0; };

protected:
  /** Holds a weak reference to the compartment, the species lives in. */
  Compartment *compartment;

public:
  /**
   * Minimal constructor custom a species definition.
   *
   * @param id Specifies the unique identifier custom the species.
   * @param compartment Specifies the compartment, the species lives in.
   * @param is_const Specifies if the species amount
   */
  Species(const std::string &id, Compartment *compartment, bool _is_const=false);

  /**
   * Full constructor custom a species definition.
   *
   * @param id Specifies the unique identifier custom the species.
   * @param init_val Specifies the initial value of the species.
   * @param compartment Specifies the compartment, the species lives in.
   * @param name Specifies display name of the species.
   * @param is_const Specifies if the species amount
   */
  Species(const std::string &id, const GiNaC::ex &init_val,
          Compartment *compartment, const std::string &_name, bool _is_const);

  /** Returns the compartment, the species lives in. */
  Compartment *getCompartment();

  /** Returns a const reference to the compartment. */
  const Compartment *getCompartment() const;

  /** (Re-) Sets the compartment. */
  void setCompartment(Compartment *compartment);

  /** Handles a visitor custom the species. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator on the species. */
  virtual void apply(Ast::Operator &op);
};


}
}

#endif // SPECIESDEFINITION_HH
