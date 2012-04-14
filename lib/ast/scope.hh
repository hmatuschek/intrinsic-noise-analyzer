#ifndef __FLUC_COMPILER_AST_SCOPE_HH__
#define __FLUC_COMPILER_AST_SCOPE_HH__

#include <map>
#include <string>

#include "definition.hh"
#include "variabledefinition.hh"



namespace Fluc {
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
 * by a scope are owned by the scope, that means they are desroied when the scope is destroied;
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
    /**
     * Holds an instance of the map iterator.
     */
    std::map<std::string, Definition *>::iterator inner_iter;


  public:
    /**
     * Constructs a new iterator over the definitions held by the scope.
     */
    iterator(std::map<std::string, Definition *>::iterator inner_iter);

    /**
     * Retunrs the referenced definition.
     */
    Definition *operator->();

    /**
     * Returns the referenced definition.
     */
    Definition *&operator*();

    /**
     * Shifts the iterator to the next definition.
     */
    iterator const operator++(int);

    /**
     * Iterator comparison.
     */
    bool operator==(const iterator &other);

    /**
     * Iterator comparison.
     */
    bool operator!=(const iterator &other);
  };

  /**
   * Iterator class over all definititions in the scope. The order of definition is not conserved,
   * @c Ast::Model for the order of species and paramter definition.
   */
  class const_iterator
  {
  protected:
    /**
     * Holds an instance of the map iterator.
     */
    std::map<std::string, Definition *>::const_iterator inner_iter;


  public:
    /**
     * Constructs a new iterator over the definitions held by the scope.
     */
    const_iterator(std::map<std::string, Definition *>::const_iterator inner_iter);

    /**
     * Retunrs the referenced definition.
     */
    Definition * const operator->();

    /**
     * Returns the referenced definition.
     */
    Definition * const& operator*();

    /**
     * Shifts the iterator to the next definition.
     */
    const_iterator const operator++(int);

    /**
     * Iterator comparison.
     */
    bool operator==(const const_iterator &other);

    /**
     * Iterator comparison.
     */
    bool operator!=(const const_iterator &other);
  };


protected:
  /**
   * Holds the members of the scope.
   */
  std::map<std::string, Definition *> definitions;

  /**
   * If this flag is true, no reference within this scope can be resolved to a defintion outside of
   * this scope.
   */
  bool is_closed;


protected:
  /**
   * Constructs an empty scope. Is protected to avoid direct instantiation.
   */
  Scope(bool is_close=false);


public:
  /**
   * Destroies the scope and all members.
   */
  virtual ~Scope();

  /**
   * Adds a definition to the scope. If there is a definition with the same identifier the 'old' one
   * will be silently replaced.
   *
   * \note The ownership of the definition is taked by the scope.
   */
  virtual void addDefinition(Definition *def);

  /**
   * Returns true if there is a definition of the given name in the scope.
   *
   * Is equivalent to call @c hasDefinition(getSymbol(const std::string &identifier)).
   */
  bool hasDefinition(const std::string &name) const throw();

  /**
   * Returns the definition given by the name.
   */
  Definition *getDefinition(const std::string &name);

  Definition * const getDefinition(const std::string &name) const;

  /**
   * Returns true, if the scope is closed. This means if references within the scope can not access
   * definitions outside the scope.
   */
  bool isClosed();

  /**
   * Retunrs an @c iterator pointing to the first definition in the scope.
   */
  iterator begin();

  const_iterator begin() const;

  /**
   * Returns an @c iterator pointing right after the last element in the scope.
   */
  iterator end();

  const_iterator end() const;

  /**
   * Dumps a simple string representation of the scope into the given stream.
   */
  virtual void dump(std::ostream &str);
};


}
}

#endif // SCOPE_HH
