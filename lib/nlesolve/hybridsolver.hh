#ifndef __INA_NLESOLVE_HYBRIDSOLVER_HH
#define __INA_NLESOLVE_HYBRIDSOLVER_HH

#include "nlesolve/newtonraphson.hh"
#include "ode/lsoda.hh"

namespace iNA{
namespace NLEsolve{

/**
 * A hybrid of the ODE integrator LSODA and the Newton-Raphson method for nonlinear algebraic equations.
 *
 * @ingroup nlesolve
 */
template <class Sys,
          class VectorEngine=Eval::bci::Engine<Eigen::VectorXd>,
          class MatrixEngine=Eval::bci::Engine<Eigen::VectorXd,Eigen::MatrixXd> >
class HybridSolver
    : public NewtonRaphson<Sys, VectorEngine, MatrixEngine>,
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
      : NewtonRaphson<Sys, VectorEngine, MatrixEngine>(system), LSODA()
  {

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

  /**
   * Constructor.
   *
   * @param system Specifies the ODE system to integrate.
   * @param epsilon_abs Specifies the absolute error for the step.
   */
  HybridSolver(Sys &system, Eigen::VectorXex &update, Eigen::MatrixXex &Jacobian)
      : NewtonRaphson<Sys>(system, update, Jacobian), LSODA()
  {

    this->parameters.maxIterations=100;

    istate=1;

    ywork = new double[3 * (getDimension() + 1)];
    atolwork = ywork + getDimension() + 1;
    rtolwork = atolwork + getDimension() + 1;

    for (size_t i = 1; i <= getDimension(); ++i)
    {
        rtolwork[i] = this->parameters.epsilon;
        atolwork[i] = this->parameters.epsilon;
    }
  }

  virtual ~HybridSolver(){ };


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

  size_t getDimension()
  {
    return NLEsolver<Sys, VectorEngine, MatrixEngine>::getDimension();
  }

  /**
   * Runs the solver.
   */
  Status
  solve(Eigen::VectorXd &conc, double maxTime=1.e9, double dt=0.1)
  {

      if(maxTime<dt) maxTime=dt;

      for(double t=0.;t<maxTime; t+=dt, dt*=10)
      {

          Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
          message << "Try Newton step ... ";

          Status lcheck = NewtonRaphson<Sys, VectorEngine, MatrixEngine>::solve(conc);

          switch(lcheck)
          {
            case IterationFailed:
              message << " Linesearch failed."; break;
            case NegativeValues:
              message << " Negative concentrations encountered.";  break;
            case MaxIterationsReached:
              message << " Maximum iterations reached."; break;
            case Success:
              message << "Converged in " << this->getIterations() << " iterations."; break;
          }

          Utils::Logger::get().log(message);

          if(lcheck==Success)
          {
            return Success;

          }
          else
          {
              Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
              message << "Use integration of duration "<< dt << ".";
              Utils::Logger::get().log(message);

              // Do ODE step of length dt
              ODEStep(conc,0,dt);

          }


      } // Next Newton iteration

      return MaxIterationsReached;
}

};


}
}

#endif // HYBRIDSOLVER_HH
