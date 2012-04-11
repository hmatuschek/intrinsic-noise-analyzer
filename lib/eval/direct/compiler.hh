#ifndef __FLUC_EVALUATE_DIRECT_COMPILER_HH__
#define __FLUC_EVALUATE_DIRECT_COMPILER_HH__

#include "code.hh"
#include "ast/model.hh"
#include "eigen3/Eigen/Eigen"
#include "ginacsupportforeigen.hh"


namespace Fluc {
namespace Evaluate {
namespace direct {

/**
 * This class implements the "compiler" for the direct evaluation engine.
 *
 * @ingroup eval
 */
template <class InType, class OutType=InType>
class Compiler
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
  void compileExpressionAndStore(GiNaC::ex &expression, size_t index)
  {
    this->code->addExpression(expression, index);
  }


  /**
   * Compiles a vector of expressions.
   *
   * The value of the i-th expression will be stored at the i-th index in the output-vector
   * during evaluation.
   */
  void compileVector(Eigen::VectorXex &vector)
  {
    for (int index = 0; index < vector.rows(); index++)
    {
      this->compileExpressionAndStore(vector(index), index);
    }
  }


  /**
   * Compiles a matrix of expressions, that will evaluate to an @c Eigen::MatrixXd of the same
   * shape.
   */
  void compileMatrix(Eigen::MatrixXex &matrix)
  {
    for (int i=0; i<matrix.rows(); i++)
    {
      for (int j=0; j<matrix.cols(); j++)
      {
        size_t index = 0;
        if (OutType::Flags & Eigen::RowMajorBit)
          index = j + matrix.cols()*i;
        else
          index = i + matrix.rows()*j;

        this->compileExpressionAndStore(matrix(i,j), index);
      }
    }
  }


  /**
   * Does nothing, but is required to satisfy the evaluation interface.
   */
  void finalize(size_t opt_level=0)
  {
    // Pass...
  }
};

}
}
}


#endif
