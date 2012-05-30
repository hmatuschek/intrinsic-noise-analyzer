#ifndef __FLUC_EVALUATE_DIRECT_CODE_HH__
#define __FLUC_EVALUATE_DIRECT_CODE_HH__

#include <list>
#include <map>

#include <ginac/ginac.h>


namespace Fluc {
namespace Eval {


/**
 * This namespace holds all classes used to evaluate GiNaC expressions directly.
 *
 * @ingroup direct
 */
namespace direct {


/**
 * This class implements the "code" for the direct evaluation engine.
 *
 * @ingroup direct
 */
class Code {
public:
  /**
   * Iterates over all expressions of the code.
   */
  typedef std::list< std::pair<GiNaC::ex, size_t> >::iterator iterator;

  /**
   * Defines the mapping symbol -> index.
   */
  typedef std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> IndexTable;


protected:
  /**
   * Holds the symbol-table, associating a GiNaC::symbol with some index in the input vector.
   */
  IndexTable index_table;

  /**
   * Holds the list of expressions to be evaluated.
   */
  std::list< std::pair<GiNaC::ex, size_t> > expressions;


public:
  /**
   * Constructs some empty code.
   */
  Code(size_t num_threads=1);

  /**
   * Copy constructor.
   */
  Code(const Code &other);

  /**
   * Constructs some empty code with the given index-table.
   */
  Code(const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table);

  /**
   * Adds an expression to the code.
   *
   * @note This method is specific to the direct evaluation engine.
   */
  void addExpression(const GiNaC::ex &expression, size_t index);

  /**
   * Returns the index-table of the code.
   */
  IndexTable &getIndexTable();

  /**
   * (Re-) Sets the index-table.
   */
  void setIndexTable(const IndexTable &index_table);

  /**
   * Returns an iterator pointing to the first expression.
   */
  iterator begin();

  /**
   * Returns an interator pointing right after the last expression.
   */
  iterator end();
};


}
}
}

#endif
