#ifndef __INA_MODELS_STEADYSTATEANALYSIS_HH
#define __INA_MODELS_STEADYSTATEANALYSIS_HH

#include "../nlesolve/nlesolve.hh"
#include "IOSmodel.hh"
#include "../math.hh"

namespace iNA {
namespace Models {

/**
* Performs the Steady State Analysis on a model.
*/

template <class M,
          class VectorEngine = Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          class MatrixEngine = Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> >
class SteadyStateAnalysis
{
protected:
    /**
     * Holds a reference to a SSE model.
     */

    M &sseModel;

    typename VectorEngine::Code codeODE;
    typename MatrixEngine::Code codeJac;

    /**
     * An instance of a nonlinear solver.
     */
    NLEsolve::HybridSolver<M, VectorEngine, MatrixEngine> solver;

    /**
     * Holds maximum the maximum integrate time.
     */
    double max_time;

    /**
     * Holds the minimum time step.
     */
    double min_time_step;

public:

    /**
    * Constructor
    */
    SteadyStateAnalysis(M &model)
      : sseModel(model), solver(model), max_time(1e9), min_time_step(1e-1)

    {

        // Fold all constants and get update vector
        Trafo::ConstantFolder constants(model);
        InitialConditions ICs(model);

        Eigen::VectorXex REs = ICs.apply(constants.apply( model.getUpdateVector().head(model.numIndSpecies())) );
        Eigen::MatrixXex Jac = ICs.apply(constants.apply( model.getJacobian() ));

        // Compile ODEs
        typename VectorEngine::Compiler compilerA(model.stateIndex);
        compilerA.setCode(&codeODE);
        compilerA.compileVector(REs);
        compilerA.finalize(0);

        // Compile Jacobian
        typename MatrixEngine::Compiler compilerB(model.stateIndex);
        compilerB.setCode(&codeJac);
        compilerB.compileMatrix(Jac);
        compilerB.finalize(0);

        solver.set(codeODE, codeJac);

    }

    /**
    * Constructor
    */
    SteadyStateAnalysis(M &model, size_t iter, double epsilon, double t_max=1e9, double dt=1e-1)
      : sseModel(model), solver(model), max_time(t_max), min_time_step(dt)

    {

        // Fold all constants and get update vector
        Trafo::ConstantFolder constants(model);
        InitialConditions ICs(model);

        Eigen::VectorXex REs = ICs.apply(constants.apply( model.getUpdateVector().head(model.numIndSpecies())) );
        Eigen::MatrixXex Jac = ICs.apply(constants.apply( model.getJacobian() ));

        // Compile ODEs
        typename VectorEngine::Compiler compilerA(model.stateIndex);
        compilerA.setCode(&codeODE);
        compilerA.compileVector(REs);
        compilerA.finalize(0);

        // Compile Jacobian
        typename MatrixEngine::Compiler compilerB(model.stateIndex);
        compilerB.setCode(&codeJac);
        compilerB.compileMatrix(Jac);
        compilerB.finalize(0);

        solver.set(codeODE, codeJac);

        this->setPrecision(epsilon);
        this->setMaxIterations(iter);
    }

    /**
    * Set the precision of the root finding algorithm
    */
    void setPrecision(const double &epsilon)

    {
        this->solver.parameters.epsilon = epsilon;
    }

    /**
    * Set the maximum number of iterations used by the root finding algorithm
    */
    void setMaxIterations(const size_t &maxiter)

    {
          this->solver.parameters.maxIterations = maxiter;
    }

    /**
     * Solves rate equations for steady state concentrations in @c conc and returns the number
     * of function evalutions.
     */
    int calcConcentrations(Eigen::VectorXd &conc)

    {

        // solve it
        switch(this->solver.solve(conc, max_time, min_time_step))
        {
            case NLEsolve::Success:
              break;
            case NLEsolve::MaxIterationsReached:
                throw NumericError("Maximum iterations reached. iNA is unable to find the roots of the system. You may try again with bigger number of iterations."); break;
            case NLEsolve::IterationFailed:
                throw NumericError("Line search failed. iNA is unable to find the roots of the system."); break;
            default:
                throw InternalError("Solver returned non-standard error. Status not resolved."); break;
        }

        //std::cerr << solver.getJacobianM().eigenvalues() << std::endl;

        // test for negative concentrations
        if((conc.array()<0).any())
            throw NumericError("iNA encountered negative concentrations. The system may not have a positive root.");

        // ... and test for stability
        if ((solver.getJacobianM().eigenvalues().real().array()>=0.).array().any())
            throw NumericError("iNA has found a steady state which is unstable.");

        // return number of iterations
        return solver.getIterations();

    }

    /**
     * Solves for steady state of the reduced state vector and returns number of function evaluations
     * used.
     *
     * @param x: Outputs the steady state concentrations, covariance and EMRE vector in reduced
     *        coordinates. Contents of x will be overwritten.
     */
    void calcLNA(LNAmodel &model,Eigen::VectorXd &x, Eigen::VectorXex &sseUpdate)

