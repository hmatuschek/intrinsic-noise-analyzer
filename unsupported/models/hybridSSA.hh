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

  std::vector<Models::OptimizedSSA *> ssaSim;

public:


  HybridSSA(Models::HybridModel &model, size_t ensembleSize, size_t num_threads=OpenMP::getMaxThreads())
    : GenericHybridSimulator(model, ensembleSize, num_threads),
      numExtVars(model.getExternalModel().numSpecies()),
      ssaSim(ensembleSize)

  {

    // Construct parameter table
    std::vector<const GiNaC::symbol *> params;
    for(size_t i=0; i<model.getExternalModel().numSpecies(); i++)
      params.push_back(&(model.getExternalModel().getSpecies(i)->getSymbol()));

    for(int i=0; i<this->ensembleSize; i++)
      ssaSim[i] = new OptimizedSSA(model,ensembleSize,time(0),1,1,params);

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

    for(int i=0; i<this->ensembleSize; i++)
    {
      delete ssaSim[i]; ssaSim[i]=0;
    }

  }


  void getInitial(Eigen::VectorXd &state)

  {
      state = (ssaSim[0]->getObservationMatrix().row(0)).tail(this->numExtVars);
  }


  void runInternal(Eigen::VectorXd &state, const size_t &sid, const double &t_in, const double &t_out)

  {

     // Update external parameters
     for(int i=0; i<ssaSim[OpenMP::getThreadNum()]->getObservationMatrix().rows();i++)
        ssaSim[sid]->getObservationMatrix().row(i).tail(numExtVars) = state; //.tail(numExtVars);

     // Run the actual simulation
     ssaSim[sid]->run(t_out-t_in);

  }


  void reset()

  {
    // Think about it
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
