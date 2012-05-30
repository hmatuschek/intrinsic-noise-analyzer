/**
 * @defgroup nlesolve Nonlinear solver
 * @ingroup math
 */

#ifndef __FLUC_MODELS_NLESOLVE_HH
#define __FLUC_MODELS_NLESOLVE_HH

#include <eigen3/Eigen/Eigen>
#include <iostream>

namespace Fluc {
namespace NLEsolve {

    enum Status {
        Waiting = 0,
        Success = 1,
        MaxIterationsReached = 2,
        LineSearchFailed = 3,
        RoundOffProblem = 4,
    };

/**
 * NewtonRaphson solver. This class is a bit more efficient with memory and things
 * over the old function calls.
 * @ingroup nlesolve
 */
template<typename T>
class NewtonRaphson
{
protected:
  /**
   * Holds a reference to an instance of LinearNoiseApproximation.
   */
   T &that;

   Eigen::MatrixXd JacobianM;
   Eigen::VectorXd REs;

   size_t iterations;

public:

   struct params {
       params(size_t dim) : maxIterations(100*(dim+1)), epsilon(1.e-9),
           ALF(1.e-4), TOLX(std::numeric_limits<double>::epsilon()), TOLF(epsilon), TOLMIN(1.e-12), STPMX(100.){}

       size_t maxIterations;   // maximum number of function evaluation
       double epsilon;

       double ALF;
       double TOLX,TOLF,TOLMIN;
       int STPMX;

   };

   params parameters;

  /**
   * Constructor...
   */

  NewtonRaphson(T &model)
      : that(model), parameters(model.numIndSpecies())
  {
      // ... pass
  }

  const Eigen::MatrixXd&
  getJacobianM()
  {
     return this->JacobianM;
  }

  int getIterations()
  { return this->iterations; }

