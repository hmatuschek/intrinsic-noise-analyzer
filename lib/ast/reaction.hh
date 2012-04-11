#ifndef __FLUC_COMPILER_AST_REACTION_HH__
#define __FLUC_COMPILER_AST_REACTION_HH__

#include <map>

#include "definition.hh"
#include "parameter.hh"
#include "scope.hh"
#include "species.hh"
#include <ginac/ginac.h>


namespace Fluc {
namespace Ast {


/**
 * Represents the kinetic law of a reaction.
 *
 * A KinetciLaw implements a a @c Scope since it may have some local @c Parameter defined that
 * are used in the kinetic-law expression.
 *
 * @ingroup ast
 */
class KineticLaw : public Node, public Scope
{
protected:
  /**
   * Holds the expression of the kinetic law.
   */
  GiNaC::ex expression;

  /**
   * Holds weak references to all parameters defined in this scope.
   */
  std::vector<Parameter *> parameters;


public:
  /**
   * Constructor.
   *
   * @param expr Specifies the kinetic law expression, the owner ship is taken by the instance.
   */
  KineticLaw(GiNaC::ex expr);

  /**
   * Destructor. Also destroies the expression passed to the constructor.
   */
  virtual ~KineticLaw();

  /**
   * Returns the expression of the kinetic law.
   */
  GiNaC::ex getRateLaw();

  /**
   * Sets the rate law expression.
   *
   * \note The replaced expression is not destroied!
   */
  void setRateLaw(GiNaC::ex node);

  /**
   * Adds a definition to the KineticLaw's scope, if something else than a @c Parameter definition
   * is added to the scope an @c InternalError exception is thrown.
   */
  virtual void addDefinition(Definition *def);

  /**
   * Returns the number of parameters defined in this scope.
   */
  size_t numParameters() const;

  /**
   * Returns the paramter by index.
   */
  Parameter *getParameter(size_t i);

  /**
   * Dumps a string representation of the kinetic law into the given stream.
   */
  virtual void dump(std::ostream &str);
};



/**
 * Represents a reaction as an AST node.
 *
 * @ingroup ast
 */
class Reaction : public Definition
{
public:
  /**
   * Defines the iterator over reactants and products of the reaction.
   */
  typedef std::map<Species *, GiNaC::ex>::iterator iterator;


protected:
  /**
   * Holds the stoichiometric expressions for all reactants of the reaction.
   */
  std::map<Species *, GiNaC::ex> reactants;

  /**
   * Holds the stoichiometric expressions for all products of the reaction.
   */
  std::map<Species *, GiNaC::ex> products;

  /**
   * Holds a set of species being modifier of the reaction.
   */
  std::set<Species *> modifiers;

  /**
   * Holds the kinetic law of the reaction.
   */
  KineticLaw *kinetic_law;

  /**
   * Is true, if the reaction is declared to be reversible.
   */
  bool is_reversible;

  /**
   * Holds the optional display name of the reaction.
   */
  std::string name;


public:
  /**
   * Constructs a new reaction.
   *
   * The stoichiometric expressions must be added once the reaction is created.
   *
   * @param id Specifies the identifier/name of the reaction.
   * @param law Specifies the kinetic law of the reaction. The ownership of the kinetic law is
   *        passed to the reaction.
   * @param reversible Specifies if the reaction is reversible.
   */
  Reaction(const std::string &id, KineticLaw *law, bool reversible);

  /**
   * Constructs a new reaction.
   * The stoichiometric expressions must be added once the reaction is created.
   *
   * @param id Specifies the identifier/name of the reaction.
   * @param name Speciefies the optional display-name of the reaction.
   * @param law Specifies the kinetic law of the reaction. The ownership of the kinetic law is
   *        passed to the reaction.
   * @param reversible Specifies if the reaction is reversible.
   */
  Reaction(const std::string &id, const std::string &name, KineticLaw *law, bool reversible);

  /**
   * Destroies the reaction.
   *
   * The destructor also frees all stoichiometric expressions and the kinetic law.
   */
  virtual ~Reaction();

  /**
   * Returns true if the given species specifies a reactant or product of the reaction.
   */
  bool hasSpecies(Species *species);

  /**
   * Returns true if the given symbol specifies a reactant or product of the reaction.
   */
  bool hasSpecies(GiNaC::symbol symbol);

  /**
   * Retunrs true if the given species specifies a reactant of the reaction.
   */
  bool hasReactant(Species *species);

  /**
   * Retunrs true if the given symbol specifies a reactant of the reaction.
   */
  bool hasReactant(GiNaC::symbol id);

  /**
   * Returns the stoichiometric expression of the given reactant.
   */
  GiNaC::ex getReactantStoichiometry(Species *species);

  /**
   * Returns the stoichiometric expression of the given reactant.
   */
  GiNaC::ex getReactantStoichiometry(GiNaC::symbol id);

  /**
   * (Re-) Sets the soichiometric expression for the given reactant.
   */
  void setReactantStoichiometry(Species *sepcies, GiNaC::ex expr);

  /**
   * Adds the given expression to the soichiometric expression of the specified reactant.
   */
  void addReactantStoichiometry(Species *species, GiNaC::ex st);

  /**
   * Retunrs true if the given species specifies a product of the reaction.
   */
  bool hasProduct(Species *species);

  /**
   * Retunrs true if the given symbol specifies a product of the reaction.
   */
  bool hasProduct(GiNaC::symbol symbol);

  /**
   * Returns the stoichiometric expression for the given reaction product.
   */
  GiNaC::ex getProductStoichiometry(Species *species);

  /**
   * Returns the stoichiometric expression for the given reaction product.
   */
  GiNaC::ex getProductStoichiometry(GiNaC::symbol id);

  /**
   * (Re-) Sets the stoichiometric expression for the given reaction product.
   */
  void setProductStoichiometry(Species *species, GiNaC::ex st);

  /**
   * Adds the given expression to the stoichiometric expression of the specified product.
   */
  void addProductStoichiometry(Species *species, GiNaC::ex st);

  /**
   * Adds a species to be modifier of the reaction.
   */
  void addModifier(Species *species);

  /**
   * Returns true, if the reaction has modifiers.
   */
  bool hasModifier() const;

  /**
   * Returns true if the given species is a modifier of the reaction.
   */
  bool isModifier(Species *species) const;

  /**
   * Returns true if the given species is a modifier of the reaction.
   */
  bool isModifier(const GiNaC::symbol &id) const;

  /**
   * Retunrs the kinetic law of the reaction.
   */
  KineticLaw *getKineticLaw();

  /**
   * (Re-) Sets the kinetic law of the reaction.
   *
   * @note The replaced law is not freed.
   */
  void setKineticLaw(KineticLaw *law);

  /**
   * Retrns true, if the reaction is defined to be reversible.
   */
  bool isReversible() const;

  /**
   * Retruns an iterator pointing on the first reactant of the reaction.
   */
  iterator reacBegin();

  /**
   * Retunrs an iterator pointing right after the last reactant of the reaction.
   */
  iterator reacEnd();

  /**
   * Returns an iterator pointing on the first product of the reaction.
   */
  iterator prodBegin();

  /**
   * Returns an iterator pointing right after the last product of the reaction.
   */
  iterator prodEnd();

  /**
   * Dumps a string representation of the reaction into the given stream.
   */
  virtual void dump(std::ostream &str);
};


}
}

#endif // REACTION_HH
