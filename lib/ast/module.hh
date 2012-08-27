#ifndef __FLUC_COMPILER_AST_MODULE_HH__
#define __FLUC_COMPILER_AST_MODULE_HH__

#include <map>
#include <list>

#include "scope.hh"
#include "constraint.hh"
#include "unitdefinition.hh"


namespace Fluc {
namespace Ast {


/**
 * The module class collects all "code" to be "compiled" from a SBML model, this includes functions,
 * parameter definitions and kinetic laws.
 *
 * @ingroup ast
 */
class Module: public Scope
{
public:
  /**
   * Defines the iterator type over all global constraints.
   */
  typedef std::list<Constraint *>::iterator constraintIterator;

  /**
   * Defines the const iterator type over all global constraints.
   */
  typedef std::list<Constraint *>::const_iterator const_constraintIterator;


protected:
  /**
   * Holds the optional display-name of the module.
   */
  std::string name;

  /**
   * List of global constaints applied to all variables.
   */
  std::list<Constraint *> constraints;

  /**
   * Holds the module-global unique symbol to represent the time in a explicit time-dependent
   * system.
   */
  GiNaC::symbol time_symbol;

  /**
   * Holds the global default unit for substance.
   */
  Unit substance_unit;

  /**
   * Holds the global default unit for a volume.
   */
  Unit volume_unit;

  /**
   * Holds the global default unit for a area.
   */
  Unit area_unit;

  /**
   * Holds the global default unit for a length.
   */
  Unit length_unit;

  /**
   * Holds teh global default unit for the time.
   */
  Unit time_unit;

  /**
   * Holds a list of pre-defined units.
   */
  std::map<std::string, Unit> predefined_units;


public:
  /**
   * Creates an empty module with the given name.
   */
  Module(const std::string &name = "");

  /**
   * Destructor. Destroies also all definitions held by this module.
   */
  virtual ~Module();

  /**
   * Returns true, if the module has a display-name.
   */
  bool hasName() const;

  /**
   * Returns the display-name of the module.
   */
  const std::string &getName() const;

  /**
   * (Re-) Sets the display-name of the module.
   */
  void setName(const std::string &name);

  /**
   * Addes the given definition to the module and if the definition is a
   * variable, it also updates the symbol table.
   */
  virtual void addDefinition(Definition *def);

  /**
   * Removes the given definition from the module. The ownership of the definition is transferred
   * to the callee. (The user is responsible to destroy the definition.) If the definition is
   * a @c VariableDefinition, the associated symbol is removed from the symbol table.
   */
  virtual void remDefinition(Definition *def);

  /**
   * Retunrs the number of constaints applied to this module.
   */
  size_t numConstraints() const;

  /** Adds a cosntraint to the module. */
  void addConstraint(Constraint *constraint);

  /** Removes a certain constraint from list of constraints.
   * The user is responsible to free the constraint. */
  void remConstraint(Constraint *constraint);

  /**
   * Retunrs a constraintIterator pointing to the first constraint of the module.
   */
  constraintIterator constraintBegin();

  /**
   * Returns a constraintIterator pointing right after the last constraint of the module.
   */
  constraintIterator constraintEnd();

  /**
   * Retunrs a constraintIterator pointing to the first constraint of the module.
   */
  const_constraintIterator constraintBegin() const;

  /**
   * Returns a constraintIterator pointing right after the last constraint of the module.
   */
  const_constraintIterator constraintEnd() const;

  /**
   * Returns the module-global unique time symbol.
   */
  GiNaC::symbol getTime() const;

  /**
   * Retruns the default unit for substance measures.
   */
  const Unit &getDefaultSubstanceUnit() const;

  /**
   * Resets the default unit for substance measures.
   */
  void setDefaultSubstanceUnit(const ScaledBaseUnit &unit);

  /**
   * Returns the default unit for volumes.
   */
  const Unit &getDefaultVolumeUnit() const;

  /**
   * Resets the default unit for volumes.
   */
  void setDefaultVolumeUnit(const ScaledBaseUnit &unit);

  /**
   * Returns the default unit for areas.
   */
  const Unit &getDefaultAreaUnit() const;

  /**
   * Resets the default unit for areas.
   */
  void setDefaultAreaUnit(const ScaledBaseUnit &unit);

  /**
   * Returns the default unit for length measures.
   */
  const Unit &getDefaultLengthUnit() const;

  /**
   * Resets the default unit for length measures.
   */
  void setDefaultLengthUnit(const ScaledBaseUnit &unit);

  /**
   * Returns the default time-unit.
   */
  const Unit &getDefaultTimeUnit() const;

  /**
   * Resets the default time-unit.
   */
  void setDefaultTimeUnit(const ScaledBaseUnit &unit);

  /**
   * Returns the given unit or throws an exception if unknwon.
   */
  const Unit &getUnit(const std::string &name) const;

  /**
   * Returns the unit definition by identifier.
   *
   * Equivalent to call @c getUnitDefinition(getSymbol(const std::string &identifier)).
   *
   * @throws SymbolError If the identifier is not associated with a unit definition.
   */
  UnitDefinition *getUnitDefinition(const std::string &identifier);

  /**
   * Returns the unit definition by identifier.
   *
   * Equivalent to call @c getUnitDefinition(getSymbol(const std::string &identifier)).
   *
   * @throws SymbolError If the identifier is not associated with a unit definition.
   */
  UnitDefinition * const getUnitDefinition(const std::string &identifier) const;

  /**
   * Returns the unit definition by matching the unit.
   *
   * @throws SymbolError If the unit is not associated with a unit definition.
   */
  UnitDefinition * const getUnitDefinition(const Unit &unit) const;

  /**
   * Returns the reaction definition by identifier.
   *
   * Is equivalent to call @c getReaction(getSymbol(const std::string &identifier)).
   *
   * @throws SymbolError If the identifier is not associated with a reaction definition.
   */
  Reaction *getReaction(const std::string &identifier);

  /**
   * Returns the reaction definition by identifier.
   *
   * Is equivalent to call @c getReaction(getSymbol(const std::string &identifier)).
   *
   * @throws SymbolError If the identifier is not associated with a reaction definition.
   */
  Reaction * const getReaction(const std::string &identifier) const;

  /**
   * Returns the function definition by identifier.
   *
   * Is equivalent to call @c getFunction(getSymbol(const std::string &identifier));
   *
   * @throws SymbolError If the identifier is not associated with a function.
   */
  FunctionDefinition *getFunction(const std::string &identifier);

  /**
   * Returns true, if the given expression is explicitly time dependent.
   */
  bool isExplTimeDep(const GiNaC::ex &expression) const;
};


}
}

#endif // ASTMODULE_HH
