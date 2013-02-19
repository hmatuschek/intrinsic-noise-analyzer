#ifndef __INA_NLESOLVE_HYBRIDSOLVER_HH
#define __INA_NLESOLVE_HYBRIDSOLVER_HH

#include "nlesolve/newtonraphson.hh"
#include "ode/lsoda.hh"

#include "models/ssebasemodel.hh"
#include "models/initialconditions.hh"

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

  int istate;

  double *ywork;
  double *atolwork;
  double *rtolwork;

  typedef Eval::bci::Engine<Eigen::VectorXd> LSODAengine;

  LSODAengine::Code LSODAcode;
  LSODAengine::Interpreter LSODAint;

public:

  /**
   * Constructor.
   *
   * @param system Specifies the ODE system to integrate.
   * @param epsilon_abs Specifies the absolute error for the step.
   */
  HybridSolver(Sys &system)
      : NewtonRaphson<Sys, VectorEngine, MatrixEngine>(system), LSODA(),
        istate(1)
  {

    this->parameters.maxIterations=100;

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

  virtual ~HybridSolver(){ };


  void ODEStep(Eigen::VectorXd &state, double t, double dt)
  {

      istate = 1;  // force initial call.
      lsoda(getDimension(), state.data()-1, &t, t+dt, 2, rtolwork, atolwork, 1, &istate, 0, 2);

  }

  virtual void evalODE(double t, double state[], double dx[], int nsize)
  {
      this->LSODAint.run(state,dx);
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
  solve(Eigen::VectorXd &state, double maxTime=1.e9, double dt=0.1, const Models::ParameterSet &parameters=Models::ParameterSet())
  {

      if(maxTime<dt) maxTime=dt;

      for(double t=0.;t<maxTime; t+=dt, dt*=10)
      {

          Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
          message << "Try Newton step ... ";

          Status lcheck = NewtonRaphson<Sys, VectorEngine, MatrixEngine>::solve(state);

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

          if(lcheck == Success)
          {
            return Success;
          }
          else
          {
              Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
              message << "Use integration of duration "<< dt << ".";
              Utils::Logger::get().log(message);

              if(t==0) {
                  //#pragma omp critical(ginac)
                  this->setupLSODA(parameters);
              }

              // Do ODE step of length dt
              ODEStep(state,0,dt);

          }


      } // Next Newton iteration

      return MaxIterationsReached;
  }

  void setupLSODA(const Models::ParameterSet &params)

  {

      // Generate parameter substitution table
      Trafo::excludeType ptab = this->model.makeExclusionTable(params);

      // Collect all the values of constant parameters except variable parameters
      Trafo::ConstantFolder constants(this->model, Trafo::Filter::ALL_CONST, ptab);
      Models::InitialConditions ICs(this->model,ptab);

      // Translate identifier to parameters collect variable parameters
      Models::ParameterFolder parameters(ptab);

      // Set bytecode for interpreter
      this->LSODAint.setCode(&this->LSODAcode);

      // Compile expressions
      LSODAengine::Compiler compiler(this->model.stateIndex);
      compiler.setCode(&this->LSODAcode);
      compiler.compileVector( ICs.apply(parameters.apply(constants.apply( this->model.getUpdateVector().head(this->dim) ) )) );
      compiler.finalize(0); // no need to optimize anything here.

  }

};


}
}

#endif // HYBRIDSOLVER_HH
