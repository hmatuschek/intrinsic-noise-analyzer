#ifndef __FLUC_ODE_ODEMODEL_HH__
#define __FLUC_ODE_ODEMODEL_HH__

#include "eval/eval.hh"

namespace iNA {
namespace ODE {


/**
 * Convenience class to implement a time-independent system of ordinary differential equations
 * being evaluated using the default execution engine @see eval.
 *
 * This class implements the ODE interface expected by all intergrator steps like
 * @c EulerDriver, @c RungeKutta4, @c RKF45, @c RosenbrockTimeInd or @c SemiImplicitEuler. It also
 * derives the jacobian of the system analytically used by the implicit methods like
 * @c RosenbrockTimeInd and @c SemiImplicitEuler.
 *
 * The following example shows how to implement a Lorenz attractor using the ODE model:
 * @code
 * // Some constants:
 * double a=10, b=28, c=8./3;
 * // Allocate vector of symbols:
 * std::vector<GiNaC::symbol> symbols(3);
 * symbols[0] = GiNaC::symbol("x");
 * symbols[1] = GiNaC::symbol("y");
 * symbols[3] = GiNaC::symbol("z");
 * // Allocate vector of expressions:
 * Eigen::VectorXex odes(3);
 *
 * // Assemble expressions:
 * odes(0) = a*(symbols(1) - symbols(0));
 * odes(1) = symbols(0)*(a - symbols(2)) - symbols(1);
 * odes(2) = symbols(0)*symbols(1) - c*symbols(2);
 *
 * // Construct system:
 * ODE::TimeIndODEModel ode(symbols, odes);
 *
 * ODE::RKF45<ODE::TimeIndODEModel> stepper(ode, dt, 1e-5, 1e-6);
 * @endcode
 *
 * @ingroup ode
 */
class TimeIndepODEModel
{
protected:
  /**
   * Holds the number of ODEs (ie. system-dimension).
   */
  size_t numODEs;

  /**
   * Holds the compiled code of the ODEs.
   */
  Eval::bci::Code ode_code;

  /**
   * Holds the compiled code of the Jacobian.
   */
  Eval::bci::Code jacobian_code;

  /**
   * Holds the interpreter to evaluate the ODEs.
   */
  Eval::bci::Interpreter<Eigen::VectorXd, Eigen::VectorXd> ode_interpreter;

  /**
   * Holds the interpreter to evaluate the Jacobian.
   */
  Eval::bci::Interpreter<Eigen::VectorXd, Eigen::MatrixXd> jacobian_interpreter;


public:
  /**
   * Constructs an ODE model, performs analytic deviation of the system Jacobian.
   *
   * @param symbols Specifies the state-vector as a vector of variable-symbols, must not match
   *        dimension of @c odes.
   * @param odes Specifies the vector of ODE expressions using symbols defined in @c symbols.
   * @param opt_level Passed to the execution-engine compiler to specify the optimization level to
   *        perform on the compiled code.
   */
  TimeIndepODEModel(const std::vector<GiNaC::symbol> &symbols, Eigen::VectorXex &odes,
                    size_t opt_level=0);

  /**
   * Constructs an ODE model, but does not perform an automatic deviation of the system Jacobian.
   */
  TimeIndepODEModel(const std::vector<GiNaC::symbol> &symbols, Eigen::VectorXex &odes,
                    Eigen::MatrixXex &jacobian, size_t opt_level);

  /**
   * Evaluates the ODEs at the given state.
   */
  inline void evaluate(const Eigen::VectorXd &state, double t, Eigen::VectorXd &diff)
  {
    this->ode_interpreter.run(state, diff);
  }

  /**
   * Evaluates the Jacobian of the ODEs at the given state.
   */
  inline void evaluateJacobian(const Eigen::VectorXd &state, double t, Eigen::MatrixXd &jacobian)
  {
    this->jacobian_interpreter.run(state, jacobian);
  }

  /**
   * Retuns the dimension of the ODE system.
   */
  inline size_t getDimension()
  {
    return this->numODEs;
  }
};


}
}

#endif // ODEMODEL_HH
