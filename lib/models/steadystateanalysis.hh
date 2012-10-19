#ifndef __INA_MODELS_STEADYSTATEANALYSIS_HH
#define __INA_MODELS_STEADYSTATEANALYSIS_HH

#include "nlesolve/nlesolve.hh"
#include "LNAmodel.hh"
#include "math.hh"

namespace iNA {
namespace Models {

/**
* Folds a set of given constants from an parameter set.
*/
class ParameterFolder
{

    /**
    * Holds a reference to the substitutions.
    */
    GiNaC::exmap &_substitutions;

public:
    /**
    * Constructor
    */
    ParameterFolder(GiNaC::exmap &constants)
        : _substitutions(constants)
    {

    }


    /**
    * A method that folds all constants in a vector or matrix.
    */
    Eigen::MatrixXex apply(const Eigen::MatrixXex &vecIn)
    {

        Eigen::MatrixXex vecOut(vecIn.rows(),vecIn.cols());
        // ... and fold all constants due to conservation laws
        for (int i=0; i<vecIn.rows(); i++)
        for (int j=0; j<vecIn.cols(); j++)
                vecOut(i,j)=vecIn(i,j).subs(_substitutions);

        return vecOut;

    }

};

template <typename M>
class SteadyStateAnalysis
{
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
      // Pass...
    }

    /**
    * Constructor
    */
    SteadyStateAnalysis(M &model, size_t iter, double epsilon, double t_max=1e9, double dt=1e-1)
      : sseModel(model), solver(model), max_time(t_max), min_time_step(dt)

