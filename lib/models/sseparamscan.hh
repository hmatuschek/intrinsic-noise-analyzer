#ifndef __INA_MODELS_SSEPARAMSCAN_HH
#define __INA_MODELS_SSEPARAMSCAN_HH


#include "nlesolve/nlesolve.hh"
#include "IOSmodel.hh"
#include "math.hh"
#include "steadystateanalysis.hh"

namespace iNA {
namespace Models {

/**
* Extension of the SteadyStateAnalysis to perform a Parameter scan.
*/

template <class M,
          class VectorEngine = Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          class MatrixEngine = Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> >
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
     * @param opt_level: Optimization level for expression evaluation.
     */
    int parameterScan(std::vector<ParameterSet> &parameterSets,
                      std::vector<Eigen::VectorXd> &resultSet,
                      size_t numThreads = OpenMP::getMaxThreads(),
                      size_t opt_level = 0)

    {

        // Only works with single thread due to Ginac
        numThreads = 1;

        // First make space
        resultSet.resize(parameterSets.size());

        // Get the SSE vector
        size_t offset = this->sseModel.numIndSpecies();
        size_t sseLength = this->sseModel.getUpdateVector().size()-this->sseModel.numIndSpecies();
        size_t lnaLength = sseLength > 0 ? offset*(offset+1)/2 : 0;
        size_t iosLength = (sseLength - lnaLength) > 0 ? (sseLength - lnaLength) : 0;

        int iter=0;

        std::vector< NLEsolve::HybridSolver<M> > solvers(numThreads,this->solver);

        std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> index(this->sseModel.stateIndex);

        int nstate = index.size();

        // Fetch compartments
        for(size_t i = 0; i<this->sseModel.numCompartments(); i++)
            index.insert(std::make_pair(this->sseModel.getCompartment(i)->getSymbol(), nstate++));

        // Fetch global parameters
        for(size_t i = 0; i<this->sseModel.numParameters(); i++)
            index.insert(std::make_pair(this->sseModel.getParameter(i)->getSymbol(), nstate++));

        // Fetch local parameters
        for(size_t i = 0; i<this->sseModel.numReactions(); i++)
        {
          Ast::KineticLaw *kin = this->sseModel.getReaction(i)->getKineticLaw();
          for(size_t j=0; j<kin->numParameters(); j++)
             index.insert(std::make_pair(kin->getParameter(j)->getSymbol(), nstate++));
        }

        // Fetch conservation laws
        for(int i = 0; i<this->sseModel.getConservationConstants().size(); i++)
            index.insert(std::make_pair<GiNaC::symbol,size_t>(this->sseModel.getConservationConstants()(i), nstate++));

        // Now compile

//        Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>::Code codeODE;
//        Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd>::Code codeJac;
//        compileREs(this->sseModel, index, codeODE, codeJac);

        typename VectorEngine::Code LNAcodeA;
        typename MatrixEngine::Code LNAcodeB;
        compileLNA(this->sseModel, index, LNAcodeA, LNAcodeB, opt_level);

        typename VectorEngine::Code IOScodeA;
        typename MatrixEngine::Code IOScodeB;
        compileIOS(this->sseModel, index, IOScodeA, IOScodeB, opt_level);

        // ... and setup the interpreter

        typename VectorEngine::Interpreter interpreter;
        typename MatrixEngine::Interpreter matrix_interpreter;

        // Initialize with initial concentrations
        Eigen::VectorXd x(nstate);
        Eigen::VectorXd init(sseLength+offset);
        this->sseModel.getInitialState(init);

        x.head(offset+sseLength)=init;
        Eigen::VectorXd conc = init.head(offset);

        Eigen::VectorXex REs;
        Eigen::MatrixXex Jacobian;

//#pragma omp parallel for if(numThreads>1) num_threads(numThreads) schedule(dynamic) firstprivate(iter,x,conc)
        // Iterate over all parameter sets
        for(size_t j = 0; j < parameterSets.size(); j++)
        {

            // Generate parameter substitution table
            Trafo::excludeType ptab = this->sseModel.makeExclusionTable(parameterSets[j]);

            // Collect all the values of constant parameters except variable parameters
            Trafo::ConstantFolder constants(this->sseModel, Trafo::Filter::ALL_CONST, ptab);
            InitialConditions ICs(this->sseModel,ptab);

            // Translate identifier to parameters collect variable parameters
            ParameterFolder parameters(ptab);

            // Initialize the state vector which includes the reaction constants
            for(std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::const_iterator it = index.begin(); it!=index.end(); it++)
            {
              if((*it).second>=(unsigned)this->sseModel.getUpdateVector().size())
                x((*it).second) = Eigen::ex2double( ICs.apply(parameters.apply(constants.apply((*it).first))) );
            }

            // Fold variable parameters and all the rest
            REs = ICs.apply(parameters.apply(constants.apply( this->sseModel.getUpdateVector().head(offset) ) ));
            Jacobian = ICs.apply(parameters.apply(constants.apply( this->sseModel.getJacobian()) ));

            // Setup solver and solve for RE concentrations
            try
            {

                // Solve the deterministic equations
                //solvers[OpenMP::getThreadNum()].set(codeODE,codeJac);
                solvers[OpenMP::getThreadNum()].set(index,REs,Jacobian);

                iter = solvers[OpenMP::getThreadNum()].solve(conc, this->max_time, this->min_time_step);
                x.head(offset) = conc;

                // Now calculate LNA
                Eigen::VectorXd A(lnaLength);
                Eigen::MatrixXd B(lnaLength,lnaLength);

                interpreter.setCode(&LNAcodeA);
                matrix_interpreter.setCode(&LNAcodeB);
                interpreter.run(x,A);
                matrix_interpreter.run(x,B);

                // (needs to go in function to match template)
                x.segment(offset,lnaLength) = solvers[OpenMP::getThreadNum()].precisionSolve(B,-A);

                // Next calculate IOS
                A.resize(iosLength);
                B.resize(iosLength,iosLength);

                interpreter.setCode(&IOScodeA);
                matrix_interpreter.setCode(&IOScodeB);
                interpreter.run(x,A);
                matrix_interpreter.run(x,B);

                // (needs to go in function)
                x.segment(offset+lnaLength, iosLength) = solvers[OpenMP::getThreadNum()].precisionSolve(B,-A);

                // Store result
                resultSet[j] = x.head(offset+sseLength);

            }
            catch (iNA::NumericError &err)
            {
                // Generate a vector of nans the easy way
                resultSet[j] = Eigen::VectorXd::Zero(this->sseModel.getDimension())/0.;
            }

        }

        return iter;
    }

