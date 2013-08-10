#ifndef __INA_NLESOLVE_NEWTONRAPHSON_HH
#define __INA_NLESOLVE_NEWTONRAPHSON_HH

#include "../eval/eval.hh"
#include "../trafo/constantfolder.hh"
#include "nlesolver.hh"
#include "precisionsolve.hh"

namespace iNA {
namespace NLEsolve {


inline double
maxNorm(const Eigen::VectorXd &vector)

{
    return vector.lpNorm<Eigen::Infinity>();
}


/**
 * Nonlinear algebraic equation solver using the Newton-Raphson method with linesearch.
 * @ingroup nlesolve
 */
template<class T,
         class VectorEngine=Eval::bci::Engine<Eigen::VectorXd>,
         class MatrixEngine=Eval::bci::Engine<Eigen::VectorXd,Eigen::MatrixXd> >
class NewtonRaphson
    : public NLEsolver<T, VectorEngine, MatrixEngine>
{

protected:

   PrecisionSolve LinSolver;

public:

   struct params {
       params(size_t dim) : linesearch(Optimization), maxIterations(100*(dim+1)),
                            relError(1.e-9), absError(1.e-9),
                            ALF(1.e-4), TOLX(1.e-9), TOLMIN(1.e-12), STPMX(100.){}

       LineSearchMethod linesearch;

       size_t maxIterations;   // maximum number of function evaluation
       /**
        * @brief Relative error
        */
       double relError;
       /**
        * @brief Absolute error
        */
       double absError;

       /**
        * @brief Acceptable step length for Wolfe condition. Don't change standard value.
        */
       double ALF;

       /**
        * @brief Tolerances for linesearch.
        */
       double TOLX,TOLMIN;

       /**
        * @brief Maximum steps for linesearch. Assumes standard value.
        */
       int STPMX;

   };

   params parameters;

  /**
   * Constructor...
   */

  NewtonRaphson(T &model)
      : NLEsolver<T, VectorEngine, MatrixEngine>(model),
        LinSolver(model.numIndSpecies()),
        parameters(model.numIndSpecies())

  {

      // Pass...

  }

  virtual ~NewtonRaphson(){ };

  const Eigen::MatrixXd&
  getJacobianM()

  {
     return this->JacobianM;
  }

  /**
   * @brief Run the Newton-Raphson solver
   * @param state provides initial condition for the solver which is updated on exist
   * @return Status of the solver.
   */
  virtual Status
  solve(Eigen::VectorXd &state)

  {

      Eigen::VectorXd state_old;

      Eigen::VectorXd nablaf;
      Eigen::VectorXd dx;

      // Calculate maximum step size heuristic
      const double stpmax=this->parameters.STPMX*std::max(state.head(this->dim).norm(),double(this->dim));

      // Do Newton iteration
      for(this->iterations=1;this->iterations<parameters.maxIterations;this->iterations++)
      {

          state_old = state;

          LineSearchStatus lcheck = newtonStep(state_old,state,stpmax);

          if ((state.head(this->dim).array()<0).any())
          {
              state = state_old;
              return NegativeValues;
          }

          // Check linesearch
          switch(lcheck)
          {
            case RoundOffProblem:
            case LineSearchFailed:
              state = state_old;
              return IterationFailed;
            default: break;
          }

          // Test for absolute and relative errors
          if (maxNorm(state-state_old) < std::max(this->parameters.absError, maxNorm(state)*this->parameters.relError )
              && maxNorm(this->ODEs) < std::max(this->parameters.absError, maxNorm(state)*this->parameters.relError) )
          {
              //convergence of dx
              return Success;
          }

      } // next newton iteration

      return MaxIterationsReached;
  }

  /**
   * @brief Performs a single Newton-Raphson iteration with linesearch
   * @param inState : initial state
   * @param outState : output state updated on exit
   * @param stpmax : Maximum number of steps used in linesearch
   * @return
   */
  LineSearchStatus
  newtonStep(const Eigen::VectorXd &inState, Eigen::VectorXd &outState ,double stpmax)

  {

      double f,fold;

      double test,temp,den;

      Eigen::VectorXd nablaf;
      Eigen::VectorXd dx;

      // Construct Jacobian matrix
      this->interpreter.run(inState,this->ODEs);
      this->jacobian_interpreter.run(inState,this->JacobianM);

      // Evaluate objective function f
      f = .5*(this->ODEs.squaredNorm());
      // Calculate steepest descent direction
      nablaf = this->JacobianM.transpose()*this->ODEs;

      // Store also old value of objective function f
      fold = f;

      // Solve JacobianM*dx=-REs to obtain the update vector
      dx = LinSolver.solve(this->JacobianM, -this->ODEs);

      // Perform linesearch
      LineSearchStatus lcheck;
      switch(parameters.linesearch)
      {
          case Optimization:
            lcheck = this->linesearch(inState, outState, dx, fold, f, nablaf, stpmax);
            break;
          case Damped:
            lcheck = this->linesearch2(inState, outState, dx, fold, f, stpmax);
          default: return Done;

      }
      //< returns new outState and f, also updates ODEs

      // Check for NaNs
      for(size_t i=0; i<this->dim; i++)
        if(std::isnan(outState(i))) return LineSearchFailed;

      // Check for spurious convergence of nablaf = 0
      if (lcheck == LineSearchFailed) {
         test = 0.0;
         den = std::max(f,0.5*double(this->dim));
         for(size_t i=0; i<this->dim;i++)
         {
             temp = std::abs(nablaf(i))*std::max(std::abs(outState(i)),1.)/den;
             if (temp > test) test = temp;
         }

         if(test < this->parameters.TOLMIN)
         {
             return LineSearchFailed;
         }
         else
         {
             return Converged;
         }
      }

      return lcheck;

  }




  /**
   * Adaptive line search method.
   *
   * The method uses a cubic interpolation formula to find the optimal step size.
   * \cite press2007
   *
   * @param xold : start function arguments
   * @param x : end function arguments
   * @param dx : the search direction
   * @param fold : old function value
   * @param f : new function value
   * @param nablaf : gradient of f in search direction
   *
   */
  LineSearchStatus
  linesearch(const Eigen::VectorXd &xold, Eigen::VectorXd &x, Eigen::VectorXd &dx,
                                      const double fold, double &f, const Eigen::VectorXd &nablaf,
                                      const double &stpmax)
  {

      //size_t dim = xold.size();

      double temp=0., test=0.;
      double rhs1=0., rhs2=0.;
      double lambda=0., lambda2=0., lambdamin=0., tmplambda=0;
      double a=0., b=0., disc=0., f2=0.;

      double norm = dx.norm();
      // scale if dx is too large
      if(norm > stpmax) dx*=(stpmax/norm);

      double slope = nablaf.adjoint()*dx;
      if (slope >= 0.) return RoundOffProblem;

      // determine minimum lambda
      test = 0.0;
      for (size_t i=0;i<this->dim;i++)
      {
          temp=std::abs(dx(i))/std::max(abs(i),1);
          if (temp > test) test = temp;
      }
      lambdamin = this->parameters.TOLX/test;

      // first do newton step
      lambda   = 1.0;
      for(int it=0;;it++){

          x.head(this->dim) = xold.head(this->dim)+lambda*dx;

          // evaluate rate equations
          this->interpreter.run(x,this->ODEs);

          f = 0.5*(this->ODEs.squaredNorm());

          if (lambda < lambdamin)
          {
             x=xold;
             return LineSearchFailed;
          }
          else if (f <= (fold + this->parameters.ALF*lambda*slope) )
          {
              return Done;
          }
          else
          {
              if (lambda == 1.0)
                  tmplambda = -slope/(2.*(f-fold-slope));
              else
              {
                 rhs1 = f-fold-lambda*slope;
                 rhs2 = f2-fold-lambda2*slope;
                 a = (rhs1/(lambda*lambda)-rhs2/(lambda2*lambda2))/(lambda-lambda2);
                 b = (-lambda2*rhs1/(lambda*lambda)+lambda*rhs2/(lambda2*lambda2))/(lambda-lambda2);
                 if (a==0.) tmplambda = -slope/(2.*b);
                 else{
                    disc=b*b-3.*a*slope;
                    if (disc<0.) tmplambda = 0.5*lambda;
                    else if (b<=0.) tmplambda=(-b+std::sqrt(disc))/(3.*a);
                    else tmplambda=-slope/(b+std::sqrt(disc));
                 }
                 if (tmplambda>0.5*lambda) tmplambda = 0.5*lambda;
              }
          }

          lambda2 = lambda;
          f2 = f;
          lambda = std::max(tmplambda,0.1*lambda);

          if(it>100000) return LineSearchFailed;

      }

  }

  /**
   * Damped line search method.
   *
   * @ingroup nlesolve
   * @todo this method does not work properly.
   *
   * This method simply halfens the step size.
   */

  LineSearchStatus
  linesearch2(const Eigen::VectorXd &xold, Eigen::VectorXd &x, Eigen::VectorXd &dx,
                                      const double fold, double &f, double stpmax)
  {

      // first try Newton step

      int n=0;

      double lambda=1;


      double norm = dx.norm();
      // scale if dx is too large
      if(norm > stpmax) dx*=(stpmax/norm);

      // loop over until f has sufficiently decreased
      while(f > fold)
      {

          n++;

          x = xold+lambda*dx;

          this->interpreter.run(x,this->ODEs);

          f = 0.5*this->ODEs.squaredNorm();

          // take half step size
          lambda *= 0.5;

          if(n>32)
          {
              std::cerr<<"max reached"<<std::endl;
              return LineSearchFailed;
          }

      }

      return Done;

  }

};



}
}


#endif // NEWTONRAPHSON_HH
