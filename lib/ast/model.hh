#ifndef __FLUC_AST_MODEL_HH__
#define __FLUC_AST_MODEL_HH__

#include <sbml/SBMLTypes.h>

#include "scope.hh"
#include "species.hh"
#include "compartment.hh"
#include "parameter.hh"


namespace iNA {
namespace Ast {


/**
 * Extends the @c Ast::Scope class to handle species, compartments and parameters. Within a
 * model, all the definitions have the same order as they are added to the model. While on scope
 * level, there is no ordering of the definitions. This allows access of definitions by index and
 * eases the iteration over specific definitions like species.
 *
 * @ingroup ast;
 */
class Model : public Scope
{
public:
  /** Visitor class for models. */
  class Visitor { public: virtual void visit(const Model *model) = 0; };
  /** Operator class for models. */
  class Operator { public: virtual void act(Model *model) = 0; };

protected:
  /** Holds the model identifier, should be a unique identifier. */
  std::string _identifier;

  /** Holds the model name. This name is used as a display name for the model. */
  std::string _name;

  /** Holds the global unique symbol to represent the time in a explicit time-dependent
   * system. */
  GiNaC::symbol _time_symbol;

  /** If true, the species are measured in substance units
   * (the unit returned by @c getSubstanceUnit). If not, the species are measured in concentration
   * units, the unit given by @c getSubstanceUnit() / @c getVolumeUnit(). */
  bool _species_have_substance_units;

  /** Holds the global default unit for substance. */
  Unit _substance_unit;

  /** Holds the global default unit for a volume. */
  Unit _volume_unit;

  /** Holds the global default unit for a area. */
  Unit _area_unit;

  /** Holds the global default unit for a length. */
  Unit _length_unit;

  /** Holds teh global default unit for the time. */
  Unit _time_unit;

  /** Holds a list of pre-defined units. */
  std::map<std::string, Unit> _predefined_units;

  /** Holds a vector of weak-references to all compartments defined in the model, in order of
   * definition. This vector does not own the compartment instance. */
  std::vector<Compartment *> compartment_vector;

  /** Holds a vector of weak-references to all species defined in the model, in oder of their
   * definition. */
  std::vector<Species *> species_vector;

  /** Holds a vector of weak-references to all (global) parameters in the model, in order of their
   * definition. */
  std::vector<Parameter *> parameter_vector;

  /** Holds a vector of weak-references to all reactions in the model, in order of their definition.
   * This vector does not own the reaction instances. */
  std::vector<Reaction *> reaction_vector;

public:


  /** The compartment iterator. */
  typedef std::vector<Compartment *>::iterator CompartmentIterator;

  /** The parameter iterator. */
  typedef std::vector<Parameter *>::iterator ParameterIterator;

  /** The species iterator. */
  typedef std::vector<Species *>::iterator SpeciesIterator;

  /** The reaction iterator. */
  typedef std::vector<Reaction *>::iterator ReactionIterator;

  /** Returns an iterator pointing on the first compartment of the model. */
  CompartmentIterator compartmentsBegin();

  /** Returns an iterator pointing on somewhere after the last compartment of the model. */
  CompartmentIterator compartmentsEnd();

  /** Returns an iterator pointing on the first parameter of the model. */
  ParameterIterator parametersBegin();

  /** Returns an iterator pointing on somewhere after the last parameter of the model. */
  ParameterIterator parametersEnd();

  /** Returns an iterator pointing on the first species of the model. */
  SpeciesIterator speciesBegin();

  /** Returns an iterator pointing on somewhere after the last species of the model. */
  SpeciesIterator speciesEnd();

  /** Returns an iterator pointing on the first reaction of the model. */
  ReactionIterator reactionsBegin();

  /** Returns an iterator pointing on somewhere after the last reaction of the model. */
  ReactionIterator reactionsEnd();

public:
  /** Constructs an empty model, use @c addDefinition to populate the model with definitions
   * (species, reactions, ...) */
  Model(const std::string &identifier="model", const std::string &name="");

