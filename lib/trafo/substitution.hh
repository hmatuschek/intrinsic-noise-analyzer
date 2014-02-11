#ifndef __INA_TRAFO_SUBSTITUTION_HH__
#define __INA_TRAFO_SUBSTITUTION_HH__

#include "../ast/ast.hh"


namespace iNA {
namespace Trafo {


/** This @c Ast::Operator implements a simple substitution operation on all expressions.
 * This operator also allows to normalize the set of substitutions and detects circular
 * substitution rules. For example the substitutions \f$a\rightarrow b\f$ and \f$b\rightarrow c\f$
 * can be normalized to the substitutions \f$a\rightarrow c\f$ and \f$b\rightarrow c\f$.
 * @ingroup trafo */
class Substitution:
    public Ast::Operator, public Ast::VariableDefinition::Operator, public Ast::Rule::Operator,
    public Ast::Reaction::Operator, public Ast::KineticLaw::Operator
{
public:
  /** Empty constructor, use @c add to add substitutions. */
  Substitution();
  /** Constructor with initial table. */
  Substitution(const GiNaC::exmap &table);

  /** Adds the replacement lhs -> rhs.
   * By default, the resulting substitution table will be normalized whenever it it modified. As
   * normalization is quiet expensive, you may disable it whenever you add several substitutions
   * at once and call @c normalize once the set of substitutions are complete. */
  void add(GiNaC::ex lhs, GiNaC::ex rhs, bool normalize=true);

  /** Explicitly normalizes the substitution table. */
  void normalize();

  /** Retunrs the substitution table. */
  const GiNaC::exmap &getTable() const;

public:
  /** Performs substitutions on the intial value of the variable, if defined.
   * Variables are @c Ast::Parameter, @c Ast::Compartment and @c Ast::Species definitions. */
  virtual void act(Ast::VariableDefinition *var);

  /** Performs substitutions on the rule expression. */
  virtual void act(Ast::Rule *rule);

  /** Performs substitutions on all stoichiometry expressions. */
  virtual void act(Ast::Reaction *reac);

  /** Performs substitutions on @c Ast::KineticLaw. */
  virtual void act(Ast::KineticLaw *law);

protected:
  /** Checks if there is any substitution like 'a -> a' which is invalid. */
  void _check_substitution_table();

  /** Normalizes a substitution table such that 'a->b, b->c' is normalized to 'a->c, b->c'. */
  void _normalize_substitution_table();

  /** Returns true, if the given expression contains any part that can be substituted. */
  bool _has_substitue(GiNaC::ex expr);

protected:
  /** Holds the normalized substitution table. */
  GiNaC::exmap _substitution_table;
};


}
}

#endif // SUBSTITUTION_HH
