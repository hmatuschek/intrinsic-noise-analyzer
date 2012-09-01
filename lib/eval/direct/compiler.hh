#ifndef __FLUC_EVALUATE_DIRECT_COMPILER_HH__
#define __FLUC_EVALUATE_DIRECT_COMPILER_HH__

#include "code.hh"
#include "ast/model.hh"
#include "eigen3/Eigen/Eigen"
#include "ginacsupportforeigen.hh"
#include "eval/compilercommon.hh"


namespace iNA {
namespace Eval {
namespace direct {

/**
 * This class implements the "compiler" for the direct evaluation engine.
 *
 * @ingroup direct
 */
template <class InType, class OutType=InType>
class Compiler : public CompilerCommon<InType, OutType>
{
protected:
  /**
   * Holds the "code".
   */
  Code *code;

  /**
   * Holds the index-table mapping a GiNaC symbol to an index of the input-vector.
   */
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> index_table;


public:
  /**
   * Empty constructor.
   */
  Compiler()
    : code(0)
  {
    // Pass...
  }


  /**
   * Constructor with index-table.
   */
  Compiler(const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table)
    : index_table(index_table)
  {
    // Pass...
  }


  /**
   * Constructs a compiler using the symbol->index mapping of the given @c Ast::Model.
   */
  Compiler(const Ast::Model &model)
  {
    // Associate species symbol with its index in model:
    for (size_t i=0; i<model.numSpecies(); i++) {
      this->code.getSymbolTable()[model.getSpecies(i)->getSymbol()] = i;
    }
  }


  /**
   * Sets the code object.
   */
  void setCode(Code *code)
  {
    this->code = code;
    code->setIndexTable(this->index_table);
  }


  /**
   * "Compiles" the given expression, its result will be stored in the output
   * vector at the given index.
   */
  virtual void compileExpressionAndStore(const GiNaC::ex &expression, size_t index)
  {
    this->code->addExpression(expression, index);
  }


  /**
   * Does nothing, but is required to satisfy the evaluation interface.
   */
  virtual void finalize(size_t opt_level=0)
  {
    // Pass...
  }
};

}
}
}


#endif
