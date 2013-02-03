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
              Eigen::VectorXex &updateVector, Eigen::MatrixXex &Jacobian, size_t opt_level=0)
     {

         // clean up
         this->iterations = 0;
         this->ODEcode.clear();
         this->jacobianCode.clear();

         // Compile expressions
         Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Compiler compiler(indexTable);
         compiler.setCode(&this->ODEcode);
         compiler.compileVector(updateVector);
         compiler.finalize(opt_level);

         // Set bytecode for interpreter
         this->interpreter.setCode(&(this->ODEcode));
         this->jacobian_interpreter.setCode(&(this->jacobianCode));

         // Compile jacobian:
         Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Compiler jacobian_compiler(indexTable);
         jacobian_compiler.setCode(&jacobianCode);
         jacobian_compiler.compileMatrix(Jacobian);
         jacobian_compiler.finalize(opt_level);

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

}}

#endif // NLESOLVER_HH