    {
        size_t offset = model.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;

        Eigen::VectorXd A(lnaLength);
        Eigen::MatrixXd B(lnaLength,lnaLength);

        // calc coeff-matrices
        GiNaC::exmap subs_table;
        for (size_t i=0; i<lnaLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( model.getSSEvar(i), 0 ) );
        for(size_t i=0; i<lnaLength; i++)
        {
            A(i) = GiNaC::ex_to<GiNaC::numeric>(
                  GiNaC::evalf(sseUpdate(i).subs(subs_table)) ).to_double();
            for(size_t j=0; j<lnaLength; j++)
            {
               B(i,j) = GiNaC::ex_to<GiNaC::numeric>(
                     GiNaC::evalf(sseUpdate(i).diff(model.getSSEvar(j))) ).to_double();
            }
        }


        x.segment(offset,lnaLength) = NLEsolve::PrecisionSolve::precisionSolve(B, -A, solver.parameters.epsilon);

        // substitute LNA
        subs_table.clear();
        for (size_t i=0; i<lnaLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( model.getSSEvar(i), x(offset+i) ) );
        for(int i=0; i<sseUpdate.size(); i++)
            sseUpdate(i)=sseUpdate(i).subs(subs_table);

    }



    void calcLNA(REmodel &model,Eigen::VectorXd &x, Eigen::VectorXex &sseUpdate)

    {
           // Pass...nothing to do.
    }

    void calcLNA(Eigen::VectorXd &x, Eigen::VectorXex &sseUpdate)

    {
        calcLNA(sseModel,x,sseUpdate);
    }


    /**
     * Solves for steady state of the reduced state vector and returns number of function evaluations
     * used.
     *
     * @param x: Outputs the steady state concentrations, covariance and EMRE vector in reduced
     *        coordinates. Contents of x will be overwritten.
     */
    int calcSteadyState(Eigen::VectorXd &x)

    {

        // collect all the values of constant parameters except variable parameters
        Trafo::ConstantFolder constants(sseModel);

        // fold variable parameters first and then all the rest as constant parameters
        Eigen::VectorXex updateVector = constants.apply( sseModel.getUpdateVector() );
        InitialConditions context(sseModel);
        updateVector = context.apply(updateVector);


        // initialize with initial concentrations
        Eigen::VectorXd conc(sseModel.getDimension());
        sseModel.getInitialState(x);
        conc=x.head(sseModel.numIndSpecies());

        size_t offset = sseModel.numIndSpecies();
        //size_t lnaLength = offset*(offset+1)/2;
        size_t sseLength = updateVector.size()-sseModel.numIndSpecies();

        Eigen::VectorXex sseUpdate = updateVector.segment(offset,sseLength);

        // Calc RE concentrations
        int iter = this->calcConcentrations(conc);
        // ... and substitute RE concentrations
        GiNaC::exmap subs_table;
        for (size_t s=0; s<sseModel.numIndSpecies(); s++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( sseModel.getREvar(s), conc(s) ) );
        for (size_t i=0; i<sseLength; i++)
            sseUpdate(i) = sseUpdate(i).subs(subs_table);

        // Calc LNA
        calcLNA(x,sseUpdate);
        // Calc IOS
        calcIOS(x,sseUpdate);

        x.head(offset) = conc;
//        x.tail(sseLength-lnaLength) = solver.precisionSolve(B,-A);

        return iter;
    }


    void
    calcIOS(IOSmodel &model, Eigen::VectorXd &x, const Eigen::VectorXex &sseUpdate)

    {

        // Get the SSE vector
        size_t offset = model.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;
        size_t sseLength = model.getUpdateVector().size()-model.numIndSpecies();

        // Calc coefficient matrices
        Eigen::VectorXd A(sseLength-lnaLength);
        Eigen::MatrixXd B(sseLength-lnaLength,sseLength-lnaLength);

        GiNaC::exmap subs_table;
        for (size_t i=lnaLength; i<sseLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( model.getSSEvar(i), 0 ) );
        for(size_t i=lnaLength; i<sseLength; i++)
        {
            A(i-lnaLength) = GiNaC::ex_to<GiNaC::numeric>(
                  GiNaC::evalf( sseUpdate(i).subs(subs_table)) ).to_double();
            for(size_t j=lnaLength; j<sseLength; j++)
            {
               B(i-lnaLength,j-lnaLength) = GiNaC::ex_to<GiNaC::numeric>(
                     GiNaC::evalf( sseUpdate(i).diff(model.getSSEvar(j))) ).to_double();
            }
        }

        x.tail(sseLength-lnaLength) = NLEsolve::PrecisionSolve::precisionSolve(B,-A,solver.parameters.epsilon);

    }

    void
    calcIOS(REmodel &model, Eigen::VectorXd &x, const Eigen::VectorXex &sseUpdate)
    {
      // Pass...nothing to do for RE model.
    }

    void
    calcIOS(LNAmodel &model, Eigen::VectorXd &x, const Eigen::VectorXex &sseUpdate)
    {
      // Pass...nothing to do for LNA model.
    }

    void
    calcIOS(Eigen::VectorXd &x, const Eigen::VectorXex &sseUpdate)
    {
        calcIOS(sseModel, x, sseUpdate);
    }

    /** Returns the dimension of the reduced state. */
    inline size_t getDimension() const { return sseModel.getDimension(); }
};




}} // Close namespaces

#endif // __INA_MODELS_STEADYSTATEANALYSIS_HH
