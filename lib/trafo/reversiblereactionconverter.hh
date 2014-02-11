#ifndef __INA_TRAFO_REVERSIBLEREACTIONCONVERTER_HH__
#define __INA_TRAFO_REVERSIBLEREACTIONCONVERTER_HH__

#include "../ast/ast.hh"

namespace iNA {
namespace Trafo {


/**
 * This class implements a simple algorithm to create two irreversible reactions from a single
 * reversible one.
 * @ingroup trafo
 */
class ReversibleReactionConverter
{
public:
  /** Expands reversible reactions into irreversible ones. */
  static void apply(Ast::Model &model);
  /** Expands reversible reactions into irreversible ones and stores the IDs of the modified
   * and new reactions. */
  static void apply(Ast::Model &model, std::list<std::string> &modified,
                    std::list<std::string> &added);
};

/**
 * This class collapses two irreversible reactions to a single
 * reversible one.
 * @ingroup trafo
 */
class IrreversibleReactionCollapser
{
public:
  /** Combines irreversible reactions. */
  static void apply(Ast::Model &model);
  /** Combines irreversible reactions and stores the identifiers of the modified and removed
   * reactions within the given lists. */
  static void apply(Ast::Model &model, std::list<std::string> &modified,
                    std::list<std::string> &removed);
};

}
}

#endif // __INA_TRAFO_REVERSIBLEREACTIONCONVERTER_HH__
