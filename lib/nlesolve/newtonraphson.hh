#ifndef __FLUC_MODELS_NEWTONRAPHSON_HH
#define __FLUC_MODELS_NEWTONRAPHSON_HH

#include "eval/eval.hh"
#include "trafo/constantfolder.hh"

namespace iNA {
namespace NLEsolve {


inline double
maxNorm(const Eigen::VectorXd &vector)

{
    return vector.lpNorm<Eigen::Infinity>();
}


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


template<typename T>
class NLEsolver
{



protected:

    size_t dim;

    Eigen::VectorXd ODEs;
    Eigen::MatrixXd JacobianM;

     /**
     * The bytecode interpreter instance to evaluate the ODEs.
     */
     size_t iterations;

     /**
      * The bytecode interpreter instance to evaluate the ODEs.
      */
     Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Interpreter interpreter;

     /**
      * Holds the interpreter to evaluate the Jacobian.
      */
     Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Interpreter jacobian_interpreter;

     /**
      * The bytecode for the ODE.
      */
     Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Code ODEcode;

     /**
      * The bytecode for the Jacobian.
      */
     Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Code jacobianCode;

public:

     NLEsolver(T &model)
         : dim(model.numIndSpecies()), ODEs(dim), JacobianM(dim,dim)
     {

     }


     /**
      * Returns the reduced dimension of the ODE system.
      */
     inline size_t
     getDimension()
     {
        return this->dim;
     }

     /**
      * Returns no. of iterations...
      */
     int getIterations()
     {
         return this->iterations;
     }

     /**
      * Every solver has to implement its own method...
      */
     virtual Status solve(Eigen::VectorXd &state)=0;

     /**
      * Sets the ODE and Jacobian code...
      */

     void set(std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &indexTable,
              Eigen::VectorXex &updateVector, Eigen::MatrixXex &Jacobian)
     {

         // clean up
         this->iterations = 0;
         this->ODEcode.clear();
         this->jacobianCode.clear();

         // Compile expressions
         Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Compiler compiler(indexTable);
         compiler.setCode(&this->ODEcode);
         compiler.compileVector(updateVector);
         //std::cerr << updateVector;
         compiler.finalize(0);

         // Set bytecode for interpreter
         this->interpreter.setCode(&(this->ODEcode));
         this->jacobian_interpreter.setCode(&(this->jacobianCode));

         // Compile jacobian:
         Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Compiler jacobian_compiler(indexTable);
         jacobian_compiler.setCode(&jacobianCode);
         jacobian_compiler.compileMatrix(Jacobian);
         jacobian_compiler.finalize(0);

     }

     /**
      * Sets the ODE and Jacobian code...
      */
     void set(Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Code ODEcode, Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Code jacobianCode)
    {

         // clean up
         this->iterations = 0;
         this->ODEcode.clear();
         this->jacobianCode.clear();

         // Set bytecode for interpreter
         this->interpreter.setCode(&(ODEcode));
         this->jacobian_interpreter.setCode(&(jacobianCode));

    }


};

/**
 * Nonlinear algebraic equation solver using the Newton-Raphson method with linesearch.
 * @ingroup nlesolve
 */
template<typename T>
class NewtonRaphson
    : public NLEsolver<T>
{
protected:

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
      : NLEsolver<T>(model),
        parameters(model.numIndSpecies())

  {

      // Pass...

  }

  /**
   * Constructor...
   */

  NewtonRaphson(T &model, Eigen::VectorXex &updateVector, Eigen::MatrixXex &Jacobian)
      : NLEsolver<T>(model),
        parameters(model.numIndSpecies())

  {
      this->set(model.stateIndex, updateVector, Jacobian);
  }

  const Eigen::MatrixXd&
  getJacobianM()

  {
     return this->JacobianM;
  }

  virtual Status
  solve(Eigen::VectorXd &conc)

  {

      double test,temp;

      Eigen::VectorXd conc_old;

      Eigen::VectorXd nablaf;
      Eigen::VectorXd dx;

      // Dimension
      size_t dim = conc.size();

      // Calculate maximum step size heuristic
      const double stpmax=this->parameters.STPMX*std::max(conc.norm(),double(dim));

      // Do Newton iteration
      for(this->iterations=1;this->iterations<parameters.maxIterations;this->iterations++)
      {

          conc_old = conc;

          LineSearchStatus lcheck = newtonStep(conc_old,conc,stpmax);

          if ((conc.array()<0).any())
          {
              conc = conc_old;
              return NegativeValues;
          }

          // Test for convergence of ODEs
          if ( maxNorm(this->ODEs) < this->parameters.TOLF )
          {
             return Success;
          }

          // Check linesearch
          switch(lcheck)
          {
            case Converged:
              return Success;
            case RoundOffProblem:
            case LineSearchFailed:
              conc = conc_old;
              return IterationFailed;
            default: break;
          }

          // Test for convergence of update vector dx
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
  newtonStep(const Eigen::VectorXd &inState, Eigen::VectorXd &outState ,double stpmax)

  {

      double f,fold;

      double test,temp,den;

      size_t dim = inState.size();

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
      dx = precisionSolve(this->JacobianM, -this->ODEs);

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
      for(int i=0; i<outState.size(); i++)
        if(std::isnan(outState(i))) return LineSearchFailed;

      // Check for spurious convergence of nablaf = 0
      if (lcheck==LineSearchFailed) {
         test = 0.0;
         den = std::max(f,0.5*double(dim));
         for(size_t i=0; i<dim;i++)
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
   * Simple inline function that attempts to find a solution within the precision of the NLE solver (advantageous for stiff systems).
   */

  inline Eigen::VectorXd precisionSolve(const Eigen::MatrixXd &B, const Eigen::VectorXd &A)
  {

      // this is fast
      Eigen::VectorXd x = B.lu().solve(A);
      if((B*x).isApprox(A,parameters.epsilon))
      {
         // this is rather slow
         Eigen::FullPivLU<Eigen::MatrixXd> LU(B);
         x = LU.solve(A);
      }

      return x;

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
      for(int it=0;;it++){

          x = xold+lambda*dx;

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

          if(it>100000)return LineSearchFailed;

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