    void compileREs(REmodel &model, const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &indexTable,
                    typename VectorEngine::Code &codeODE,
                    typename MatrixEngine::Code &codeJac,
                    size_t opt_level = 0)

    {

        // Compile ODEs
        typename VectorEngine::Compiler compilerA(indexTable);
        compilerA.setCode(&codeODE);
        compilerA.compileVector(model.getUpdateVector().head(model.numIndSpecies()));
        compilerA.finalize(0);

        // Compile Jacobian
        typename MatrixEngine::Compiler compilerB(indexTable);
        compilerB.setCode(&codeJac);
        compilerB.compileMatrix(model.getJacobian());
        compilerB.finalize(0);

    }


    void
    compileLNA(REmodel &model, const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &indexTable,
               typename VectorEngine::Code &codeA,
               typename MatrixEngine::Code &codeB,
               size_t opt_level = 0)

    {
        // Pass..
    }

    void compileLNA(LNAmodel &model, const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &indexTable,
                    typename VectorEngine::Code &codeA,
                    typename MatrixEngine::Code &codeB,
                    size_t opt_level = 0)

    {
        size_t offset = model.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;

        Eigen::VectorXex A(lnaLength);
        Eigen::MatrixXex B(lnaLength,lnaLength);

        // Calculate coefficent matrices
        GiNaC::exmap subs_table;
        for (size_t i=0; i<lnaLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( model.getSSEvar(i), 0 ) );
        for(size_t i=0; i<lnaLength; i++)
        {
            A(i) =  model.getUpdateVector()(offset+i).subs(subs_table);
            for(size_t j=0; j<lnaLength; j++)
            {
               B(i,j) = model.getUpdateVector()(offset+i).diff(model.getSSEvar(j));
            }
        }

        // Compile A
        typename VectorEngine::Compiler compilerA(indexTable);
        compilerA.setCode(&codeA);
        compilerA.compileVector(A);
        compilerA.finalize(opt_level);

        // Compile B
        typename MatrixEngine::Compiler compilerB(indexTable);
        compilerB.setCode(&codeB);
        compilerB.compileMatrix(B);
        compilerB.finalize(opt_level);

    }

    void
    compileIOS(REmodel &model, const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &indexTable,
               typename VectorEngine::Code &codeA,
               typename MatrixEngine::Code &codeB,
               size_t opt_level = 0)

    {
        // Pass..
    }

    void
    compileIOS(LNAmodel &model, const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &indexTable,
               typename VectorEngine::Code &codeA,
               typename MatrixEngine::Code &codeB,
               size_t opt_level = 0)

    {
        // Pass..
    }


    void
    compileIOS(IOSmodel &model, const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &indexTable,
               typename VectorEngine::Code &codeA,
               typename MatrixEngine::Code &codeB,
               size_t opt_level = 0)

    {

        // Get the SSE vector
        size_t offset = model.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;
        size_t sseLength = model.getUpdateVector().size()-model.numIndSpecies();
        size_t iosLength = sseLength - lnaLength;

        // Calc coefficient matrices
        Eigen::VectorXex A(iosLength);
        Eigen::MatrixXex B(iosLength,iosLength);

        GiNaC::exmap subs_table;
        for (size_t i=0; i<iosLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( model.getSSEvar(i+lnaLength), 0 ) );
        for(size_t i=0; i<iosLength; i++)
        {
            A(i) = model.getUpdateVector()(i+offset+lnaLength).subs(subs_table);
            for(size_t j=0; j<iosLength; j++)
            {
               B(i,j) = model.getUpdateVector()(i+offset+lnaLength).diff(model.getSSEvar(j+lnaLength));
            }
        }

        // Compile A
        typename VectorEngine::Compiler compilerA(indexTable);
        compilerA.setCode(&codeA);
        compilerA.compileVector(A);
        compilerA.finalize(opt_level);

        // Compile B
        typename MatrixEngine::Compiler compilerB(indexTable);
        compilerB.setCode(&codeB);
        compilerB.compileMatrix(B);
        compilerB.finalize(opt_level);

    }




};



/**
* Extension of the SteadyStateAnalysis to perform a Parameter scan.
*/
template <typename M>
class ParameterScanOld
        : public SteadyStateAnalysis<M>
{

public:

    ParameterScanOld(M &model)
        : SteadyStateAnalysis<M>(model)
    {

        // Pass...

    }

    /**
    * Constructor
    */
    ParameterScanOld(M &model, size_t iter, double epsilon, double t_max=1e9, double dt=1e-1)
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



};

}} // Close namespaces

#endif // SSEPARAMSCAN_HH
