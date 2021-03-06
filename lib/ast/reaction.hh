#ifndef __FLUC_COMPILER_AST_REACTION_HH__
#define __FLUC_COMPILER_AST_REACTION_HH__

#include <map>

#include "definition.hh"
#include "parameter.hh"
#include "scope.hh"
#include "species.hh"
#include <ginac/ginac.h>


namespace iNA {
namespace Ast {


/** Represents the kinetic law of a reaction.
 * A KineticLaw implements a a @c Scope since it may have some local @c Parameter defined that
 * are used in the kinetic-law expression.
 * @ingroup ast */
class KineticLaw : public Node, public Scope
{
public:
  /** Visitor class for kinetic laws. */
  class Visitor {
  public:
    /** Implement this method for KineticLaw visitors. */
    virtual void visit(const KineticLaw *law) = 0;
  };

  /** Operator class for kinetic laws. */
  class Operator {
  public:
    /** Implement this method for KineticLaw operators. */
    virtual void act(KineticLaw *law) = 0;
  };


public:
  /** Constructor.
   * @param expr Specifies the kinetic law expression, the owner ship is taken by the instance. */
  KineticLaw(GiNaC::ex expr);

  /** Destructor. Also destroies the expression passed to the constructor. */
  virtual ~KineticLaw();

  /** Returns the expression of the kinetic law. */
  GiNaC::ex getRateLaw() const;

  /** Sets the rate law expression. */
  void setRateLaw(GiNaC::ex node);

  /** Adds a definition to the KineticLaw's scope, if something else than a @c Parameter definition
   * is added to the scope an @c InternalError exception is thrown. */
  virtual void addDefinition(Definition *def);

  /** Removes the parameter from the list of local paramters. */
  virtual void remDefinition(Definition *def);

  /** Returns the number of parameters defined in this scope. */
  size_t numParameters() const;

  /** Returns the paramter by index. */
  Parameter *getParameter(size_t i);

  /** Returns the parameter by identifier. */
  Parameter *getParameter(const std::string &identifier);

  /** Removes all redundant parameters, i.e., those which are not present in the rate law. */
  void cleanUpParameters();

  /** Dumps a string representation of the kinetic law into the given stream. */
  virtual void dump(std::ostream &str);

  /** Handles a visitor. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator. */
  virtual void apply(Ast::Operator &op);

  /** Forwards the visitor to the local paramter definitions. */
  virtual void traverse(Ast::Visitor &visitor) const;

  /** Applies the visitor on the local paramter definitions. */
  virtual void traverse(Ast::Operator &op);

protected:
  /** Holds the expression of the kinetic law. */
  GiNaC::ex _expression;

  /** Holds weak references to all parameters defined in this scope. */
  std::vector<Parameter *> _parameters;
};



/** Represents a reaction as an AST node.
 * @ingroup ast */
class Reaction : public Definition
{
public:
  /** Defines the iterator over reactants and products of the reaction. */
  typedef std::map<Species *, GiNaC::ex>::iterator iterator;

  /** Defines the const iterator over reactants and products of the reaction. */
  typedef std::map<Species *, GiNaC::ex>::const_iterator const_iterator;


  /** Visitor class for reactions. */
  class Visitor {
  public:
    /** Implement this method for Reaction visitors. */
    virtual void visit(const Reaction *reac) = 0;
  };

  /** Operator class for reactions. */
  class Operator {
  public:
    /** Implement this method for Reaction operators. */
    virtual void act(Reaction *reac) = 0;
  };


public:
  /** Constructs a new reaction.
   * The stoichiometric expressions must be added once the reaction is created.
   * @param id Specifies the identifier/name of the reaction.
   * @param law Specifies the kinetic law of the reaction. The ownership of the kinetic law is
   *        passed to the reaction.
   * @param reversible Specifies if the reaction is reversible. */
  Reaction(const std::string &id, KineticLaw *law, bool reversible);

  /** Constructs a new reaction.
   * The stoichiometric expressions must be added once the reaction is created.
   * @param id Specifies the identifier/name of the reaction.
   * @param name Speciefies the optional display-name of the reaction.
   * @param law Specifies the kinetic law of the reaction. The ownership of the kinetic law is
   *        passed to the reaction.
   * @param reversible Specifies if the reaction is reversible. */
  Reaction(const std::string &id, const std::string &_name, KineticLaw *law, bool reversible);

  /** Destroys the reaction. The destructor also frees all stoichiometric expressions and the
   * kinetic law. */
  virtual ~Reaction();

  /** Returns true if the given species specifies a reactant or product of the reaction. */
  bool hasSpecies(Species *species);
  /** Returns true if the given symbol specifies a reactant or product of the reaction. */
  bool hasSpecies(GiNaC::symbol symbol);

