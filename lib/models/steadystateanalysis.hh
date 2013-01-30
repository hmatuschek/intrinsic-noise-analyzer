#ifndef __INA_MODELS_STEADYSTATEANALYSIS_HH
#define __INA_MODELS_STEADYSTATEANALYSIS_HH

#include "nlesolve/nlesolve.hh"
#include "IOSmodel.hh"
#include "math.hh"

namespace iNA {
namespace Models {

/**
* Performs the Steady State Analysis on a model.
*/

template <typename M>
class SteadyStateAnalysis
{
protected:
    /**
     * Holds a reference to a SSE model.
     */

    M &sseModel;

    /**
     * An instance of a nonlinear solver.
     */
    //NLEsolve::NewtonRaphson<M> solver;
    NLEsolve::HybridSolver<M> solver;

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

        solver.set(model.stateIndex, REs, Jac);

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

        solver.set(model.stateIndex, REs, Jac);

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
            A(i) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).subs(subs_table) ).to_double();
            for(size_t j=0; j<lnaLength; j++)
            {
               B(i,j) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).diff(model.getSSEvar(j)) ).to_double();
            }
        }


        x.segment(offset,lnaLength) = solver.precisionSolve(B,-A);


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

//        // calc coeff-matrices
//        Eigen::VectorXd A(sseLength-lnaLength);
//        Eigen::MatrixXd B(sseLength-lnaLength,sseLength-lnaLength);
//        subs_table.clear();
//        for (size_t i=lnaLength; i<sseLength; i++)
//            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( sseModel.getSSEvar(i), 0 ) );
//        for(size_t i=lnaLength; i<sseLength; i++)
//        {
//            A(i-lnaLength) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).subs(subs_table) ).to_double();
//            for(size_t j=lnaLength; j<sseLength; j++)
//            {
//               B(i-lnaLength,j-lnaLength) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).diff(sseModel.getSSEvar(j)) ).to_double();
//            }
//        }

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
            A(i-lnaLength) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).subs(subs_table) ).to_double();
            for(size_t j=lnaLength; j<sseLength; j++)
            {
               B(i-lnaLength,j-lnaLength) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).diff(model.getSSEvar(j)) ).to_double();
            }
        }

        x.tail(sseLength-lnaLength) = solver.precisionSolve(B,-A);

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