    {
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

        x.segment(offset,lnaLength) = B.lu().solve(-A);

        double relative_error = (B*x.segment(offset,lnaLength) + A).norm() / A.norm(); // norm() is L2 norm
        std::cout << "The relative error is:\n" << relative_error << std::endl;

        // substitute LNA
        subs_table.clear();
        for (size_t i=0; i<lnaLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( model.getSSEvar(i), x(offset+i) ) );
        for(int i=0; i<sseUpdate.size(); i++)
            sseUpdate(i)=sseUpdate(i).subs(subs_table);

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

        // initialize with initial concentrations
        Eigen::VectorXd conc(sseModel.getDimension());
        sseModel.getInitialState(x);
        conc=x.head(sseModel.numIndSpecies());

        size_t offset = sseModel.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;
        size_t sseLength = updateVector.size()-sseModel.numIndSpecies();

        Eigen::VectorXex sseUpdate = updateVector.segment(offset,sseLength);


        // calc
        int iter = this->calcConcentrations(conc);
        // ... and substitute RE concentrations
        GiNaC::exmap subs_table;
        for (size_t s=0; s<sseModel.numIndSpecies(); s++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( sseModel.getREvar(s), conc(s) ) );
        for (size_t i=0; i<sseLength; i++)
            sseUpdate(i) = sseUpdate(i).subs(subs_table);

        // calc LNA
        calcLNA(x,sseUpdate);

        // calc coeff-matrices
        Eigen::VectorXd A(sseLength-lnaLength);
        Eigen::MatrixXd B(sseLength-lnaLength,sseLength-lnaLength);
        subs_table.clear();
        for (size_t i=lnaLength; i<sseLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( sseModel.getSSEvar(i), 0 ) );
        for(size_t i=lnaLength; i<sseLength; i++)
        {
            A(i-lnaLength) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).subs(subs_table) ).to_double();
            for(size_t j=lnaLength; j<sseLength; j++)
            {
               B(i-lnaLength,j-lnaLength) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).diff(sseModel.getSSEvar(j)) ).to_double();
            }
        }

        x.head(offset) = conc;
        x.tail(sseLength-lnaLength) = B.lu().solve(-A);

        double relative_error = (B*x.tail(sseLength-lnaLength) + A).norm() / A.norm(); // norm() is L2 norm
        std::cout << "The relative error is:\n" << relative_error << std::endl;


        return iter;
    }


    /**
     * Perform a parameter scan using the steady state analysis.
     *
     * @param parameterSets: Vector of parameter sets to perform analysis for.
     * @param resultSet: Outputs the steady state concentrations, covariance and EMRE vector in reduced
     *        coordinates. Contents will be overwritten.
     */
    int parameterScan(std::vector<ParameterSet> &parameterSets, std::vector<Eigen::VectorXd> &resultSet, const size_t &numThreads=OpenMP::getMaxThreads())

    {

        // First make space
        resultSet.resize(parameterSets.size());

        // Initialize with initial concentrations
        Eigen::VectorXd x(sseModel.getDimension());
        sseModel.getInitialState(x);
        Eigen::VectorXd conc=x.head(sseModel.numIndSpecies());

        // Get the SSE vector
        size_t offset = sseModel.numIndSpecies();
        //size_t lnaLength = offset*(offset+1)/2;
        size_t sseLength = sseModel.getUpdateVector().size()-sseModel.numIndSpecies();

        Eigen::VectorXex updateVector;

        int iter=0;

        std::vector< NLEsolve::HybridSolver<M> > solvers(numThreads,solver);

        // Copy models for parallelization
        std::vector< M * > models(numThreads);
        models[0] = &sseModel;
        Ast::Model* base = dynamic_cast<Ast::Model*>(&sseModel);
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
            GiNaC::exmap ptab = models[OpenMP::getThreadNum()]->translate(parameterSets[j]);

            // Collect all the values of constant parameters except variable parameters
            Trafo::ConstantFolder constants(*(models[OpenMP::getThreadNum()]), Trafo::Filter::ALL_CONST, ptab);

            // Translate identifier to parameters collect variable parameters
            ParameterFolder parameters(ptab);

            // Fold variable parameters and all the rest
            Eigen::VectorXex updateVector = parameters.apply(constants.apply( models[OpenMP::getThreadNum()]->getUpdateVector() ) );
            Eigen::VectorXex REs = updateVector.head(offset);
            Eigen::VectorXex sseUpdate = updateVector.segment(offset,sseLength);
            Eigen::MatrixXex Jacobian = parameters.apply(constants.apply( models[OpenMP::getThreadNum()]->getJacobian()) );

            // Setup solver and solve for RE concentrations
            try
            {

                solvers[OpenMP::getThreadNum()].set(models[OpenMP::getThreadNum()]->stateIndex,REs,Jacobian);
                iter = solvers[OpenMP::getThreadNum()].solve(conc, max_time, min_time_step);
                x.head(offset) = conc;

                // ... and substitute RE concentrations
                GiNaC::exmap subs_table;
                for (size_t s=0; s<models[OpenMP::getThreadNum()]->numIndSpecies(); s++)
                    subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( models[OpenMP::getThreadNum()]->getREvar(s), conc(s) ) );
                for (size_t i=0; i<sseLength; i++)
                    sseUpdate(i) = sseUpdate(i).subs(subs_table);

                // Calc LNA & IOS
                calcLNA(*models[OpenMP::getThreadNum()],x,sseUpdate);
                calcIOS(*models[OpenMP::getThreadNum()],x,sseUpdate);

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


    void
    calcIOS(LNAmodel &model, Eigen::VectorXd &x, const Eigen::VectorXex &sseUpdate)
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

        x.tail(sseLength-lnaLength) = B.lu().solve(-A);

        double relative_error = (B*x.tail(sseLength-lnaLength) + A).norm() / A.norm(); // norm() is L2 norm
        std::cout << "IOS, The relative error is:\n" << relative_error << std::endl;

    }

    void
    calcIOS(Eigen::VectorXd &x, const Eigen::VectorXex &sseUpdate)
    {
        calcIOS(sseModel, x, sseUpdate);
    }

};


}} // Close namespaces

#endif // __FLUC_MODELS_STEADYSTATEANALYSIS_HH
