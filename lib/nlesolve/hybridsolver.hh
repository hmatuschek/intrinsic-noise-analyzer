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

    this->parameters.maxIterations=100;

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

  virtual void evalODE(double t, double state[], double dx[], int nsize)
  {
      this->interpreter.run(state,dx);
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

  /**
   * Runs the solver.
   */
  Status
  solve(Eigen::VectorXd &conc, double maxTime=1.e9)
  {

      double dt = 0.1;
      double test,temp;

      Eigen::VectorXd conc_old;

      Eigen::VectorXd nablaf;
      Eigen::VectorXd dx;

      // dimension
      size_t dim = conc.size();

      if(maxTime<1.e-1) maxTime=1.e-1;


      Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);

      for(double t=0.;t<maxTime; t+=dt, dt*=10)
      {

          conc_old = conc;

          message << "Try Newton step ..."<< std::endl;
          Status lcheck = NewtonRaphson<Sys>::solve(conc);

          message.clear();
          switch(lcheck)
          {
            case IterationFailed:
              message << "Newton step: Linesearch failed." << std::endl; break;
            case NegativeValues:
              message << "Newton step: Negative concentrations encountered." << std::endl;  break;
            case MaxIterationsReached:
              message << "Newton step: Maximum iterations reached." << std::endl; break;
            case Success:
              message << "Newton step converged in " << this->getIterations() << "." << std::endl; break;
          }
          Utils::Logger::get().log(message);

          if(lcheck!=Success){
              message.clear();
              message << "Use integration of duration "<< dt << "." << std::endl;
              ODEStep(conc,0,dt);             
              Utils::Logger::get().log(message);
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
             NewtonRaphson<Sys>::solve(conc);
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
              NewtonRaphson<Sys>::solve(conc);
              return Success;
          }




      } // next newton iteration

      return MaxIterationsReached;
}

};


}
}

#endif // HYBRIDSOLVER_HH
