#ifndef __INA_SSAPARAMSCAN_HH
#define __INA_SSAPARAMSCAN_HH

#include "optimizedSSA.hh"
#include "ssebasemodel.hh"


namespace iNA {
namespace Models {

class ParamScanInterface
{

public:

  ParamScanInterface()
  {

  };

  virtual const Eigen::MatrixXd& getMean() const =0;

  virtual const Eigen::MatrixXd& getCovariance() const =0;

  virtual void resetStatistics()=0;

  virtual void run(double timestep)=0;

  virtual ~ParamScanInterface()
  {

  };


};

template <class Engine = Eval::bci::Engine<Eigen::VectorXd> >
class SSAparamScan
  : public ParamScanInterface
{
protected:

    Ast::Model& sbml_model;
    double transientTime;

    std::vector<Models::GenericOptimizedSSA<Engine>*> simulators;

    size_t _n;

    Eigen::MatrixXd _mean;
    Eigen::MatrixXd _cov;

    // Something to store the result
    std::vector<Eigen::VectorXd> state;


public:
    SSAparamScan(Ast::Model &model, std::vector<ParameterSet> &parameterSets,
                 double transientTime, size_t numThreads=OpenMP::getMaxThreads(), size_t opt_level=0)
      : sbml_model(model), transientTime(transientTime),
        simulators(parameterSets.size()),
        _n(1), state(numThreads)
    {

      _mean = Eigen::MatrixXd::Zero(parameterSets.size(),sbml_model.numSpecies());
      _cov  = Eigen::MatrixXd::Zero(parameterSets.size(),sbml_model.numSpecies()*(sbml_model.numSpecies()+1)/2);

      // Create SSA for all parameter sets
      for(size_t j = 0; j < parameterSets.size(); j++)
      {
        // Copy model
        Ast::Model mod(sbml_model);
        // Apply parameter set
        for(ParameterSet::iterator it=parameterSets[j].begin(); it!=parameterSets[j].end(); it++)
          mod.getParameter((*it).first)->setValue((*it).second);
        // Create SSA
        simulators[j] = new Models::GenericOptimizedSSA<Engine>(mod, 1, time(0), opt_level, 1);
      }

      // Advance state
#pragma omp parallel for if(numThreads>1) num_threads(numThreads) schedule(dynamic)
      for(size_t j = 0; j < parameterSets.size(); j++)
      {
        simulators[j]->run(transientTime);
      }
    }


    void run(double timestep)
    {      
      size_t numThreads = state.size();

      // Do the average average over time
#pragma omp parallel for if(numThreads>1) num_threads(numThreads) schedule(dynamic)
      for(size_t i = 0; i < simulators.size(); i++)
      {
        simulators[i]->run(timestep);
        state[OpenMP::getThreadNum()] = simulators[i]->getState().row(0);
        size_t idx =0;
        for(size_t j=0; j<simulators[i]->numSpecies(); j++)
        {
          _mean(i,j) =( _mean(i,j)*(_n-1) + state[OpenMP::getThreadNum()](j) )/_n;
          for (size_t k=0; k<=j; k++)
          {
            _cov(i,idx) = ( _cov(i,idx)*(_n-1) + (state[OpenMP::getThreadNum()](k)-_mean(i,k))*(state[OpenMP::getThreadNum()](j)-_mean(i,j)) )/_n;
            idx++;
          }
        }
      }

      // Increment internal counter
      _n++;
    }


    const Eigen::MatrixXd& getMean() const
    {
      return this->_mean;
    }


    const Eigen::MatrixXd& getCovariance() const
    {
      return this->_cov;
    }

    void resetStatistics()
    {

        this->_n=1;
        this->_mean = Eigen::MatrixXd::Zero(_mean.rows(),_mean.cols());
        this->_cov  = Eigen::MatrixXd::Zero(_cov.rows(),_cov.cols());

    }



};



}}


#endif // SSAPARAMSCAN_HH