  /** Recursive copy-constructor. The copy constructor creates a deep copy of the given model
   * including new @c GiNaC::symbol instances for variables. */
  explicit Model(const Model &other);

  /** Returns the identifier of the model. */
  const std::string &getIdentifier() const;

  /** (Re-) Sets the identifier of the model. */
  void setIdentifier(const std::string &identifier);

  /** Returns true, if the model has a name. */
  bool hasName() const;

  /** Returns the name of the model or an empty string if there is no name set. */
  const std::string &getName() const;

  /** (Re-) Sets the name of the model. */
  void setName(const std::string &name);

  /** Returns the model-global unique time symbol. */
  GiNaC::symbol getTime() const;

  /** Returns true, if the given GiNaC expression is explicitly time dependent. */
  bool isExplTimeDep(const GiNaC::ex &expression) const;

  /** Returns the model global unique Avogadro constant symbol. The constant is defined as a global
   * constant parameter (id = "_avogadro_const"), the first time the symbol is requested, the
   * parameter is defined. */
  GiNaC::symbol getAvogadro();

  /** Returns true if all species in the model are defined in substance units. In this case,
   * @c getSpeciesUnit will return the substance unit as returned by @c getSubstanceUnit. If false
   * all species are defined in concentration units and @c getSpeciesUnit will return
   * the quotient of the units returned by @c getSubstanceUnit and @c getVolumeUnit. */
  bool speciesHasSubstanceUnits() const;

  /** Resets if the species in the model are defined in substance units, fixes complete model
   * to maintain consistency. */
  void setSpeciesHasSubstanceUnits(bool has_substance_units);

  /** Returns the unit, the species are measured in. The species unit can not be changed directly
   * as it is defined implicitly by the substance and volume unit. */
  Unit getSpeciesUnit() const;

  /** Returns the substance unit of the model. */
  const Unit &getSubstanceUnit() const;

  /** Returns the concentration unit, just the quotient of the units of @c getSubstanceUnit and
   * @c getVolumeUnit. */
  Unit getConcentrationUnit() const;

  /** Resets the default substance unit. A @c UnitError exception is thrown if the given unit is not
   * a valid substance unit. If @c scale_model is true (default), the complete model is scaled to
   * maintain consistency. */
  void setSubstanceUnit(const Unit &unit, bool scale_model=true);

  /** Returns the volume unit of the model. */
  const Unit &getVolumeUnit() const;

  /** Resets the default volume unit. A @c UnitError exception is thrown if the given unit is not
   * a valid volume unit. If @c scale_model is true (default), the complete model is scaled to
   * maintain consistency. */
  void setVolumeUnit(const Unit &unit, bool scale_model=true);

  /** Returns the area unit of the model.
   * @deprecated The area unit is not used anywhere.*/
  const Unit &getAreaUnit() const;

  /** Resets the area unit of the model. A @c UnitError exception is thrown if the given unit is not
   * an area unit. */
  void setAreaUnit(const Unit &unit, bool scale_model=true);

  /** Returns the length unit of the model.
   * @deprecated The length unit is not used anywhere. */
  const Unit &getLengthUnit() const;

  /** Resets the length unit of the model, a @c UnitError exception is thrown if the given unit is
   * not a length unit. */
  void setLengthUnit(const Unit &unit, bool scale_model=true);

  /** Returns the time unit of the model. */
  const Unit &getTimeUnit() const;

  /** Resets the time unit of the model, a @c UnitError exception is thrown if the given unit is
   * not a time unit. */
  void setTimeUnit(const Unit &unit, bool scale_model=true);

  /** Returns the given unit or throws an exception if unknwon. */
  const Unit &getUnit(const std::string &name) const;

  /** Returns the unit definition by identifier.
   * Equivalent to call @c getUnitDefinition(getSymbol(const std::string &identifier)).
   * @throws SymbolError If the identifier is not associated with a unit definition. */
  UnitDefinition *getUnitDefinition(const std::string &identifier);