/**
* Extension of the SteadyStateAnalysis to perform a Parameter scan.
*/
template <typename M>
class ParameterScan
        : public SteadyStateAnalysis<M>
{

public:

    ParameterScan(M &model)
        : SteadyStateAnalysis<M>(model)
    {

        // Pass...

    }

    /**
    * Constructor
    */
    ParameterScan(M &model, size_t iter, double epsilon, double t_max=1e9, double dt=1e-1)
      : SteadyStateAnalysis<M>(model,iter,epsilon,t_max,dt)

    {

    }

    /**
     * Perform a parameter scan using the steady state analysis.
     *
     * @param parameterSets: Vector of parameter sets to perform analysis for.
     * @param resultSet: Outputs the steady state concentrations, covariance and EMRE vector in reduced
     *        coordinates. Contents will be overwritten.
     */
    int parameterScan(std::vector<ParameterSet> &parameterSets, std::vector<Eigen::VectorXd> &resultSet, size_t numThreads=OpenMP::getMaxThreads())

    {

        // Only works with single thread due to Ginac
        numThreads = 1;

        // First make space
        resultSet.resize(parameterSets.size());

        // Initialize with initial concentrations
        Eigen::VectorXd x(this->sseModel.getDimension());
        this->sseModel.getInitialState(x);
        Eigen::VectorXd conc=x.head(this->sseModel.numIndSpecies());

        // Get the SSE vector
        size_t offset = this->sseModel.numIndSpecies();
        //size_t lnaLength = offset*(offset+1)/2;
        size_t sseLength = this->sseModel.getUpdateVector().size()-this->sseModel.numIndSpecies();

        Eigen::VectorXex updateVector;

        int iter=0;

        std::vector< NLEsolve::HybridSolver<M> > solvers(numThreads,this->solver);

        // Copy models for parallelization
        std::vector< M * > models(numThreads);
        models[0] = &this->sseModel;
        Ast::Model* base = dynamic_cast<Ast::Model*>(&this->sseModel);
//#pragma omp parallel for if(numThreads>1) num_threads(numThreads)
        for(size_t j=1; j<numThreads; j++)
        {
            models[j] = new M((*base));
        }

//#pragma omp parallel for if(numThreads>1) num_threads(numThreads) schedule(dynamic) firstprivate(iter,x,conc)
        // Iterate over all parameter sets
        for(size_t j = 0; j < parameterSets.size(); j++)
        {

            // Generate parameter substitution table
            Trafo::excludeType ptab = models[OpenMP::getThreadNum()]->makeExclusionTable(parameterSets[j]);

            // Collect all the values of constant parameters except variable parameters
            Trafo::ConstantFolder constants(*(models[OpenMP::getThreadNum()]), Trafo::Filter::ALL_CONST, ptab);
            InitialConditions ICs(*(models[OpenMP::getThreadNum()]),ptab);

            // Translate identifier to parameters collect variable parameters
            ParameterFolder parameters(ptab);

            // Fold variable parameters and all the rest
            Eigen::VectorXex updateVector = ICs.apply(parameters.apply(constants.apply( models[OpenMP::getThreadNum()]->getUpdateVector() ) ));
            Eigen::VectorXex REs = updateVector.head(offset);
            Eigen::VectorXex sseUpdate = updateVector.segment(offset,sseLength);
            Eigen::MatrixXex Jacobian = ICs.apply(parameters.apply(constants.apply( models[OpenMP::getThreadNum()]->getJacobian()) ));

            // Setup solver and solve for RE concentrations
            try
            {

                solvers[OpenMP::getThreadNum()].set(models[OpenMP::getThreadNum()]->stateIndex,REs,Jacobian);
                iter = solvers[OpenMP::getThreadNum()].solve(conc, this->max_time, this->min_time_step);
                x.head(offset) = conc;

                // ... and substitute RE concentrations
                GiNaC::exmap subs_table;
                for (size_t s=0; s<models[OpenMP::getThreadNum()]->numIndSpecies(); s++)
                    subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( models[OpenMP::getThreadNum()]->getREvar(s), conc(s) ) );
                for (size_t i=0; i<sseLength; i++)
                    sseUpdate(i) = sseUpdate(i).subs(subs_table);

                // Calc LNA & IOS
                this->calcLNA(*models[OpenMP::getThreadNum()],x,sseUpdate);
                this->calcIOS(*models[OpenMP::getThreadNum()],x,sseUpdate);

                // Store result
                resultSet[j] = x;

            }
            catch (iNA::NumericError &err)
            {
                // Generate a vector of nans the easy way
                resultSet[j] = Eigen::VectorXd::Zero(models[OpenMP::getThreadNum()]->getDimension())/0.;
            }

        }

        return iter;
    }


    /**
     * Perform a parameter scan using the steady state analysis.
     *
     * @param parameterSets: Vector of parameter sets to perform analysis for.
     * @param resultSet: Outputs the steady state concentrations, covariance and EMRE vector in reduced
     *        coordinates. Contents will be overwritten.
     */
    int parameterScanLLVM(std::vector<ParameterSet> &parameterSets, std::vector<Eigen::VectorXd> &resultSet, size_t numThreads=OpenMP::getMaxThreads())

    {

        // Only works with single thread due to Ginac
        numThreads = 1;

        // First make space
        resultSet.resize(parameterSets.size());

        // Initialize with initial concentrations
        Eigen::VectorXd x(this->sseModel.getDimension());
        this->sseModel.getInitialState(x);
        Eigen::VectorXd conc=x.head(this->sseModel.numIndSpecies());

        // Get the SSE vector
        size_t offset = this->sseModel.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;
        size_t sseLength = this->sseModel.getUpdateVector().size()-this->sseModel.numIndSpecies();


        Eigen::VectorXex updateVector;

        int iter=0;

        std::vector< NLEsolve::HybridSolver<M> > solvers(numThreads,this->solver);

        // Copy models for parallelization
        std::vector< M * > models(numThreads);
        models[0] = &this->sseModel;
        Ast::Model* base = dynamic_cast<Ast::Model*>(&this->sseModel);
//#pragma omp parallel for if(numThreads>1) num_threads(numThreads)
        for(size_t j=1; j<numThreads; j++)
        {
            models[j] = new M((*base));
        }

        std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> index = this->sseModel.stateIndex;

        iNA::Models::LNAmodel test;

        for(size_t i = 0; i<this->sseModel.numParameters(); i++)
            index.insert(std::make_pair(this->sseModel.getParameter().getSymbol(), index.size()++));

        for(size_t i = 0; i<this->sseModel.getConservationConstants().size(); i++)
            index.insert(std::make_pair(this->sseModel.getConservationConstants()(i), index.size()++));

        // now compile

        Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Code &LNAcodeA;
        Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Code &LNAcodeB;
        compileLNA(index,LNAcodeA,LNAcodeB);

        Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Code &IOScodeA;
        Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Code &IOScodeB;
        compileIOS(index,IOScodeA,IOScodeB);

        // and setup the interpreter
        Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Interpreter interpreter;
        Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Interpreter matrix_interpreter;

        interpreter.setCode(&LNAcodeA);
        matrix_interpreter.setCode(&LNAcodeB);


//#pragma omp parallel for if(numThreads>1) num_threads(numThreads) schedule(dynamic) firstprivate(iter,x,conc)
        // Iterate over all parameter sets
        for(size_t j = 0; j < parameterSets.size(); j++)
        {

            // Generate parameter substitution table
            Trafo::excludeType ptab = models[OpenMP::getThreadNum()]->makeExclusionTable(parameterSets[j]);

            // Collect all the values of constant parameters except variable parameters
            Trafo::ConstantFolder constants(*(models[OpenMP::getThreadNum()]), Trafo::Filter::ALL_CONST, ptab);
            InitialConditions ICs(*(models[OpenMP::getThreadNum()]),ptab);

            // Translate identifier to parameters collect variable parameters
            ParameterFolder parameters(ptab);


            // Initialize the state vector which includes the reaction constants
            for(std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::const_iterator it = index.begin(); it!=index.end(); it++)
            {
                x((*it).second) = Eigen::ex2double( ICs.apply(parameters.apply(constants.apply((*it).first))) );
            }

            // Fold variable parameters and all the rest
            Eigen::VectorXex updateVector = ICs.apply(parameters.apply(constants.apply( models[OpenMP::getThreadNum()]->getUpdateVector() ) ));
            Eigen::VectorXex REs = updateVector.head(offset);
            //Eigen::VectorXex sseUpdate = updateVector.segment(offset,sseLength);
            Eigen::MatrixXex Jacobian = ICs.apply(parameters.apply(constants.apply( models[OpenMP::getThreadNum()]->getJacobian()) ));

            // Setup solver and solve for RE concentrations
            try
            {

                // Solve the deterministic equations
                solvers[OpenMP::getThreadNum()].set(models[OpenMP::getThreadNum()]->stateIndex,REs,Jacobian);
                iter = solvers[OpenMP::getThreadNum()].solve(conc, this->max_time, this->min_time_step);
                x.head(offset) = conc;

                // Now calc LNA
                Eigen::VectorXd A;
                Eigen::MatrixXd B;
                interpreter.run(x,A);
                matrix_interpreter.run(x,B);

                x.segment(offset,lnaLength) = solvers[OpenMP::getThreadNum()].precisionSolve(B,-A);

                // Next calc IOS
                interpreter.setCode(&IOScodeA);
                matrix_interpreter.setCode(&IOScodeB);
                interpreter.run(x,A);
                matrix_interpreter.run(x,B);

                x.tail(sseLength-lnaLength) = solvers[OpenMP::getThreadNum()].precisionSolve(B,-A);

                // Store result
                resultSet[j] = x;

            }
            catch (iNA::NumericError &err)
            {
                // Generate a vector of nans the easy way
                resultSet[j] = Eigen::VectorXd::Zero(models[OpenMP::getThreadNum()]->getDimension())/0.;
            }

        }

        return iter;
    }


    void compileLNA(LNAmodel &model, const std::map<GiNaC::symbol, size_t, GiNaC::expair_is_less> &indexTable,
                    Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Code &codeA,
                    Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Code &codeB)

    {
        size_t offset = model.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;

        Eigen::VectorXex A(lnaLength);
        Eigen::MatrixXex B(lnaLength,lnaLength);

        Eigen::VectorXex& sseUpdate = model.getUpdateVector();

        // calc coeff-matrices
        GiNaC::exmap subs_table;
        for (size_t i=0; i<lnaLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( model.getSSEvar(i), 0 ) );
        for(size_t i=0; i<lnaLength; i++)
        {
            A(i) =  sseUpdate(i).subs(subs_table);
            for(size_t j=0; j<lnaLength; j++)
            {
               B(i,j) = sseUpdate(i).diff(model.getSSEvar(j));
            }
        }

        // Compile A
        Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Compiler compilerA(indexTable);
        compilerA.setCode(&codeA);
        compilerA.compileVector(A);
        compilerA.finalize(0);

        // Compile B
        Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Compiler compilerB(indexTable);
        compilerB.setCode(&codeB);
        compilerB.compileMatrix(B);
        compilerB.finalize(0);

    }

    void
    compileIOS(IOSmodel &model, const std::map<GiNaC::symbol, size_t, GiNaC::expair_is_less> &indexTable,
               Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Code &codeA,
               Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Code &codeB)

    {

        // Get the SSE vector
        size_t offset = model.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;
        size_t sseLength = model.getUpdateVector().size()-model.numIndSpecies();

        Eigen::VectorXex& sseUpdate = model.getUpdateVector();

        // Calc coefficient matrices
        Eigen::VectorXex A(sseLength-lnaLength);
        Eigen::MatrixXex B(sseLength-lnaLength,sseLength-lnaLength);

        GiNaC::exmap subs_table;
        for (size_t i=lnaLength; i<sseLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( model.getSSEvar(i), 0 ) );
        for(size_t i=lnaLength; i<sseLength; i++)
        {
            A(i-lnaLength) = sseUpdate(i).subs(subs_table);
            for(size_t j=lnaLength; j<sseLength; j++)
            {
               B(i-lnaLength,j-lnaLength) = sseUpdate(i).diff(model.getSSEvar(j));
            }
        }

        // Compile A
        Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Compiler compilerA(indexTable);
        compilerA.setCode(&codeA);
        compilerA.compileVector(A);
        compilerA.finalize(0);

        // Compile B
        Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Compiler compilerB(indexTable);
        compilerB.setCode(&codeB);
        compilerB.compileMatrix(B);
        compilerB.finalize(0);

    }



};

}} // Close namespaces

#endif // __FLUC_MODELS_STEADYSTATEANALYSIS_HH
