#ifndef __FLUC_MODELS_STEADYSTATEANALYSIS_HH
#define __FLUC_MODELS_STEADYSTATEANALYSIS_HH

#include "nlesolve/nlesolve.hh"
#include "REmodel.hh"
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
    void calcLNA(Eigen::VectorXd &x, Eigen::VectorXex &sseUpdate)

    {
        size_t offset = sseModel.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;

        Eigen::VectorXd A(lnaLength);
        Eigen::MatrixXd B(lnaLength,lnaLength);

        // calc coeff-matrices
        GiNaC::exmap subs_table;
        for (size_t i=0; i<lnaLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( sseModel.getSSEvar(i), 0 ) );
        for(size_t i=0; i<lnaLength; i++)
        {
            A(i) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).subs(subs_table) ).to_double();
            for(size_t j=0; j<lnaLength; j++)
            {
               B(i,j) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).diff(sseModel.getSSEvar(j)) ).to_double();
            }
        }

        x.segment(offset,lnaLength) = B.lu().solve(-A);

        // substitute LNA
        subs_table.clear();
        for (size_t i=0; i<lnaLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( sseModel.getSSEvar(i), x(offset+i) ) );
        for(int i=0; i<sseUpdate.size(); i++)
            sseUpdate(i)=sseUpdate(i).subs(subs_table);

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

        return iter;
    }


    /**
     * Does a parameter scan of the steady state analysis and returns a .
     *
     * @param parameterSets: Vector of parameter sets to perform analysis for.
     * @param resultSet: Outputs the steady state concentrations, covariance and EMRE vector in reduced
     *        coordinates. Contents will be overwritten.
     */
    int parameterScan(std::vector<GiNaC::exmap> &parameterSets, std::vector<Eigen::VectorXd> &resultSet, const size_t &numThreads=OpenMP::getMaxThreads())

    {

        // first make space
        resultSet.resize(parameterSets.size());

        // initialize with initial concentrations
        Eigen::VectorXd x(sseModel.getDimension());
        sseModel.getInitialState(x);
        Eigen::VectorXd conc=x.head(sseModel.numIndSpecies());

        // get the SSE vector
        size_t offset = sseModel.numIndSpecies();
        //size_t lnaLength = offset*(offset+1)/2;
        size_t sseLength = sseModel.getUpdateVector().size()-sseModel.numIndSpecies();

        Eigen::VectorXex updateVector;

        int iter=0;

        std::vector< NLEsolve::HybridSolver<M> > solvers(numThreads,solver);
        //for(size_t i=0; i<solvers.size(); i++)
          //  solvers[i].set


//#pragma omp parallel for if(numThreads>1) num_threads(numThreads) schedule(dynamic) private(updateVector,iter,x)
        // iterate over all parameter sets
        for(size_t j = 0; j < parameterSets.size(); j++)
        {

            // collect all the values of constant parameters except variable parameters
            Trafo::ConstantFolder constants(sseModel, Trafo::Filter::ALL_CONST, parameterSets[j]);
            // collect the variable parameters
            ParameterFolder parameters(parameterSets[j]);

            // fold variable parameters first and then all the rest as constant parameters
            updateVector = parameters.apply(constants.apply( sseModel.getUpdateVector() ) );
            Eigen::VectorXex REs = updateVector.head(offset);
            Eigen::VectorXex sseUpdate = updateVector.segment(offset,sseLength);
            Eigen::MatrixXex Jacobian = parameters.apply(constants.apply( sseModel.getJacobian()) );

            // setup solver and solve for RE concentrations
            try
            {

                solvers[OpenMP::getThreadNum()].set(sseModel.stateIndex,REs,Jacobian);
                iter = solvers[OpenMP::getThreadNum()].solve(conc, max_time, min_time_step);
                x.head(offset) = conc;

                // ... and substitute RE concentrations
                GiNaC::exmap subs_table;
                for (size_t s=0; s<sseModel.numIndSpecies(); s++)
                    subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( sseModel.getREvar(s), conc(s) ) );
                for (size_t i=0; i<sseLength; i++)
                    sseUpdate(i) = sseUpdate(i).subs(subs_table);

                // calc LNA & IOS
                calcLNA(x,sseUpdate);
                calcIOS(x,sseUpdate);

                // save result in matrix
                resultSet[j] = x;

            }
            catch (iNA::NumericError &err)
            {
                // generate a vector of nans the easy way
                resultSet[j] = Eigen::VectorXd::Zero(sseModel.getDimension())/0.;
            }


        }

        return iter;
    }

    void
    calcIOS(Eigen::VectorXd &x, const Eigen::VectorXex &sseUpdate)
    {

        // get the SSE vector
        size_t offset = sseModel.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;
        size_t sseLength = sseModel.getUpdateVector().size()-sseModel.numIndSpecies();

        // calc coefficient matrices
        Eigen::VectorXd A(sseLength-lnaLength);
        Eigen::MatrixXd B(sseLength-lnaLength,sseLength-lnaLength);

        GiNaC::exmap subs_table;
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

        x.tail(sseLength-lnaLength) = B.lu().solve(-A);

    }

    int
    calcSteadyStateOld(Eigen::VectorXd &x)

    {



        // initialize with initial concentrations
        Eigen::VectorXd conc(sseModel.getDimension());
        sseModel.getInitialState(x);
        conc=x.head(sseModel.numIndSpecies());

        int iter = this->calcConcentrations(conc);

        // construct Diffusion vector
        Eigen::VectorXd DiffusionVec(sseModel.numIndSpecies()*sseModel.numIndSpecies());
        sseModel.getDiffusionVec(DiffusionVec);

        // solve for covariances
        Eigen::VectorXd sol(sseModel.numIndSpecies()*sseModel.numIndSpecies());
        sol = (-KroneckerSum(solver.getJacobianM(),solver.getJacobianM())).lu().solve(DiffusionVec);

        // should be check here if this is a good solution (requires a new epsilon!!!)
        //bool a_solution_exists = (JMsum*sol).isApprox(-DiffusionVec, 1.E-10);

        Eigen::VectorXd covVec((sseModel.numIndSpecies()*(sseModel.numIndSpecies()+1))/2);
        Eigen::MatrixXd cov(sseModel.numIndSpecies(),sseModel.numIndSpecies());

        // take only important elements into reduced state vector
        size_t idx=0;
        for(size_t i=0;i<sseModel.numIndSpecies();i++)
        {
          for(size_t j=0;j<=i;j++)
          {

            size_t idy=i+j*sseModel.numIndSpecies();
            covVec(idx)=sol(idy);
            cov(i,j) =sol(idy);//=covarianceMatrix(i,j);
            idx++;
            cov(j,i)=cov(i,j);

          }
        }
        // covariance finished

        // calculate EMRE

        Eigen::VectorXd Delta(sseModel.numIndSpecies());

        // get Rate corrections
        Eigen::VectorXd REcorr;
        sseModel.getRateCorrections(REcorr);

        // get stacked up Hessian
        Eigen::MatrixXd HessianM;
        sseModel.getHessian(HessianM);

        for (size_t i=0; i<sseModel.numIndSpecies(); i++)
        {
          Delta(i)=0.;
          idx=0;
          for (size_t j=0; j<sseModel.numIndSpecies(); j++)
          {
            for (size_t k=0; k<=j; k++)
            {

                if(k==j)
                {
                    Delta(i) += HessianM(i,idx)*cov(j,k);
                }
                else
                {
                    Delta(i) += 2.*HessianM(i,idx)*cov(j,k);
                }
                idx++;

            }
          }
          Delta(i)/=2.;
          // add RE corrections
          Delta(i)+=REcorr(i);
        }

        // solve EMRE
        Eigen::VectorXd emre(sseModel.numIndSpecies());
        emre=solver.getJacobianM().lu().solve(-Delta);

        // update reduced state vector
        // for concentrations
        // and covariances
        // and emre

        x.resize(sseModel.getDimension());
        x << conc, covVec, emre;

        return iter;

    }

};


}} // close namespaces

#endif // __FLUC_MODELS_STEADYSTATEANALYSIS_HH
