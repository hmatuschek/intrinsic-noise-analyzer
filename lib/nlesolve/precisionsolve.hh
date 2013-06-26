#ifndef __INA_NLESOLVE_PRECISIONSOLVE_HH
#define __INA_NLESOLVE_PRECISIONSOLVE_HH

#include <eigen3/Eigen/Eigen>

namespace iNA{

namespace NLEsolve{

/** Solves a linear system of equations.
 * Simple wrapper about Eigen's LU decomposition without reallocation and internal storage of
 * result vector. */
class PrecisionSolve
{
  /** LU decomposition with partial pivoting. */
  Eigen::PartialPivLU<Eigen::MatrixXd> luPP;
  /** LU decomposition with full pivoting. */
  Eigen::FullPivLU<Eigen::MatrixXd> luFP;
  /** Temporary variable for the solution. */
  Eigen::VectorXd x;

public:
  /** Constructs a solver for a @c size -dimensional system. */
  PrecisionSolve(size_t size);

  /** Solves \f$Bx=A\f$ using the partial pivoted LU decomposition.
   * @param B Specifies the system matrix.
   * @param A Specifies the vector of results.
   * @param epsilon Specifies the required (absolute) precision. */
  const Eigen::VectorXd &solve(const Eigen::MatrixXd &B, const Eigen::VectorXd &A, double epsilon=1.e-9);

  /** Static variant of @c solve. */
  static inline Eigen::VectorXd
  precisionSolve(const Eigen::MatrixXd &B, const Eigen::VectorXd &A, double epsilon=1.e-9) {
      // First, try to solve using LU w partial pivoting, this is fast
      Eigen::VectorXd x = B.lu().solve(A);
      // If not precise enougth -> use full pivoting
      if((B*x).isApprox(A,epsilon)) {
         // this is rather slow
         Eigen::FullPivLU<Eigen::MatrixXd> LU(B);
         x = LU.solve(A);
      }

      return x;
  }
};

}}


#endif // PRECISIONSOLVE_HH
