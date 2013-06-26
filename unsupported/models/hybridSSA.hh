#ifndef __INA_HYBRIDSSA_HH
#define __INA_HYBRIDSSA_HH

#include "hybridsimulator.hh"

namespace iNA {
namespace Models{

class HybridSSA
    : public GenericHybridSimulator
{

protected:

  size_t numExtVars;

  size_t numExtEns;
  size_t numIntEns;

  std::vector<Models::OptimizedSSA *> ssaSim;

public:


  HybridSSA(Models::HybridModel &model, size_t numExtEns, size_t num_threads=OpenMP::getMaxThreads())
    : GenericHybridSimulator(model, numExtEns, num_threads),
      numExtVars(model.getExternalModel().numSpecies()),
      numExtEns(numExtEns), numIntEns(2),
      ssaSim(numExtEns)

  {

    // Construct parameter table
    std::vector<const GiNaC::symbol *> params;
    for(size_t i=0; i<model.getExternalModel().numSpecies(); i++)
      params.push_back(&(model.getExternalModel().getSpecies(i)->getSymbol()));

    for(size_t i=0; i<numExtEns; i++)
      ssaSim[i] = new OptimizedSSA(model,numIntEns,time(0),1,1,params);

    // Make lookup for signal of interest
    std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> extIndex;
    for(size_t i=0; i<extModel.numSpecies(); i++)
      extIndex.insert(std::make_pair(extModel.getSpecies(i)->getSymbol(),i));

    // Compile
    VectorEngine::Compiler compiler(extIndex);
    compiler.setCode(&SoIcode);
    const SignalOfInterest &soi = model.getSoI();
    for(size_t i=0; i<soi.size(); i++)
       compiler.compileExpressionAndStore(soi[i],i);
    compiler.finalize(1);
    SoIint.setCode(&SoIcode);

  }


  virtual ~HybridSSA()
  {

    for(size_t i=0; i<numIntEns; i++)
    {
      delete ssaSim[i]; ssaSim[i]=0;
    }

  }


  void getInitial(Eigen::VectorXd &state)

  {
      state = (ssaSim[0]->getObservationMatrix().row(0)).tail(this->numExtVars);
  }

  void mechError(const std::vector<Eigen::VectorXd> &stateMatrix, Eigen::MatrixXd &mechErr)

  {

    // Zero matrix
    mechErr = Eigen::MatrixXd::Zero(intModel.numSpecies(),intModel.numSpecies());

    for(int sid=0; sid<ensembleSize; sid++)
    {
      Eigen::VectorXd r1 = ssaSim[sid]->getState().row(0).head(intModel.numSpecies());
      Eigen::VectorXd r2 = ssaSim[sid]->getState().row(1).head(intModel.numSpecies());
      mechErr += (r1-r2)*(r1-r2).transpose();
    }

    mechErr/=2*ensembleSize;

  }


  void covar(Eigen::MatrixXd &cov)

  {

    // Zero matrix
    cov = Eigen::MatrixXd::Zero(intModel.numSpecies(),intModel.numSpecies());
    Eigen::VectorXd mean = Eigen::VectorXd::Zero(intModel.numSpecies());

    for(int sid=0; sid<ensembleSize; sid++)
    {
      Eigen::VectorXd r1 = ssaSim[sid]->getState().row(0).head(intModel.numSpecies());
      mean.noalias() += r1;
      cov.noalias()  += (r1)*(r1.transpose());
    }

    mean/=ensembleSize;
    cov/=ensembleSize;

    cov -= mean*(mean.transpose());

  }

  void fidError(const std::vector<Eigen::VectorXd> &stateMatrix, Eigen::VectorXd &mean, Eigen::MatrixXd &transErr, Eigen::MatrixXd &fidErr, size_t level=1)

  {
      // Zero input
      mean = Eigen::VectorXd::Zero(intModel.numSpecies());
      transErr = Eigen::MatrixXd::Zero(intModel.numSpecies(),intModel.numSpecies());
      //fidErr = Eigen::MatrixXd::Zero(intModel.numSpecies(),intModel.numSpecies());

      histType histExt;
      condMeanType condMean;
      condVarType condVar;
      condStat(stateMatrix, histExt, condMean, condVar, level);

      for(std::map<Eigen::VectorXd, Eigen::VectorXd,lessVec>::iterator item = condMean.begin();
          item!=condMean.end(); item++)
      {
          mean += (item->second)*histExt[item->first];
          transErr += (item->second)*(item->second.transpose())*histExt[item->first];
      }

      // Substract mean
      transErr -=  mean*mean.transpose();

      covar(fidErr);
      fidErr -= transErr;

  }

  void resetInternal()

  {
    // Pass...
  }


  void runInternal(Eigen::VectorXd &state, const size_t &sid, const double &t_in, const double &t_out)

  {

     // Update external parameters
     for(int i=0; i<ssaSim[OpenMP::getThreadNum()]->getObservationMatrix().rows();i++)
        ssaSim[sid]->getObservationMatrix().row(i).tail(numExtVars) = state; //.tail(numExtVars);

     // Run the actual simulation
     ssaSim[sid]->run(t_out-t_in);

  }


  void resetEnsemble()

  {

      for(size_t sid=0; sid<numExtEns;sid++)
          ssaSim[sid]->reset();

      this->reset();

  }


  void getInternalStats(const Eigen::VectorXd &state, const size_t &sid, std::vector<Eigen::VectorXd> &mean, std::vector<Eigen::MatrixXd> &cov)

  {
    mean.resize(1);
    cov.resize(1);
    // Use covvec as a dummy here
    Eigen::VectorXd skew;
    ssaSim[sid]->stats(mean[0], cov[0], skew);

  }

};

}}
#endif // HYBRIDSSA_HH
