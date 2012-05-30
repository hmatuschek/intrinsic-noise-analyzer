#ifndef __FLUC_AST_TRAFO_SUBSTITUTIONCOLLECTOR_HH__
#define __FLUC_AST_TRAFO_SUBSTITUTIONCOLLECTOR_HH__

#include "walker.hh"


namespace Fluc {
namespace Ast {
namespace Trafo {


/**
 * Just collects all symbols and their substitutions, that are commonly performed. Like
 * parameters or species, defined by an assignment-rule. This type of substitutions must be
 * performed before any evaluation or compilation of expressions can be performed.
 *
 * @ingroup ast
 */
class SubstitutionCollector : public Walker
{
protected:
  /**
   * Holds a weak reference to the substitution table, to populate.
   */
  GiNaC::exmap &table;

public:
  /**
   * Constructor.
   *
   * @param table Specifies the substitution table to be populated with substitutions.
   */
  SubstitutionCollector(GiNaC::exmap &table);

  /**
   * Checks if the variable is constant or defined by an AssignmentRule. If this is the case,
   * the variable is marked for substitution.
   */
  virtual void handleVariableDefinition(Ast::VariableDefinition *node);

protected:
  /**
   * Adds a substitution to the substitution table @c table. This method ensures that the
   * substitutions remain confluent.
   */
  void addSubstitution(const GiNaC::symbol &symbol, GiNaC::ex expression);
};


}
}
}

#endif // SUBSTITUTIONCOLLECTOR_HH