  /** Retunrs true if the given species specifies a reactant of the reaction. */
  bool hasReactant(Species *species) const;
  /** Retunrs true if the given symbol specifies a reactant of the reaction. */
  bool hasReactant(GiNaC::symbol id) const;
  /** Returns the number of reactants for the reaction. */
  size_t numReactants() const;

  /** Returns the stoichiometric expression of the given reactant. */
  GiNaC::ex getReactantStoichiometry(Species *species);
  /** Returns the stoichiometric expression of the given reactant. */
  GiNaC::ex getReactantStoichiometry(GiNaC::symbol id);

  /** (Re-) Sets the soichiometric expression for the given reactant. */
  void setReactantStoichiometry(Species *sepcies, GiNaC::ex expr);
  /** Adds the given expression to the soichiometric expression of the specified reactant. */
  void addReactantStoichiometry(Species *species, GiNaC::ex st);

  /** Removes all reactants from the reaction. */
  void clearReactants();

  /** Retunrs true if the given species specifies a product of the reaction. */
  bool hasProduct(Species *species) const;
  /** Retunrs true if the given symbol specifies a product of the reaction. */
  bool hasProduct(GiNaC::symbol symbol) const;
  /** Returns the number of products. */
  size_t numProducts() const;

  /** Returns the stoichiometric expression for the given reaction product. */
  GiNaC::ex getProductStoichiometry(Species *species);
  /** Returns the stoichiometric expression for the given reaction product. */
  GiNaC::ex getProductStoichiometry(GiNaC::symbol id);

  /** (Re-) Sets the stoichiometric expression for the given reaction product. */
  void setProductStoichiometry(Species *species, GiNaC::ex st);
  /** Adds the given expression to the stoichiometric expression of the specified product. */
  void addProductStoichiometry(Species *species, GiNaC::ex st);

  /** Removes all products from the reaction. */
  void clearProducts();

  /** Returns true if the given species is a modifier of the reaction.
   * This means that the @c KineticLaw depends on this species but it is neither a reactant or a
   * product of the reaction. */
  bool isModifier(Species *species) const;
  /** Returns true if the given species is a modifier of the reaction.
   * This means that the @c KineticLaw depends on this species but it is neither a reactant or a
   * product of the reaction. */
  bool isModifier(const GiNaC::symbol &id) const;

  /** Returns the kinetic law of the reaction. */
  KineticLaw *getKineticLaw() const;
  /** (Re-) Sets the kinetic law of the reaction.
   * @note The replaced law is not destroyed. */
  void setKineticLaw(KineticLaw *law);

  /** Returns true, if the reaction is defined to be reversible. */
  bool isReversible() const;
  /** (Re-) Sets the reversible flag of the reaction. */
  void setReversible(bool val);

  /** Tells you wether the reaction can be considered as the reverse of another and vice versa. */
  bool isReverseOf(Reaction *other);

  /** Returns an iterator pointing on the first reactant of the reaction. */
  iterator reactantsBegin();
  /** Returns an iterator pointing right after the last reactant of the reaction. */
  iterator reactantsEnd();

  /** Retunrs an iterator pointing on the first reactant of the reaction. */
  const_iterator reactantsBegin() const;
  /** Retunrs an iterator pointing right after the last reactant of the reaction. */
  const_iterator reactantsEnd() const;

  /** Returns an iterator pointing on the first product of the reaction. */
  iterator productsBegin();
  /** Returns an iterator pointing right after the last product of the reaction. */
  iterator productsEnd();

  /** Returns an iterator pointing on the first product of the reaction. */
  const_iterator productsBegin() const;
  /** Returns an iterator pointing right after the last product of the reaction. */
  const_iterator productsEnd() const;

  /** Dumps a string representation of the reaction into the given stream. */
  virtual void dump(std::ostream &str);

  /** Handles a visitor for the reaction. */
  virtual void accept(Ast::Visitor &visitor) const;

  /** Applies an operator on the reaction. */
  virtual void apply(Ast::Operator &op);

  /** Forwards the visitor to the kinetic law. */
  virtual void traverse(Ast::Visitor &visitor) const;

  /** Applies the operator on the kinetic law. */
  virtual void traverse(Ast::Operator &op);

protected:
  /** Holds the stoichiometric expressions for all reactants of the reaction. */
  std::map<Species *, GiNaC::ex> _reactants;

  /** Holds the stoichiometric expressions for all products of the reaction. */
  std::map<Species *, GiNaC::ex> _products;

  /** Holds the kinetic law of the reaction. */
  KineticLaw *_kinetic_law;

  /** Is true, if the reaction is declared to be reversible. */
  bool _is_reversible;

  /** Holds the optional display name of the reaction. */
  std::string _name;
};


}
}

#endif // REACTION_HH
