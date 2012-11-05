#ifndef __INA_MODELS_LNAINTERPRETER_HH
#define __INA_MODELS_LNAINTERPRETER_HH

#include "REmodel.hh"
#include "LNAmodel.hh"
#include "IOSmodel.hh"
#include "eval/eval.hh"
#include "eval/bcimp/engine.hh"
#include "trafo/constantfolder.hh"

namespace iNA {
namespace Models {


/** This class defines the virtual base class of all interpreters. This is necessary to allow
 * the determination of the execution engine at runtime. This class does not define any methods
 * to be implemented by the @c GenericSSEinterpreter, it just forces a RTTI vtable for all
 * interpreters.
 */
class SSEInterpreterInterface {
public:
  virtual ~SSEInterpreterInterface();
};



/**
 * Wraps an instance of SSE model and compiles it.
 *
 * @ingroup models
 */
template <class Sys, class SysEngine, class JacEngine>
class GenericSSEinterpreter : public SSEInterpreterInterface
{
protected:
   /**
   * Holds a reference to an instance of LinearNoiseApproximation.
   */
   Sys &sseModel;

   /**
   * Holds the initial conditions.
   */
   InitialConditions ICs;

   /**
    * The bytecode interpreter instance to evaluate the ODEs.
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


   /**
    * Holds the update vector with constants folded.
    */
   Eigen::VectorXex updateVector;


public:
  /**
   * Constructor.
   *
   * @param model Specifies the SSE model to integrate.
   * @param opt_level Specifies the code-optimization level.
   * @param num_threads Specifies the (optional) number of threads to use to evaluate the
   *        system. By default, @c OpenMP::getMaxThreads will be used.
   * @param compileJac Specifies if the Jacobian should be compiled immediately. If false, it will
   *        be compiled on demand.
   */

  GenericSSEinterpreter(Sys &model, size_t opt_level,
                 size_t num_threads=OpenMP::getMaxThreads(), bool compileJac = false)
    : sseModel(model), ICs(model), bytecode(num_threads), jacobianCode(num_threads),
      hasJacobian(false), opt_level(opt_level)

  {

    // Fold constants and get update vector
    Trafo::ConstantFolder constants(sseModel);
    updateVector = constants.apply(sseModel.getUpdateVector());

    // Account for conservation laws
    updateVector = ICs.apply(updateVector);

    // Compile expressions
    typename SysEngine::Compiler compiler(sseModel.stateIndex);
    compiler.setCode(&this->bytecode);
    compiler.compileVector(updateVector);
    compiler.finalize(opt_level);

    // Set bytecode for interpreter
    this->interpreter.setCode(&(this->bytecode));
    this->jacobian_interpreter.setCode(&(this->jacobianCode));

    if (compileJac)
      this->compileJacobian();
  }


  /**
   * Derives and compiles the Jacobian from the ODEs.
   * If the Jacobian was already compiled, this method does nothing.
   */

  void compileJacobian()

  {
    if(hasJacobian) return;

    // Assemble Jacobian
    Eigen::MatrixXex jacobian(sseModel.getDimension(), sseModel.getDimension());
    {
      std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::const_iterator it;
      for(it = sseModel.stateIndex.begin(); it != sseModel.stateIndex.end(); ++it)
      {
        for (size_t i=0; i<sseModel.getDimension(); i++)
          jacobian(i,(*it).second) = updateVector(i).diff((*it).first);
      }
    }

    // Compile jacobian:
    typename JacEngine::Compiler jacobian_compiler(sseModel.stateIndex);
    jacobian_compiler.setCode(&jacobianCode);
    jacobian_compiler.compileMatrix(jacobian);
    jacobian_compiler.finalize(opt_level);

    hasJacobian = true;
  }


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
   * Evaluates the initial state.
   */

  void getInitialState(Eigen::VectorXd &state)

  {
      sseModel.getInitialState(state);
  }


  /**
   * Returns the dimension of the system.
   */

  size_t getDimension()

  {
    return this->sseModel.getDimension();
  }

};


/**
 * Defines the default SSE interpreter using byte-code interpreter with OpenMP support (if enabled).
 */
template <class Sys>
class SSEinterpreter :
    public GenericSSEinterpreter< Sys ,Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
    Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> >
{
public:
  SSEinterpreter(Sys &model, size_t opt_level,
                 size_t num_threads=OpenMP::getMaxThreads(), bool compileJac = false)
    : GenericSSEinterpreter<Sys, Eval::bcimp::Engine<Eigen::VectorXd, Eigen::VectorXd>,
      Eval::bcimp::Engine<Eigen::VectorXd, Eigen::MatrixXd> >(model, opt_level, num_threads, compileJac)
  {
    // Pass...
  }
};

typedef SSEinterpreter<IOSmodel> IOSinterpreter;
typedef SSEinterpreter<LNAmodel> LNAinterpreter;
typedef SSEinterpreter<REmodel> REinterpreter;


}
}

#endif // __FLUC_MODELS_LNAWRAPPER_HH