  Status
  solve(Eigen::VectorXd &conc)
  {

      double test,temp,den;
      double f,fold;

      Eigen::VectorXd conc_old;
      Eigen::VectorXd rhs;

      Eigen::VectorXd nablaf;
      Eigen::VectorXd dx;

      //double delta=1;

      bool lcheck=true;

      // evaluate rate equations
      that.getREs(conc,REs);
      // construct also Jacobian matrix
      that.getJacobianMatrix(JacobianM);

      // dimension
      size_t dim = conc.size();

      // calc max step
      const double stpmax=this->parameters.STPMX*std::max(conc.norm(),double(dim));

      for(size_t k=0;k<this->parameters.maxIterations;k++)
      {

          // compute f to minimize
          f = .5*(REs.squaredNorm());
          // calculate steepest descent direction
          nablaf = JacobianM.transpose()*REs;

          // store old concentrations
          conc_old = conc;
          // store also old f
          fold = f;

          // solve JacobianM*dx=-REs
          dx = JacobianM.fullPivLu().solve(-REs);

          // if linesearch failed try with full pivoting.
          lcheck = this->linesearch(conc_old, conc, dx, fold, f, nablaf, stpmax);

          //< returns new conc, f and updates REs with new state

          // construct Jacobian matrix
          that.getJacobianMatrix(JacobianM);

          // test for convergence of REs
          if ( REs.lpNorm<Eigen::Infinity>() < this->parameters.TOLF)
          {
             this->iterations=k+1;
             return Success;
          }

          // check for spurious convergence of nablaf = 0
          if (lcheck==LineSearchFailed) {
             test = 0.0;
             den = std::max(f,0.5*double(dim));
             for(size_t i=0; i<dim;i++)
             {
                 temp = std::abs(nablaf(i))*std::max(std::abs(conc(i)),1.)/den;
                 if (temp > test) test = temp;
             }

             /* @todo check this return */
             if(test < this->parameters.TOLMIN)
                 return LineSearchFailed;
             else
             {
                 this->iterations=k+1;
                 return Success;
             }
          }

          // test for convergence of dx
          test = 0.;
          for(size_t i=0;i<dim;i++)
          {
              temp = (std::abs(conc(i)-conc_old(i)))/std::max(conc(i),1.);
              if (temp > test) test = temp;
          }

          if (test < this->parameters.TOLX)
          {
              //convergence of dx
              this->iterations=k+1;
              return Success;
          }

      } // next newton iteration

      return MaxIterationsReached;
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
  Status
  linesearch(const Eigen::VectorXd &xold, Eigen::VectorXd &x, Eigen::VectorXd &dx,
                                      const double fold, double &f, const Eigen::VectorXd &nablaf,
                                      const double &stpmax)
  {

      size_t dim = x.size();

      double temp=0., test=0.;
      double rhs1=0., rhs2=0.;
      double lambda=0., lambda2=0., lambdamin=0., tmplambda=0;
      double a=0., b=0., disc=0., f2=0.;

      double norm = dx.norm();
      // scale if dx is too large
      if(norm > stpmax) dx*=(stpmax/norm);

      double slope = nablaf.adjoint()*dx;

      if (slope >= 0.) return RoundOffProblem;
      test = 0.0;
      for (size_t i=0;i<dim;i++)
      {
          temp=std::abs(dx(i))/std::max(abs(i),1);
          if (temp > test) test = temp;
      }

      lambdamin = this->parameters.TOLX/test;

      // first do newton step
      lambda   = 1.0;
      for(;;){

          x = xold+lambda*dx;

          // evaluate rate equations
          that.getREs(x,REs);

          f = 0.5*(REs.squaredNorm());

          if (lambda < lambdamin) {
             // linesearch failed
             x=xold;
             return LineSearchFailed;
          }
          else if (f <= (fold + this->parameters.ALF*lambda*slope) )
          {
              // success
              return Success;
          }
          else
          {
              if (lambda == 1.0)
                  tmplambda = -slope/(2.*(f-fold-slope));
              else {
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

      }

  }

  /**
   * Powell's dogleg method (requires the Hessian of the objective function).
   *
   * @param fold : old function value
   * @param f : new function value
   * @param xold : start point
   * @param x : end function arguments
   * @param dx : the direction
   *
   */
  Status dogleg(const Eigen::VectorXd &xold,
                double fold, const Eigen::VectorXd &nablaf,
                Eigen::VectorXd &dx,
                Eigen::VectorXd &x,
                double &delta,
                double &f)
  {

    bool check;

    const int dim = xold.size();
    Eigen::VectorXd pB, pU;
    Eigen::MatrixXd Hessian;

    // evaluate Hessian of objective function.
    that.getfHessianM(x,Hessian);

    size_t j = 0;
    do
    {
      // compute the Newton point
      pB = -nablaf;
      pB = Hessian.lu().solve(pB);

      // compute minimizer of the quadratic
      // model along the negative gradient
      pU = (-nablaf.squaredNorm()/( nablaf.transpose()*(Hessian*nablaf)) )*nablaf;

      if(pB.squaredNorm() <= delta)
      {
        dx = pB;
      }
      else
      {
        if(pU.squaredNorm() > delta)
        {
          dx = delta * pU / pU.squaredNorm();
        }
        else
        {

          Eigen::VectorXd pB_pU = pB - pU;

          Eigen::VectorXd v = 2.0*pU-pB;

          double a = pB_pU.squaredNorm();
          double b = 2.0 * v.dot(pB_pU);
          double c = v.squaredNorm() - delta*delta;

          double tau = (-b + std::sqrt(b*b-4.0*a*c)) / (2.0*a);

          dx = pU + (tau-1.0) * pB_pU;

        }
      }

      x = xold + dx;


      // update f here
      that.getREs(x,REs);
      f = 0.5*(REs.squaredNorm());

      // compute reduction
      double pred = - (nablaf.dot(dx));
             pred -=  0.5 * (dx.dot(Hessian*dx));
      double rho = (fold - f) / pred;

      // and check radius
      if(rho < 0.25)
          delta = 0.25 * dx.squaredNorm();
      else
      {
          if(rho > 0.75 && std::fabs(dx.squaredNorm() - delta) < 1e-14)
          delta = std::min(2.0 * delta, 1e6); // also the latter should be parametrized
      }
      if(rho <= 0.1) // this should be a parameter of acceptance
        check = false;
      else
        check = true;


      j++;
    }
    while(!check && j < 100); // the latter should be parametrized

    if(!check)
    {
      x = xold;
      f = fold;

      return LineSearchFailed;
    }

    return Success;
  }




};

}
}


#endif // NLESOLVE_HH
