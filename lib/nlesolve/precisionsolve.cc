#include "precisionsolve.hh"

using namespace iNA::NLEsolve;

PrecisionSolve::PrecisionSolve(size_t size) :
    luPP(size), luFP(size,size), x(size)

{
    // Pass...
}

const Eigen::VectorXd &
PrecisionSolve::solve(const Eigen::MatrixXd &B, const Eigen::VectorXd &A, double epsilon)

{

    // this is fast
    luPP.compute(B);
    x = luPP.solve(A);

    if(!(B*x).isApprox(A, epsilon))
    {
       // this is slower
      luFP.compute(B);
      x = luFP.solve(A);
    }

    return x;

}

