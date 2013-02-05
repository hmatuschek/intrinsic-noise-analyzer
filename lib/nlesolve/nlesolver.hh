#ifndef __INA__NLESOLVE_NLESOLVER_HH
#define __INA__NLESOLVE_NLESOLVER_HH

#include "eval/eval.hh"

namespace iNA{
namespace NLEsolve{


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

template<class T,
         class VectorEngine=Eval::bci::Engine<Eigen::VectorXd>,
         class MatrixEngine=Eval::bci::Engine<Eigen::VectorXd,Eigen::MatrixXd> >
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
     typename VectorEngine::Interpreter interpreter;

     /**
      * Holds the interpreter to evaluate the Jacobian.
      */
     typename MatrixEngine::Interpreter jacobian_interpreter;

     /** The bytecode for the ODE. */
     typename VectorEngine::Code *ODEcode;

     /** The bytecode for the Jacobian. */
     typename MatrixEngine::Code *jacobianCode;

public:

     NLEsolver(T &model)
       : dim(model.numIndSpecies()), ODEs(dim), JacobianM(dim,dim), ODEcode(0), jacobianCode(0)
     {
       // Pass...
     }

     virtual ~NLEsolver(){
       //if (0 != ODEcode) { delete ODEcode; }
       //if (0 != jacobianCode) { delete jacobianCode; }
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
              Eigen::VectorXex &updateVector, Eigen::MatrixXex &Jacobian, size_t opt_level=0)
     {
       // clean up
       this->iterations = 0;
       if (0 != ODEcode) { delete ODEcode; }
       if (0 != jacobianCode) { delete jacobianCode; }

       this->ODEcode = new typename VectorEngine::Code();
       this->jacobianCode = new typename MatrixEngine::Code();

       // Set bytecode for interpreter
       this->interpreter.setCode(this->ODEcode);
       this->jacobian_interpreter.setCode(this->jacobianCode);

       // Compile expressions
       typename VectorEngine::Compiler compiler(indexTable);
       compiler.setCode(this->ODEcode);
       compiler.compileVector(updateVector);
       compiler.finalize(opt_level);

       // Compile jacobian:
       typename MatrixEngine::Compiler jacobian_compiler(indexTable);
       jacobian_compiler.setCode(jacobianCode);
       jacobian_compiler.compileMatrix(Jacobian);
       jacobian_compiler.finalize(opt_level);
     }

     /**
      * Sets the ODE and Jacobian code...
      * @bug This does not work for the JIT compiler.
      */
     void set(typename VectorEngine::Code &ODEcode, typename MatrixEngine::Code &jacobianCode)
     {
       // clean up
       this->iterations = 0;

       if (0 != this->ODEcode) { delete this->ODEcode; }
       if (0 != this->jacobianCode) { delete this->jacobianCode; }

       // Set bytecode for interpreter
       interpreter.setCode(&(ODEcode));
       jacobian_interpreter.setCode(&(jacobianCode));
     }


};

}}

#endif // NLESOLVER_HH
