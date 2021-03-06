#ifndef __FLUC_COMPILER_AST_SCOPE_HH__
#define __FLUC_COMPILER_AST_SCOPE_HH__

#include <map>
#include <string>

#include "definition.hh"
#include "variabledefinition.hh"



namespace iNA {
namespace Ast {


// Forward declaration:
class Reaction;
class FunctionDefinition;


/**
 * Base class for all containers holding definitions
 *
 * A @c Scope is a mapping from string names to @c Definition instances. A module is a scope because
 * it contains for exampe species, function, parameter and compartment definitions. A
 * @c KineticLawDefinition is also a scope since it can hold its own parameters. The members held
 * by a scope are owned by the scope, that means they are desroyed when the scope is destroyed;
 *
 * @ingroup ast
 */
class Scope
{
public:
  /**
   * Iterator class over all definititions in the scope. The order of definition is not conserved,
   * @c Ast::Model for the order of species and paramter definition.
   */
  class iterator
  {
  protected:
    /** Holds an instance of the map iterator. */
    std::map<std::string, Definition *>::iterator inner_iter;

  public:
    /** Constructs a new iterator over the definitions held by the scope. */
    iterator(std::map<std::string, Definition *>::iterator inner_iter);
    /** Retunrs the referenced definition. */
    Definition *operator->();
    /** Returns the referenced definition. */
    Definition *&operator*();
    /** Shifts the iterator to the next definition. */
    iterator const operator++(int);
    /** Iterator comparison. */
    bool operator==(const iterator &other);
    /** Iterator comparison. */
    bool operator!=(const iterator &other);
  };

  /**
   * Iterator class over all definititions in the scope. The order of definition is not conserved,
   * @c Ast::Model for the order of species and paramter definition.
   */
  class const_iterator
  {
  protected:
    /** Holds an instance of the map iterator. */
    std::map<std::string, Definition *>::const_iterator inner_iter;

  public:
    /** Constructs a new iterator over the definitions held by the scope. */
    const_iterator(std::map<std::string, Definition *>::const_iterator inner_iter);
    /** Retunrs the referenced definition. */
    Definition * const operator->();
    /** Returns the referenced definition. */
    Definition * const& operator*();
    /** Shifts the iterator to the next definition. */
    const_iterator const operator++(int);
    /** Iterator comparison. */
    bool operator==(const const_iterator &other);
    /** Iterator comparison. */
    bool operator!=(const const_iterator &other);
  };


protected:
  /** Holds the members of the scope. */
  std::map<std::string, Definition *> _definitions;
  /** Holds map @c GiNaC::symbol -> @c VariableDefinition. */
  std::map<GiNaC::symbol, VariableDefinition *, GiNaC::ex_is_less> _symbol_table;
  /** If this flag is true, no reference within this scope can be resolved to a defintion outside of
   * this scope. */
  bool _is_closed;
  /** Holds a weak reference to the parent scope. If there is a parent scope and this scope is not
   * closed, a symbol will be first resolved in this scope and then in the parent scope. */
  Scope *_parent_scope;


public:
  /** Constructs an empty scope. Is protected to avoid direct instantiation. */
  Scope(Scope *parent=0, bool is_close=false);

  /** Destroyes the scope and all members. */
  virtual ~Scope();

  /** Returns true, if this scope has a parent scope. */
  bool hasParentScope() const;

  /** Returns the root scope (model). */
  Scope *getRootScope();

  /** Returns the root scope (model). */
  const Scope *getRootScope() const;

  /** Returns the parent scope. */
  Scope *getParentScope();

  /** Retunrs a const parent scope. */
  const Scope *getParentScope() const;

  /** (Re-) Sets the parent scope of this scope. */
  void setParent(Scope *parent);

  /** Updates the resolution table: Identifier -> @c Definition. */
  virtual void resetIdentifier(const std::string &id, const std::string &new_id);

  /** Adds a definition to the scope. If there is a definition with the same identifier the 'old' one
   * will be silently replaced.
   * \note The ownership of the definition is taked by the scope.
   */
  virtual void addDefinition(Definition *def);

  /** Removes a definition from the scope. The ownership of the definition is transferred to the
   * callee. */
  virtual void remDefinition(Definition *def);

  /** Returns true if there is a definition of the given name in the scope.
   * Is equivalent to call @c hasDefinition(getSymbol(const std::string &identifier)). */
  bool hasDefinition(const std::string &name, bool local=false) const throw();

  /** Returns the definition given by the name. */
  Definition *getDefinition(const std::string &name, bool local=false);
  /** Returns a const reference to the definition. */
  Definition * getDefinition(const std::string &name) const;

  /** Returns true, if the given identifier names a @c VariableDefinition. */
  bool hasVariable(const std::string &identifier) const;
  /** Returns true, if the given symbol belongs to a variable definition. */
  bool hasVariable(const GiNaC::symbol &symbol) const;
  /** Returns the variable definition by name.
   * Is equivalent to call @c getVariable(getSymbol(const std::string &identifier).
   * @throws SymbolError If identifier is not associated with a variable. */
  VariableDefinition *getVariable(const std::string &identifier);
  /** Returns the variable definition by name.
   * Is equivalent to call @c getVariable(getSymbol(const std::string &identifier).
   * @throws SymbolError If identifier is not associated with a variable. */
  VariableDefinition * const getVariable(const std::string &identifier) const;
  /** Returns the variable definition associated with the given symbol.
   * @throws SymbolError If the symbol is not associated with a variable definition. */
  VariableDefinition *getVariable(const GiNaC::symbol &symbol);
  /** Returns the variable definition associated with the given symbol.
   * @throws SymbolError If the symbol is not associated with a variable definition. */
  VariableDefinition * const getVariable(const GiNaC::symbol &symbol) const;

  /** Returns true, if the scope is closed. This means if references within the scope can not access
   * definitions outside the scope. */
  bool isClosed() const;

  /** Finds a similar identifer that is not in use. */
  std::string getNewIdentifier(const std::string &base_name);

  /** Retunrs an @c iterator pointing to the first definition in the scope. */
  iterator begin();
  /** Returns a @c const_iterator pointing to the first definition in the scope. */
  const_iterator begin() const;
  /** Returns an @c iterator pointing right after the last element in the scope. */
  iterator end();
  /** Returns an @c const_iterator pointing right after the last element in the scope. */
  const_iterator end() const;

  /** Dumps a simple string representation of the scope into the given stream. */
  virtual void dump(std::ostream &str);

  /** Handles a visitor for the scope (forward for all definitions) .*/
  virtual void accept(Ast::Visitor &visitor) const ;

  /** Applies an operator on all definitinos of the scope. */
  virtual void apply(Ast::Operator &op);

  /** Forwards the visitor to all definitions within this scope. */
  virtual void traverse(Ast::Visitor &visitor) const;

  /** Applies the operator on all definitions within this scope. */
  virtual void traverse(Ast::Operator &op);
};


}
}

#endif // SCOPE_HH
