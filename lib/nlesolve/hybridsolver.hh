#ifndef HYBRIDSOLVER_HH
#define HYBRIDSOLVER_HH

#include "nlesolve/newtonraphson.hh"
#include "ode/lsoda.hh"

namespace Fluc{
namespace NLEsolve{
/**
 * A hybrid of the ODE integrator LSODA and the Newton-Raphson method for nonlinear algebraic equations.
 *
 * @ingroup nlesolve
 */
template <class Sys>
class HybridSolver
    : public NewtonRaphson<Sys>,
      protected ODE::LSODA
{


protected:

  double *ywork;
  double *atolwork;
  double *rtolwork;

public:
  /**
   * Constructor.
   *
   * @param system Specifies the ODE system to integrate.
   * @param epsilon_abs Specifies the absolute error for the step.
   */
  HybridSolver(Sys &system)
      : NewtonRaphson<Sys>(system), LSODA()
  {

    //parameters.epsilon=epsilon;

    this->parameters.maxIterations=50;

    istate=1;

    ywork = new double[3 * (getDimension() + 1)];
    atolwork = ywork + getDimension() + 1;
    rtolwork = atolwork + getDimension() + 1;

    /* @todo use to define staged errors using the system size */
    for (size_t i = 1; i <= getDimension(); ++i)
    {
        rtolwork[i] = this->parameters.epsilon;
        atolwork[i] = this->parameters.epsilon;
    }
 }

  void ODEStep(Eigen::VectorXd &state, double t, double dt)
  {
      lsoda(getDimension(), state.data()-1, &t, t+dt, 2, rtolwork, atolwork, 1, &istate, 0, 2);
  }

  virtual void evalODE(double t, double y[], double yd[], int nsize)
  {
      Eigen::VectorXd REs;
      this->that.getREs(Eigen::Map<Eigen::VectorXd>(y,nsize),REs);
      for(int i=0;i<nsize; i++)
        yd[i]=REs(i);
  }

  virtual void evalJac(double t, double *y, double **jac, int nsize)
  {
      // @todo implement.
  }

public:

  /**
   * Returns the reduced dimension of the ODE system.
   */
  inline size_t getDimension()
  {
    return this->that.numIndSpecies();
  }

  Status
  solve(Eigen::VectorXd &conc, double dt=0.1)
  {

      double test,temp;

      Eigen::VectorXd conc_old;
      Eigen::VectorXd rhs;

      Eigen::VectorXd nablaf;
      Eigen::VectorXd dx;

      // evaluate rate equations
      this->that.getREs(conc,rhs);

      // dimension
      size_t dim = conc.size();

      for(size_t k=0;k<this->parameters.maxIterations; k++, dt*=10)
      {

          conc_old = conc;

          std::cerr << "Try Newton step"<< std::endl;
          Status lcheck = NewtonRaphson<Sys>::solve(conc);
          std::cerr << "Iterations: "<< this->getIterations() << std::endl;
          if(lcheck==LineSearchFailed) std::cerr << "Linesearch failed." << std::endl;

          if(lcheck!=Success || !(conc.array()>0).all()){
              if(!(conc.array()>0).all()) std::cerr << "Negative concentrations encountered." << std::endl;
              std::cerr << "Integration step: "<<dt<< std::endl;
              conc=conc_old;
              ODEStep(conc,0,dt);
          }

          // test for convergence of derivatives
          test = 0.;
          for(size_t i=0;i<dim;i++)
          {
              temp = std::abs(this->REs(i));
              if (temp > test) test = temp;
          }
          if ( test < this->parameters.TOLF)
          {
             // do additional Newton step?
             this->iterations=k+1;
             return Success;
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

};


}
}

#endif // HYBRIDSOLVER_HH
