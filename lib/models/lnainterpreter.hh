#ifndef __FLUC_MODELS_LNAINTERPRETER_HH
#define __FLUC_MODELS_LNAINTERPRETER_HH

#include "linearnoiseapproximation.hh"
#include "eval/eval.hh"
#include "eval/bcimp/engine.hh"

namespace Fluc {
namespace Models {

/**
 * Wraps an instance of LinearNoiseApproximation and compiles it.
 *
 * @ingroup models
 */
template < class SysEngine, class JacEngine>
class GenericLNAinterpreter
{
protected:
  /**
   * Holds a reference to an instance of LinearNoiseApproximation.
   */
   LinearNoiseApproximation &lnaModel;

   /**
    * The bytecode interpreter instance to evaluate the propensities etc.
    */
   typename SysEngine::Interpreter interpreter;

   /**
    * Holds the interpreter to evaluate the Jacobian.
    */
   typename JacEngine::Interpreter jacobian_interpreter;

   /**
    * The bytecode to interprete.
    */
   typename SysEngine::Code bytecode;

   /**
    * The bytecode to interprete.
    */
   typename JacEngine::Code jacobianCode;

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
  GenericLNAinterpreter(LinearNoiseApproximation &model, size_t opt_level,
                 size_t num_threads=OpenMP::getMaxThreads(), bool compileJac = false)
    : lnaModel(model), bytecode(num_threads), jacobianCode(num_threads),
      hasJacobian(false), opt_level(opt_level)
  {
    // Compile expressions
    typename SysEngine::Compiler compiler(model.stateIndex);
    compiler.setCode(&this->bytecode);
    compiler.compileVector(lnaModel.getUpdateVector());
    compiler.finalize(opt_level);

    // Set bytecode for interpreter
    this->interpreter.setCode(&(this->bytecode));
    this->jacobian_interpreter.setCode(&(this->jacobianCode));

    if (compileJac)
      this->compileJacobian();
  }


  /**
   * Derives and compiles the Jacobian from the ODEs.
   * If the Jacobian was allready compiled, this method does nothing.
   */
  void compileJacobian()
  {
    if(hasJacobian) return;

    // Assemble Jacobian
    Eigen::MatrixXex jacobian(lnaModel.getDimension(), lnaModel.getDimension());
    {
      std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::const_iterator it;
      for(it = lnaModel.stateIndex.begin(); it != lnaModel.stateIndex.end(); ++it)
      {
        for (size_t i=0; i<lnaModel.getDimension(); i++)
          jacobian(i,(*it).second) = lnaModel.getUpdateVector()(i).diff((*it).first);
      }
    }

    // Compile jacobian:
    typename JacEngine::Compiler jacobian_compiler(lnaModel.stateIndex);
    jacobian_compiler.setCode(&jacobianCode);
    jacobian_compiler.compileMatrix(jacobian);
    jacobian_compiler.finalize(opt_level);

    hasJacobian = true;
  }


  /**
   * Evaluates the joint ODE of the system size expansion.
   */
  template <typename T, typename U>
  inline void evaluate(const T &state, double t, U &dx) {
    this->interpreter.run(state, dx);
  }

  /**
   * Evaluates the joint ODE of the system size expansion.
   */
  template <typename T, typename U>
  inline void evaluate(const T* state, double t, U* dx) {
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
                   Eigen::MatrixXd &covariance, Eigen::VectorXd &emre) {
    this->lnaModel.fullState(state,concentrations,covariance,emre);
  }


  /**
   * Evaluates the initial state.
   */
  void getInitialState(Eigen::VectorXd &state) {
    this->lnaModel.getInitialState(state);
  }


  /**
   * Returns the dimension of the system.
   */
  size_t getDimension() {
    return this->lnaModel.getDimension();
  }

};


/**
 * Defines the default LNA interpreter using byte-code interpreter with OpenMP support (if enabled).
 */
class LNAinterpreter :
    public GenericLNAinterpreter< Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
    Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> >
{
public:
  LNAinterpreter(LinearNoiseApproximation &model, size_t opt_level,
                 size_t num_threads=OpenMP::getMaxThreads(), bool compileJac = false)
    : GenericLNAinterpreter< Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
      Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(model, opt_level, num_threads, compileJac)
  {
    // Pass...
  }
};


}
}

#endif // __FLUC_MODELS_LNAWRAPPER_HH