  /** Returns the unit definition by identifier.
   * Equivalent to call @c getUnitDefinition(getSymbol(const std::string &identifier)).
   * @throws SymbolError If the identifier is not associated with a unit definition. */
  UnitDefinition * const getUnitDefinition(const std::string &identifier) const;

  /** Returns the unit definition by matching the unit.
   * @throws SymbolError If the unit is not associated with a unit definition. */
  UnitDefinition * const getUnitDefinition(const Unit &unit) const;

  /** Returns the number of species defined in the model. */
  size_t numSpecies() const;

  /** Returns true, if the given species (id) is defined in the model.
   * Is equivalent to call hasSpecies(getSymbol(const std::string &id)). */
  bool hasSpecies(const std::string &id) const;

  /** Returns true, if the given symbol is associated with a species definition. */
  bool hasSpecies(const GiNaC::symbol &symbol) const;

  /** Retunrs the species with the given identifer.
   * Is equivalent to call @c getSpecies(getSymbol(const std::string &id)).
   * @throws SymbolError If there is no species associated with the identifier. To check if there
   *         is a species defined with the given identifier, use the @c hasSpecies method. */
  Species *getSpecies(const std::string &id);

  /**
   * Retunrs the species with the given identifer.
   *
   * Is equivalent to call @c getSpecies(getSymbol(const std::string &id)).
   *
   * @throws SymbolError If there is no species associated with the identifier. To check if there
   *         is a species defined with the given identifier, use the @c hasSpecies method.
   */
  Species * const getSpecies(const std::string &id) const;

  /**
   * Retunrs the species with the given symbol.
   *
   * @throws SymbolError If there is no species associated with the symbol. To check if there
   *         is a species defined with the given symbol, use the @c hasSpecies method.
   */
  Species *getSpecies(const GiNaC::symbol &symbol);

  /**
   * Retunrs the species with the given symbol.
   *
   * @throws SymbolError If there is no species associated with the symbol. To check if there
   *         is a species defined with the given symbol, use the @c hasSpecies method.
   */
  Species * const getSpecies(const GiNaC::symbol &symbol) const;

  /**
   * Returns the i-th species in the model.
   */
  Species *getSpecies(size_t idx);

  /**
   * Returns the i-th species in the model.
   */
  Species * const getSpecies(size_t idx) const;

  /**
   * Returns the index of the given species.
   *
   * Is equivalent to call getSpceiesIdx(getSpecies(id));
   *
   * @throws SymbolError If the given identifier does not name a species.
   */
  size_t getSpeciesIdx(const std::string &id)  const;

  /**
   * Returns the index of the given species.
   *
   * Is equivalent to call getSpeciesIdx(getSpecies(symbol)).
   *
   * @throws SymbolError If the given symbol does not name a species.
   */
  size_t getSpeciesIdx(const GiNaC::symbol &symbol) const;

  /**
   * Returns the index of the given species.
   */
  size_t getSpeciesIdx(Species *species) const;

  /**
   * Returns the number of compartments defined in the model.
   */
  size_t numCompartments() const;

  /**
   * Returns true, if the given identifier specifies a compartment.
   */
  bool hasCompartment(const std::string &id) const;

  /**
   * Returns true, if the given symbol specifies a compartment.
   */
  bool hasCompartment(const GiNaC::symbol &symbol) const;

  /**
   * Returns the specified compartment.
   *
   * @throws SymbolError If the compartment can not be found.
   */
  Compartment *getCompartment(const std::string &id);

  /** Returns the specified compartment.
   * @throws SymbolError If the compartment can not be found. */
  Compartment * const getCompartment(const std::string &id) const;

  /** Returns the compartment associated with the given symbol.
   * @throws SymbolError If there is no compartment with the given symbol. */
  Compartment *getCompartment(const GiNaC::symbol &symbol);

  /** Returns the compartment associated with the given symbol.
   * @throws SymbolError If there is no compartment with the given symbol. */
  Compartment * const getCompartment(const GiNaC::symbol &symbol) const;

