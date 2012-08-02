#ifndef __FLUC_AST_MODEL_HH__
#define __FLUC_AST_MODEL_HH__

#include <sbml/SBMLTypes.h>

#include "module.hh"
#include "species.hh"
#include "compartment.hh"
#include "parameter.hh"


namespace Fluc {
namespace Ast {


/**
 * Extends the @c Ast::Module class to handle species, compartments and parameters. Within a
 * model, all the definitions have the same order as they are added to the model. While on module
 * level, there is no ordering of the definitions. This allows access of definitions by index and
 * eases the iteration over specific definitions like species.
 *
 * @ingroup ast;
 */
class Model : public Module
{
protected:
  /**
   * Holds a vector of weak-references to all compartments defined in the model, in order of
   * definition.
   */
  std::vector<Compartment *> compartment_vector;

  /**
   * Holds a vector of weak-references to all species defined in the model, in oder of their
   * definition.
   */
  std::vector<Species *> species_vector;

  /**
   * Holds a vector of weak-references to all (global) parameters in the model, in order of their
   * definition.
   */
  std::vector<Parameter *> parameter_vector;

  /**
   * Holds a vector of weak-references to all reactions in the model, in order of their definition.
   */
  std::vector<Reaction *> reaction_vector;


public:
  /**
   * Constructs an empty model, use @c addDefinition to populate the model with definitions
   * (species, reactions, ...)
   */
  Model();

  /* Model(libsbml::Model *model); */

  /**
   * Recursive copy-constructor.
   */
  explicit Model(const Model &other);

  /**
   * Returns the number of species defined in the model.
   */
  size_t numSpecies() const;

  /**
   * Returns true, if the given species (id) is defined in the model.
   *
   * Is equivalent to call hasSpecies(getSymbol(const std::string &id)).
   */
  bool hasSpecies(const std::string &id) const;

  /**
   * Returns true, if the given symbol is associated with a species definition.
   */
  bool hasSpecies(const GiNaC::symbol &symbol) const;

  /**
   * Retunrs the species with the given identifer.
   *
   * Is equivalent to call @c getSpecies(getSymbol(const std::string &id)).
   *
   * @throws SymbolError If there is no species associated with the identifier. To check if there
   *         is a species defined with the given identifier, use the @c hasSpecies method.
   */
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

  /**
   * Returns the specified compartment.
   *
   * @throws SymbolError If the compartment can not be found.
   */
  Compartment * const getCompartment(const std::string &id) const;

  /**
   * Returns the compartment associated with the given symbol.
   *
   * @throws SymbolError If there is no compartment with the given symbol.
   */
  Compartment *getCompartment(const GiNaC::symbol &symbol);

  /**
   * Returns the compartment associated with the given symbol.
   *
   * @throws SymbolError If there is no compartment with the given symbol.
   */
  Compartment * const getCompartment(const GiNaC::symbol &symbol) const;

  /**
   * Returns the specifies compartment (by index).
   */
  Compartment *getCompartment(size_t idx);

  /**
   * Returns the specifies compartment (by index).
   */
  Compartment * const getCompartment(size_t idx) const;

  /**
   * Retunrs the index of the specifies compartment, throws an exception if the compartment can
   * not be found.
   */
  size_t getCompartmentIdx(const std::string &id) const;

  /**
   * Returns the index for the given compartment.
   *
   * @throws SymbolError If the symbol does not name a compartment.
   */
  size_t getCompartmentIdx(const GiNaC::symbol &symbol) const;

  /**
   * Returns the index of the given compartment.
   */
  size_t getCompartmentIdx(Compartment *compartment) const;

  /**
   * Returns the number of parameters defined in the model.
   */
  size_t numParameters() const;

  /**
   * Returns true, if the given id specifies a parameter.
   */
  bool hasParameter(const std::string &id) const;

  /**
   * Returns true, if the given symbol specifies a parameter.
   */
  bool hasParameter(const GiNaC::symbol &symbol) const;

  /**
   * Returns the parameter by its identifier, throws an exception if the parameter can not
   * be found.
   */
  Parameter *getParameter(const std::string &id);

  /**
   * Returns the parameter by its identifier, throws an exception if the parameter can not
   * be found.
   */
  Parameter * const getParameter(const std::string &id) const;

  /**
   * Returns the parameter by its symbol.
   *
   * @throws SymbolError If the symbol is not associated with a parameter.
   */
  Parameter *getParameter(const GiNaC::symbol &symbol);

  /**
   * Returns the parameter by its symbol.
   *
   * @throws SymbolError If the symbol is not associated with a parameter.
   */
  Parameter * const getParameter(const GiNaC::symbol &symbol) const;

  /**
   * Returns the i-th parameter definition.
   */
  Parameter *getParameter(size_t idx);

  /**
   * Returns the i-th parameter definition.
   */
  Parameter * const getParameter(size_t idx) const;

  /**
   * Returns the index of the given parameter, throws an exception if the parameter can not be
   * found.
   */
  size_t getParameterIdx(const std::string &id) const;

  /**
   * Returns the index of the parameter specified by symbol.
   */
  size_t getParameterIdx(const GiNaC::symbol &symbol) const;

  /**
   * Returns the index of the given parameter, throws an exception if the parameter can not be
   * found.
   */
  size_t getParameterIdx(Parameter *parameter) const;

  /**
   * Returns the number of reactions defined in the model.
   */
  size_t numReactions() const;

  /**
   * Returns true if the given identifier specifies a reaction.
   */
  bool hasReaction(const std::string &id) const;

  /**
   * Returns the reaction by index.
   */
  Reaction *getReaction(size_t idx);

  /**
   * Returns the reaction by index.
   */
  Reaction * const getReaction(size_t idx) const;

  /**
   * Returns the index of the given reaction, throws an exception if the reaction can not be
   * found.
   */
  size_t getReactionIdx(const std::string &id) const;

  /**
   * Returns the index for the given reaction.
   */
  size_t getReactionIdx(Reaction *reac) const;

  /**
   * Overrides the @c Scope::addDefinition method defined in @c Scope and ensures, that
   * the definition is stored in the corresponding vector.
   */
  virtual void addDefinition(Definition *def);

  /**
   * Removes a definition (Species, Compartment, Reaction, ...) from the model. The user is
   * responsible to ensure, that no other element refers to this definition on removal
   * (also its symbols).
   */
  virtual void remDefinition(Definition *def);
};


}
}

#endif // MODEL_HH
