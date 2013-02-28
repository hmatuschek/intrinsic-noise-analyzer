#ifndef __INA_NLESOLVE_PRECISIONSOLVE_HH
#define __INA_NLESOLVE_PRECISIONSOLVE_HH

#include <eigen3/Eigen/Eigen>

namespace iNA{

namespace NLEsolve{

/* Solves a linear system of equations.
   Simple wrapper about Eigen's LU decomposition without reallocation and internal storage of result vector. */

class PrecisionSolve
{

  Eigen::PartialPivLU<Eigen::MatrixXd> luPP;
  Eigen::FullPivLU<Eigen::MatrixXd> luFP;
  Eigen::VectorXd x;

public:

  PrecisionSolve(size_t size);

  const Eigen::VectorXd &solve(const Eigen::MatrixXd &B, const Eigen::VectorXd &A, double epsilon=1.e-9);

  static inline Eigen::VectorXd precisionSolve(const Eigen::MatrixXd &B, const Eigen::VectorXd &A, double epsilon=1.e-9)

  {

      // this is fast
      Eigen::VectorXd x = B.lu().solve(A);
      if((B*x).isApprox(A,epsilon))
      {
         // this is rather slow
         Eigen::FullPivLU<Eigen::MatrixXd> LU(B);
         x = LU.solve(A);
      }

      return x;

  }

};

}}


#endif // PRECISIONSOLVE_HH