  /** Returns the specifies compartment (by index). */
  Compartment *getCompartment(size_t idx);

  /** Returns the specifies compartment (by index). */
  Compartment * const getCompartment(size_t idx) const;

  /** Retunrs the index of the specifies compartment, throws an exception if the compartment can
   * not be found. */
  size_t getCompartmentIdx(const std::string &id) const;

  /** Returns the index for the given compartment.
   * @throws SymbolError If the symbol does not name a compartment. */
  size_t getCompartmentIdx(const GiNaC::symbol &symbol) const;

  /** Returns the index of the given compartment. */
  size_t getCompartmentIdx(Compartment *compartment) const;

  /** Returns the number of parameters defined in the model. */
  size_t numParameters() const;

  /** Returns true, if the given id specifies a parameter. */
  bool hasParameter(const std::string &id) const;

  /** Returns true, if the given symbol specifies a parameter. */
  bool hasParameter(const GiNaC::symbol &symbol) const;

  /** Returns the parameter by its identifier, throws an exception if the parameter can not
   * be found. */
  Parameter *getParameter(const std::string &id);

  /** Returns the parameter by its identifier, throws an exception if the parameter can not
   * be found. */
  const Parameter * getParameter(const std::string &id) const;

  /** Returns the parameter by its symbol.
   * @throws SymbolError If the symbol is not associated with a parameter. */
  Parameter *getParameter(const GiNaC::symbol &symbol);

  /** Returns the parameter by its symbol.
   * @throws SymbolError If the symbol is not associated with a parameter. */
  Parameter * const getParameter(const GiNaC::symbol &symbol) const;

  /** Returns the i-th parameter definition. */
  Parameter *getParameter(size_t idx);

  /** Returns the i-th parameter definition. */
  Parameter * const getParameter(size_t idx) const;

  /** Returns the index of the given parameter, throws an exception if the parameter can not be
   * found. */
  size_t getParameterIdx(const std::string &id) const;

  /** Returns the index of the parameter specified by symbol. */
  size_t getParameterIdx(const GiNaC::symbol &symbol) const;

  /** Returns the index of the given parameter, throws an exception if the parameter can not be
   * found. */
  size_t getParameterIdx(const Parameter *parameter) const;

  /** Returns the number of reactions defined in the model. */
  size_t numReactions() const;

  /** Returns true if the given identifier specifies a reaction. */
  bool hasReaction(const std::string &id) const;

  /** Returns the reaction by index. */
  Reaction *getReaction(size_t idx);

  /** Returns the reaction by index. */
  Reaction * const getReaction(size_t idx) const;

  /** Returns the reaction by identifier. */
  Reaction *getReaction(const std::string &id);

  /** Returns the reaction by identifier. */
  Reaction * const getReaction(const std::string &id) const;

  /** Returns the index of the given reaction, throws an exception if the reaction can not be
   * found.
   * @throws SymbolError If the given id does not specify a reaction. */
  size_t getReactionIdx(const std::string &id) const;

  /** Returns the index for the given reaction. */
  size_t getReactionIdx(Reaction *reac) const;


  /** Returns the function definition by identifier.
   * Is equivalent to call @c getFunction(getSymbol(const std::string &identifier));
   * @throws SymbolError If the identifier is not associated with a function. */
  FunctionDefinition *getFunction(const std::string &identifier);


  /** Overrides the @c Scope::addDefinition method defined in @c Scope and ensures, that
   * the definition is stored in the corresponding vector. */
  virtual void addDefinition(Definition *def);

  /**
   * Insert element after other definition.
   */
  virtual void addDefinition(Definition *def, Definition *after);

  /**
   * Removes a definition (Species, Compartment, Reaction, ...) from the model. The user is
   * responsible to ensure, that no other element refers to this definition on removal
   * (also its symbols). */
  virtual void remDefinition(Definition *def);

  /** Handles a visitor for the model. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator on the model. */
  virtual void apply(Ast::Operator &op);
};


}
}

#endif // MODEL_HH
