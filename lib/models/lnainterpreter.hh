#ifndef __FLUC_MODELS_LNAINTERPRETER_HH
#define __FLUC_MODELS_LNAINTERPRETER_HH

#include "linearnoiseapproximation.hh"
#include "eval/eval.hh"
#include "eval/bcimp/bcimp.hh"

namespace Fluc {
namespace Models {

/**
 * Wraps an instance of LinearNoiseApproximation and compiles it.
 *
 * @ingroup models
 */
class LNAinterpreter
{
protected:
  /**
   * Holds a reference to an instance of LinearNoiseApproximation.
   */
   LinearNoiseApproximation &lnaModel;

   /**
    * The bytecode interpreter instance to evaluate the propensities etc.
    */
   Evaluate::bcimp::Interpreter<Eigen::VectorXd> interpreter;

   /**
    * Holds the interpreter to evaluate the Jacobian.
    */
   Evaluate::bcimp::Interpreter<Eigen::VectorXd, Eigen::MatrixXd> jacobian_interpreter;

   /**
    * The bytecode to interprete.
    */
   Evaluate::bcimp::Code bytecode;

   /**
    * The bytecode to interprete.
    */
   Evaluate::bcimp::Code jacobianCode;

   /**
   * If true, the Jacobian was allready compiled.
   */
   bool hasJacobian;

   /**
    * Holds the optimization level for the generic compiler.
    */
   size_t opt_level;


public:
  /**
   * Constructor.
   *
   * @param model Specifies the LNA model to integrate.
   * @param opt_level Specifies the code-optimization level.
   * @param num_threads Specifies the (optional) number of threads to use to evaluate the
   *        system. By default, @c OpenMP::getMaxThreads will be used.
   * @param compileJac Specifies if the Jacobian should be compiled immediately. If false, it will
   *        be compiled on demand.
   */
  LNAinterpreter(LinearNoiseApproximation &model, size_t opt_level,
                 size_t num_threads=OpenMP::getMaxThreads(), bool compileJac = false);

  /**
   * Derives and compiles the Jacobian from the ODEs.
   * If the Jacobian was allready compiled, this method does nothing.
   */
  void compileJacobian();

  /**
   * Evaluates the joint ODE of the system size expansion.
   */
  template <typename T, typename U>
  inline void evaluate(const T &state, double t, U &dx)
  {
    this->interpreter.run(state, dx);
  }

  /**
   * Evaluates the joint ODE of the system size expansion.
   */
  template <typename T, typename U>
  inline void evaluate(const T* state, double t, U* dx)
  {
    this->interpreter.run(state, dx);
  }

  /**
   * Evaluates the Jacobian of the ODEs at the given state.
   */
  inline void evaluateJacobian(const Eigen::VectorXd &state, double t, Eigen::MatrixXd &jacobian)
  {
    // ensures that the Jacobian was compiled
    if (! hasJacobian) {
      compileJacobian();
    }

    // Evaluate the Jacobian
    this->jacobian_interpreter.run(state, jacobian);
  }

  /**
   * Evaluates the Jacobian of the ODEs at the given state.
   */
  template <typename T, typename U>
  inline void evaluateJacobian(const T* state, double t, U* jac)
  {
      // ensures that the Jacobian was compiled
      if (! hasJacobian) {
        compileJacobian();
      }

      // Evaluate the Jacobian
      this->jacobian_interpreter.run(state, jac);
  }

  /**
   * Constructs the "full" state from the internal, reduced state.
   */
  void full_state( const Eigen::VectorXd &state, Eigen::VectorXd &concentrations,
                   Eigen::MatrixXd &covariance, Eigen::VectorXd &emre);

  /**
   * Evaluates the initial state.
   */
  void getInitialState(Eigen::VectorXd &state);

  /**
   * Returns the dimension of the system.
   */
  size_t getDimension();
};

}
}

#endif // __FLUC_MODELS_LNAWRAPPER_HH
