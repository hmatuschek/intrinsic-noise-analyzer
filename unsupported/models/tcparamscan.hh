#ifndef __INA_MODELS_TCPARAMSCAN_HH
#define __INA_MODELS_TCPARAMSCAN_HH

#include "models/IOSmodel.hh"
#include "math.hh"
#include "models/sseinterpreter.hh"
#include "ode/lsodadriver.hh"

namespace iNA {
namespace Models {

class LookUpTable
    : public std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>
{

public:

    typedef std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> Type;

    LookUpTable(IOSmodel &model, Type &index)
            : Type(index)
    {

      int nstate = this->size();

      // Fetch compartments
      for(size_t i = 0; i<model.numCompartments(); i++)
          this->insert(std::make_pair(model.getCompartment(i)->getSymbol(), nstate++));

      // Fetch global parameters
      for(size_t i = 0; i<model.numParameters(); i++)
          this->insert(std::make_pair(model.getParameter(i)->getSymbol(), nstate++));

      // Fetch local parameters
      for(size_t i = 0; i<model.numReactions(); i++)
      {
        Ast::KineticLaw *kin = model.getReaction(i)->getKineticLaw();
        for(size_t j=0; j<kin->numParameters(); j++)
           this->insert(std::make_pair(kin->getParameter(j)->getSymbol(), nstate++));
      }

      // Fetch conservation laws
      for(int i = 0; i<model.getConservationConstants().size(); i++)
          this->insert(std::make_pair<GiNaC::symbol,size_t>(model.getConservationConstants()(i), nstate++));


     }


};

/**
* Performs a Parameter scan of the time courses.
*/

template <class M,
          class VectorEngine = Eval::bci::Engine<Eigen::VectorXd, Eigen::VectorXd>,
          class MatrixEngine = Eval::bci::Engine<Eigen::VectorXd, Eigen::MatrixXd> >
class TimeCourseParameterScan

{

protected:

    M &model;

    LookUpTable index;

    GenericSSEinterpreter<M,VectorEngine,MatrixEngine> SSEint;

    size_t opt_level;

public:

    /**
    * Constructor
    */
    TimeCourseParameterScan(M &model, size_t opt_level = 0)
      : model(model),
        index(model, model.stateIndex),
        SSEint(model,index,1),
        opt_level(opt_level)
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
    void parameterScan(std::vector<ParameterSet> &parameterSets, double t_final,
                       double timestep, double epsilon_abs, double epsilon_rel,
                       size_t numThreads = OpenMP::getMaxThreads())

    {

        // Some temporary vectors for the result of the analysis
        Eigen::VectorXd concentrations(model.numSpecies());
        Eigen::VectorXd emre_corrections(model.numSpecies());
        Eigen::VectorXd iosemre_corrections(model.numSpecies());
        Eigen::MatrixXd lna_covariances(model.numSpecies(), model.numSpecies());
        Eigen::MatrixXd ios_covariances(model.numSpecies(), model.numSpecies());
        Eigen::VectorXd thirdOrder(model.numSpecies());

        // Only works with single thread due to Ginac
        numThreads = 1;

        // Initialize with initial concentrations
        Eigen::VectorXd x(index.size());
        Eigen::VectorXd init(model.getDimension());
        model.getInitialState(init);

        x.head(model.getDimension())=init;

        ODE::LsodaDriver< Models::GenericSSEinterpreter<M, VectorEngine, MatrixEngine> >
                ODEint(SSEint,timestep,epsilon_abs,epsilon_rel);

        // Gimme a global header line
        std::cout << "# time";

        for(size_t i=0; i<model.numSpecies();i++)
            std::cout << model.getSpecies(i)->getIdentifier() << "\t";
        for(size_t i=0; i<model.numSpecies(); i++)
            for(size_t j=0; j<=i; j++)
                std::cout << "LNA COV(" <<model.getSpecies(i)->getIdentifier() << "," << model.getSpecies(j)->getIdentifier() << ")\t";
        for(size_t i=0; i<model.numSpecies();i++)
            std::cout << "EMRE(" << model.getSpecies(i)->getIdentifier() << ")\t";
        for(size_t i=0; i<model.numSpecies(); i++)
            for(size_t j=0; j<=i; j++)
                std::cout << "IOS COV(" <<model.getSpecies(i)->getIdentifier() << "," << model.getSpecies(j)->getIdentifier() << ")\t";
        std::cout << std::endl;

        // Iterate over all parameter sets
        for(size_t j = 0; j < parameterSets.size(); j++)
        {

            // Generate parameter substitution table
            Trafo::excludeType ptab = model.makeExclusionTable(parameterSets[j]);

            // Collect all the values of constant parameters except variable parameters
            Trafo::ConstantFolder constants(model, Trafo::Filter::ALL_CONST, ptab);
            InitialConditions ICs(model,ptab);

            // Translate identifier to parameters collect variable parameters
            ParameterFolder parameters(ptab);

            // Initialize the state vector which includes the reaction constants
            for(std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::const_iterator it = index.begin(); it!=index.end(); it++)
            {

              if((*it).second<(unsigned)model.numIndSpecies())
                  x((*it).second) = Eigen::ex2double( ICs.apply(parameters.apply(constants.apply((*it).first))) );

              if((*it).second>=(unsigned)model.getUpdateVector().size())
                x((*it).second) = Eigen::ex2double( ICs.apply(parameters.apply(constants.apply((*it).first))) );
            }

            // Do ode step
            try
            {
                // Gimme a header line
                std::cout << "## parameterset: ";
                for(ParameterSet::iterator it = parameterSets[j].begin(); it!=parameterSets[j].end(); it++)
                {
                    std::cout << (*it).first << "=" << (*it).second << "\t";
                }
                std::cout << std::endl;
                for(double t=0; t<=t_final; t+=timestep)
                {
                    ODEint.step(x,t);

                    model.fullState(ICs, x, concentrations, lna_covariances, emre_corrections,
                                    ios_covariances, thirdOrder, iosemre_corrections);
                    std::cout << t << "\t" << concentrations.transpose();

                    for(int i=0; i<lna_covariances.rows(); i++)
                        for(int j=0; j<=i; j++)
                            std::cout << lna_covariances(i,j) << "\t";

                    std::cout << (concentrations+emre_corrections).transpose() << "\t";

                    for(int i=0; i<lna_covariances.rows(); i++)
                        for(int j=0; j<=i; j++)
                            std::cout << lna_covariances(i,j)+ios_covariances(i,j) << "\t";
                    std::cout<<std::endl;
                }

            }
            catch (iNA::NumericError &err)
            {
                std::cerr << "Numeric error" << std::endl;
            }

            ODEint.reset();

        }

    }




};






}} // Close namespaces

#endif // SSEPARAMSCAN_HH
