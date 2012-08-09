#ifndef __INA_TRAFO_ASSIGNMENTRULEINLINER_HH__
#define __INA_TRAFO_ASSIGNMENTRULEINLINER_HH__

#include <ast/ast.hh>
#include "filterflags.hh"
#include "substitution.hh"


namespace Fluc {
namespace Trafo {


/** This class collects assignment-rules from a model.
 * @ingroup trafo */
class AssignmentRuleCollector
    : public Ast::Visitor, Ast::VariableDefinition::Visitor
{
public:
protected:
  /** Substitutions are collected within this map. */
  Substitution &_substitutions;
  /** The flags. */
  unsigned _flags;

public:
  /** Constructor. */
  AssignmentRuleCollector(Substitution &subs, unsigned flags=Filter::SPECIES);

  /** Handles variable definition. */
  virtual void visit(const Ast::VariableDefinition *var);
};



/** This class collects and substitutes all occurrences of species, paramaters that have an
 * assignment rule attached.
 *
 * @ingroup trafo */
class AssignmentRuleInliner
    : public Substitution
{
protected:
  /** A weak reference to the model, used to resolve symbols etc. */
  Ast::Model &_model;

public:
  /** Constructor, collects some assignment rules from species and/or parameters, depending
   * on @c flags. */
  AssignmentRuleInliner(Ast::Model &model, unsigned flags=Filter::SPECIES);

  /** Special handleing of reactions, as modifiers may be extendent. */
  virtual void act(Ast::Reaction *reac);

protected:
  /** Adds all referred species in expression @c expr to the list @c species. */
  void _get_referred_species(GiNaC::ex expr, std::list<Ast::Species *> &species);

public:
  /** Constructs and applies the inliner on the given model. */
  static void apply(Ast::Model &model, unsigned flags=Filter::SPECIES);
};


}
}

#endif // ASSIGNMENTRULEINLINER_HH
