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
        : public SteadyStateAnalysis<M, VectorEngine, MatrixEngine>
{

protected:



    NLEsolve::PrecisionSolve computeLNA;
    NLEsolve::PrecisionSolve computeIOS;

    std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> index;

    size_t opt_level;

    size_t offset;
    size_t lnaLength;
    size_t iosLength;
    size_t sseLength;

    typename VectorEngine::Interpreter interpreter;
    typename MatrixEngine::Interpreter matrix_interpreter;

    typename VectorEngine::Code codeODE;
    typename MatrixEngine::Code codeJac;
    typename VectorEngine::Code LNAcodeA;
    typename MatrixEngine::Code LNAcodeB;
    typename VectorEngine::Code IOScodeA;
    typename MatrixEngine::Code IOScodeB;

    Eigen::VectorXd A;
    Eigen::MatrixXd B;

    Eigen::VectorXd Aios;
    Eigen::MatrixXd Bios;


public:

    /**
    * Constructor
    */
    ParameterScan(M &model, size_t iter=100, double epsilon=1.e-9, double t_max=1e9, double dt=1.e-1, size_t opt_level = 0)
      : SteadyStateAnalysis<M, VectorEngine, MatrixEngine>(model,iter,epsilon,t_max,dt),
        computeLNA(model.lnaLength()), computeIOS(model.iosLength()),
        index(this->sseModel.stateIndex),
        opt_level(opt_level),
        offset(model.numIndSpecies()), lnaLength(model.lnaLength()),
        iosLength(model.iosLength()), sseLength(model.getUpdateVector().size()-offset),
        A(lnaLength), B(lnaLength,lnaLength),
        Aios(iosLength), Bios(iosLength,iosLength)

    {

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
      compileREs(this->sseModel, index, codeODE, codeJac, opt_level);
      compileLNA(this->sseModel, index, LNAcodeA, LNAcodeB, opt_level);
      compileIOS(this->sseModel, index, IOScodeA, IOScodeB, opt_level);

      // Set ode code
      this->solver.set(codeODE, codeJac);

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
                      size_t numThreads = OpenMP::getMaxThreads())

    {

        // Only works with single thread due to Ginac
        numThreads = 1;

        // First make space
        resultSet.resize(parameterSets.size());

        int iter=0;

        // Initialize with initial concentrations
        Eigen::VectorXd x(index.size());
        Eigen::VectorXd init(sseLength+offset);
        this->sseModel.getInitialState(init);

        x.head(offset+sseLength)=init;

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

            // Setup solver and solve for RE concentrations
            try
            {

                // Solve the deterministic equations
                //this->solver.set(index,REs,Jacobian,opt_level);

                iter = this->solver.solve(x, this->max_time, this->min_time_step, parameterSets[j]);
                //x.head(offset) = conc;

                // Now calculate LNA
                calcLNA(this->sseModel,x);

                // Next calculate IOS
                calcIOS(this->sseModel,x);

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

protected:

    void calcLNA(REmodel &model, Eigen::VectorXd &x)

    {
         // Pass...
    }

    void calcLNA(LNAmodel &model, Eigen::VectorXd &x)

    {
        interpreter.setCode(&LNAcodeA);
        matrix_interpreter.setCode(&LNAcodeB);
        interpreter.run(x,A);
        matrix_interpreter.run(x,B);

        // (needs to go in function to match template)
        x.segment(offset,lnaLength) = computeLNA.solve(B,-A);

    }


    void calcIOS(REmodel &model, Eigen::VectorXd &x)

    {
        // Pass...
    }

    void calcIOS(LNAmodel &model, Eigen::VectorXd &x)

    {
        // Pass...
    }

    void calcIOS(IOSmodel &model, Eigen::VectorXd &x)

    {

      interpreter.setCode(&IOScodeA);
      matrix_interpreter.setCode(&IOScodeB);
      interpreter.run(x,Aios);
      matrix_interpreter.run(x,Bios);

      x.segment(offset+lnaLength, iosLength) = computeIOS.solve(Bios,-Aios);

    }

    void compileREs(REmodel &model, const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &indexTable,
                    typename VectorEngine::Code &codeODE,
                    typename MatrixEngine::Code &codeJac,
                    size_t opt_level = 0)

    {

        // Compile ODEs
        typename VectorEngine::Compiler compilerA(indexTable);
        compilerA.setCode(&codeODE);
        compilerA.compileVector( model.getUpdateVector().head(model.numIndSpecies()) );
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

        Eigen::VectorXex A(model.lnaLength());
        Eigen::MatrixXex B(model.lnaLength(),model.lnaLength());

        // Calculate coefficent matrices
        GiNaC::exmap subs_table;
        for (size_t i=0; i<model.lnaLength(); i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( model.getSSEvar(i), 0 ) );
        for(size_t i=0; i<model.lnaLength(); i++)
        {
            A(i) =  model.getUpdateVector()(offset+i).subs(subs_table);
            for(size_t j=0; j<model.lnaLength(); j++)
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
