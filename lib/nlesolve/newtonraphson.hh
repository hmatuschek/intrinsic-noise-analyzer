#ifndef __FLUC_MODELS_NEWTONRAPHSON_HH
#define __FLUC_MODELS_NEWTONRAPHSON_HH

namespace Fluc {
namespace NLEsolve {

    enum LineSearchMethod {
        NoLineSearch = 0,
        Damped = 1,
        Optimization = 2,
    };

    enum Status {
        Success = 1,
        MaxIterationsReached = 0,
        IterationFailed = -1,
        NegativeValues = -2,
    };

    enum LineSearchStatus {
        Done = 1,
        Converged = 2,
        LineSearchFailed = 0,
        RoundOffProblem = -1,
    };


/**
 * Nonlinear algebraic equation solver using the Newton-Raphson method with linesearch.
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
       params(size_t dim) : linesearch(Optimization), maxIterations(100*(dim+1)), epsilon(1.e-9),
           ALF(1.e-4), TOLX(std::numeric_limits<double>::epsilon()), TOLF(epsilon), TOLMIN(1.e-12), STPMX(100.){}

       LineSearchMethod linesearch;

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

      double test,temp;

      Eigen::VectorXd conc_old;
      Eigen::VectorXd rhs;

      Eigen::VectorXd nablaf;
      Eigen::VectorXd dx;

      // evaluate rate equations
      that.getREs(conc,REs);

      // dimension
      size_t dim = conc.size();

      // calc max step
      const double stpmax=this->parameters.STPMX*std::max(conc.norm(),double(dim));

      for(this->iterations=1;this->iterations<this->parameters.maxIterations;this->iterations++)
      {

          // evaluate rate equations
          that.getREs(conc,REs);

          conc_old = conc;

          LineSearchStatus lcheck = newtonStep(conc_old,conc,stpmax);

          if (!(conc.array()>0).all())
          {
              conc = conc_old;
              return NegativeValues;
          }

          // test for convergence of REs
          if ( REs.lpNorm<Eigen::Infinity>() < this->parameters.TOLF)
          {
             return Success;
          }

          // check linesearch
          switch(lcheck)
          {
            case Converged: return Success;
            case RoundOffProblem :
              conc = conc_old; return IterationFailed;
            default: break;
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
              return Success;
          }

      } // next newton iteration

      return MaxIterationsReached;
  }

  LineSearchStatus
  newtonStep(const Eigen::VectorXd &inState, Eigen::VectorXd &outState,double stpmax)
  {

      double f,fold;

      double test,temp,den;

      size_t dim = inState.size();

      Eigen::VectorXd nablaf;
      Eigen::VectorXd dx;

      // construct Jacobian matrix
      that.getREs(inState,REs);
      that.getJacobianMatrix(inState,JacobianM);

      // compute f to minimize
      f = .5*(REs.squaredNorm());
      // calculate steepest descent direction
      nablaf = JacobianM.transpose()*REs;

      // store also old f
      fold = f;

      // solve JacobianM*dx=-REs
      //dx = JacobianM.fullPivLu().solve(-REs);
      dx = JacobianM.lu().solve(-REs);

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
      //< returns new outState and f, also updates REs

      // check for spurious convergence of nablaf = 0
      if (lcheck==LineSearchFailed) {
         test = 0.0;
         den = std::max(f,0.5*double(dim));
         for(size_t i=0; i<dim;i++)
         {
             temp = std::abs(nablaf(i))*std::max(std::abs(outState(i)),1.)/den;
             if (temp > test) test = temp;
         }

         if(test < this->parameters.TOLMIN)
             return LineSearchFailed;
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

      size_t dim = xold.size();

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

          this->that.getREs(x,this->REs);

          f = 0.5*this->REs.squaredNorm();

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


#endif // NLESOLVE_HH
