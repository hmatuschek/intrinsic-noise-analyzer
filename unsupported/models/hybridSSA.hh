#ifndef __INA_HYBRIDSSA_HH
#define __INA_HYBRIDSSA_HH

#include "hybridsimulator.hh"
#include "arraySSA.hh"

namespace iNA {
namespace Models{

class HybridSSA
    : public GenericHybridSimulator<Eigen::VectorXd>
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


  Eigen::VectorXd getInitial()

  {
      return (ssaSim[0]->getObservationMatrix().row(0)).tail(this->numExtVars);
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


  void condStat(const std::vector<Eigen::VectorXd> &stateMatrix, histType &histExt,
                condMeanType &condMean, condVarType &condVar, size_t level)

  {

    std::vector<Eigen::VectorXd> mean(level);
    std::vector<Eigen::MatrixXd> cov(level);

    Eigen::VectorXd SoI(this->SoIdim);

    // Clear input variables
    condVar.clear();
    condMean.clear();
    histExt.clear();

    condVarType::iterator item;
    for(int sid=0; sid<ensembleSize; sid++)
    {

      const Eigen::VectorXd &state = stateMatrix[sid];
      getInternalStats(state,sid,mean,cov);

      Eigen::VectorXd m = mean[0];
      for(size_t i=1; i<level; i++)
        m+=mean[1];

      // Evaluate signal of interest.
      SoIint.run(state,SoI); //state.tail(extModel.numSpecies()).head(1);

      // Collect mean, variance and histogram
      item = condVar.find(SoI);
      if(item == condVar.end())
      {
        condMean.insert(std::make_pair(SoI,m));
        condVar.insert(std::make_pair(SoI,m*(m.transpose())));
        histExt.insert(std::make_pair(SoI,1));
      }
      else
      {
        item->second += m*m.transpose();
        condMean[SoI] += m;
        histExt[SoI] += 1;
      }

    } // finished ensemble average

    double norm=0;

    // Normalize
    for(item = condVar.begin(); item!=condVar.end(); item++)
    {
      // Divide by number of occurences
      condMean[item->first] /= histExt[item->first];
      item->second /= histExt[item->first];
      // Substract mean
      item->second -= (condMean[item->first]*(condMean[item->first].transpose()));
      // Compute normalization factor of histogram
      norm += histExt[item->first];
    }

    // Normalize histogram
    for(item = condVar.begin(); item!=condVar.end(); item++)
    {
        histExt[item->first]/=norm;
    }

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

      for(condMeanType::iterator item = condMean.begin();
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



class DualReporter
    : public GenericHybridSimulator<Eigen::MatrixXd>
{

protected:

  size_t numExtVars;

  size_t numExtEns;
  size_t numIntEns;

  ParametricSSA * SSA;

public:


  DualReporter(Models::HybridModel &model, size_t numExtEns, size_t num_threads=OpenMP::getMaxThreads())
    : GenericHybridSimulator(model, numExtEns, num_threads),
      numExtVars(model.getExternalModel().numSpecies()),
      numExtEns(numExtEns), numIntEns(2)

  {

    // Construct parameter table
    std::vector<const GiNaC::symbol *> params;
    for(size_t i=0; i<model.getExternalModel().numSpecies(); i++)
      params.push_back(&(model.getExternalModel().getSpecies(i)->getSymbol()));

    SSA = new ParametricSSA(model,time(0),1,num_threads,params);

    // Make lookup for signal of interest
    std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> extIndex;
    for(size_t i=0; i<extModel.numSpecies(); i++)
      extIndex.insert(std::make_pair(extModel.getSpecies(i)->getSymbol(),i+model.numSpecies()));

    // Compile
    VectorEngine::Compiler compiler(extIndex);
    compiler.setCode(&SoIcode);
    const SignalOfInterest &soi = model.getSoI();
    for(size_t i=0; i<soi.size(); i++)
       compiler.compileExpressionAndStore(soi[i],i);
    compiler.finalize(1);
    SoIint.setCode(&SoIcode);

  }

  virtual ~DualReporter()

  {
      delete SSA; SSA=0;
  }

  Eigen::MatrixXd getInitial()

  {
    return SSA->getObservationMatrix();
  }

  void resetInternal()

  {
    // Pass...
  }

  void runInternal(Eigen::MatrixXd &state, const size_t &sid, const double &t_in, const double &t_out)

  {
     // Run the actual simulation
     SSA->run(t_out-t_in, state);
  }

  void getInternalStats(const Eigen::VectorXd &state, Eigen::VectorXd &mean)

  {
    mean = state.head(state.size()-SSA->numSpecies());
  }

  void condStat(const std::vector<Eigen::MatrixXd> &stateMatrix, histType &histExt,
                condMeanType &condMean)

  {

    Eigen::VectorXd mean;
    Eigen::VectorXd SoI(this->SoIdim);

    // Clear input variables
    condMean.clear();
    histExt.clear();

    condMeanType::iterator item;
    for(size_t sid=0; sid<stateMatrix.size(); sid++)
    {

      const Eigen::VectorXd &state = stateMatrix[sid].row(0);
      getInternalStats(state,mean);

      // Evaluate signal of interest.
      SoIint.run(state,SoI);

      // Collect mean, variance and histogram
      item = condMean.find(SoI);
      if(item == condMean.end())
      {
        condMean.insert(std::make_pair(SoI,mean));
        histExt.insert(std::make_pair(SoI,1));
      }
      else
      {
        condMean[SoI] += mean;
        histExt[SoI] += 1;
      }

    } // finished ensemble average

    double norm=0;
    for(item = condMean.begin(); item!=condMean.end(); item++)
    {
      // Divide by number of occurences
      condMean[item->first] /= histExt[item->first];
      // Compute normalization factor of histogram
      norm += histExt[item->first];
    }

    // Normalize histogram
    for(item = condMean.begin(); item!=condMean.end(); item++)
    {
        histExt[item->first]/=norm;
    }

  }

  void mechError(const std::vector<Eigen::MatrixXd> &stateMatrix, Eigen::MatrixXd &mechErr)

  {

    // Zero matrix
    mechErr = Eigen::MatrixXd::Zero(intModel.numSpecies(),intModel.numSpecies());

    Eigen::VectorXd r1,r2;
    for(size_t sid=0; sid<stateMatrix.size(); sid++)
    {
      r1 = stateMatrix[sid].row(0).head(intModel.numSpecies());
      r2 = stateMatrix[sid].row(1).head(intModel.numSpecies());
      mechErr += (r1-r2)*((r1-r2).transpose());
    }

    mechErr/=2.*stateMatrix.size();

  }


  void covariance(const std::vector<Eigen::MatrixXd> &stateMatrix, Eigen::MatrixXd &cov)

  {

    // Zero matrix
    cov = Eigen::MatrixXd::Zero(intModel.numSpecies(),intModel.numSpecies());
    Eigen::VectorXd mean = Eigen::VectorXd::Zero(intModel.numSpecies());

    Eigen::VectorXd r1;
    for(size_t sid=0; sid<stateMatrix.size(); sid++)
    {
      r1 = stateMatrix[sid].row(0).head(intModel.numSpecies());
      mean += r1;
      cov.noalias()  += (r1)*(r1.transpose());
    }

    mean/=stateMatrix.size();
    cov/=stateMatrix.size();

    cov.noalias() -= mean*(mean.transpose());

  }

  void fidError(const std::vector<Eigen::MatrixXd> &stateMatrix, Eigen::VectorXd &mean, Eigen::MatrixXd &transErr, Eigen::MatrixXd &fidErr, size_t level=1)

  {
      // Zero input
      mean = Eigen::VectorXd::Zero(intModel.numSpecies());
      transErr = Eigen::MatrixXd::Zero(intModel.numSpecies(),intModel.numSpecies());

      histType histExt;
      condMeanType condMean;
      condStat(stateMatrix, histExt, condMean);

      for(condMeanType::iterator item = condMean.begin(); item!=condMean.end(); item++)
      {
          mean += (item->second)*histExt[item->first];
          transErr += (item->second)*(item->second.transpose())*histExt[item->first];
      }

      // Substract mean
      transErr.noalias() -=  mean*(mean.transpose());

      this->covariance(stateMatrix, fidErr);
      fidErr -= transErr;

  }

  void makeConcentrations(Eigen::VectorXd &vec)

  {
    vec = SSA->getOmega().asDiagonal().inverse()*vec;
  }

  void makeConcentrations(Eigen::MatrixXd &cov)

  {
    cov = (SSA->getOmega().asDiagonal().inverse())*cov*(SSA->getOmega().asDiagonal().inverse());
  }

};

}}
#endif // HYBRIDSSA_HH
