#ifndef __FLUC_EVALUATE_COMPILERCOMMON_HH__
#define __FLUC_EVALUATE_COMPILERCOMMON_HH__

#include <eigen3/Eigen/Eigen>
#include "ginacsupportforeigen.hh"


namespace Fluc {
namespace Evaluate {

/**
 * This class implements the compilation method for vectors and matrices of GiNaC expressions.
 *
 * The compilation of vectors and matrices does not depend directly on the evaluation engine and
 * the way how single expressions are compiled. Therefore these methods are implemented in a general
 * class.
 *
 * @ingroup eval
 */
template<class InType, class OutType=InType>
class CompilerCommon
{
public:
  /**
   * Needs to be implemented by any compiler to finalize (compile and optimize) the generated code.
   */
  virtual void finalize(size_t opt_level=0) = 0;

  /**
   * Defines the interface for singe-expression compilation, implemented by the execution-engine
   * specific compiler.
   */
  virtual void compileExpressionAndStore(const GiNaC::ex &expr, size_t index) = 0;


  /**
   * Compiles a vector of expressions.
   *
   * The value of the i-th expression will be stored at the i-th index in the output-vector
   * during evaluation.
   */
  void compileVector(const Eigen::VectorXex &vector)
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
};


}
}


#endif // COMPILERCOMMON_HH
