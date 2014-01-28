#ifndef __INA__NLESOLVE_NLESOLVER_HH
#define __INA__NLESOLVE_NLESOLVER_HH

#include "../eval/eval.hh"

namespace iNA{
namespace NLEsolve{


enum LineSearchMethod {
    NoLineSearch = 0,
    Optimization = 1,
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

    T &model;

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
     typename VectorEngine::Code ODEcode;

     /** The bytecode for the Jacobian. */
     typename MatrixEngine::Code jacobianCode;

public:

     NLEsolver(T &model)
       : model(model), dim(model.numIndSpecies()), ODEs(dim), JacobianM(dim,dim)
     {
       // Pass...
     }

     virtual ~NLEsolver()
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
      * Set ode and Jacobian code...
      */
     void set(typename MatrixEngine::Code &odeC, typename MatrixEngine::Code &jacC)
     {
       // clean up
       this->iterations = 0;

       // Set bytecode for interpreter
       this->interpreter.setCode(&odeC);
       this->jacobian_interpreter.setCode(&jacC);

     }


};

}}

#endif // NLESOLVER_HH
