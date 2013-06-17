#ifndef __INA_HYBRIDSIMULATOR_HH__
#define __INA_HYBRIDSIMULATOR_HH__

#include "ast/ast.hh"
#include <ginac/ginac.h>
#include "ina.hh"
#include "hybridmodel.hh"

namespace iNA {
namespace Models {


typedef Eval::bci::Engine<Eigen::VectorXd> VectorEngine;
typedef Eval::bci::Engine<Eigen::MatrixXd> MatrixEngine;


/**
 * Hybrid simulation getting serious.
 *
 * @ingroup unsupported
 */
class GenericHybridSimulator
    : public OptimizedSSA

{

protected:

  Ast::Model &intModel;
  Ast::Model &extModel;

  // Vector for time of next jump
  std::vector<double> tjump;


  VectorEngine::Code SoIcode;
  VectorEngine::Interpreter SoIint;

public:
  /**
    Constructor
  **/
  GenericHybridSimulator(Models::HybridModel &model, size_t ensembleSize, size_t num_threads=OpenMP::getMaxThreads())
    : OptimizedSSA(model.getExternalModel(), ensembleSize, time(0), 0, num_threads),
      intModel(model), extModel(model.getExternalModel()),
      tjump(ensembleSize,0)
  {

  }

  virtual ~GenericHybridSimulator()

  {

  }

  virtual void getInitial(Eigen::VectorXd &state)=0;

  virtual void runInternal(Eigen::VectorXd &state, const size_t &sid, const double &t_in, const double &t_out)=0;

  virtual void reset()=0;

  virtual void getInternalStats(const Eigen::VectorXd &state, const size_t &sid, Eigen::VectorXd &mean, Eigen::MatrixXd &cov)=0;

  /**
   * Run hybrid method in continuous time.
   *
   */
  void runHybridSingle(Eigen::VectorXd &state, const int &sid, const double &t_in, const double &t_out)

  {

      // Reset internal process
      this->reset();

      // Perform jump if it occurs before exit time
      if(t_out>tjump[sid])
      {
        double oldjumptime=tjump[sid];
        // Integrate until time of jump
        this->runInternal(state,sid,t_in,oldjumptime);

        // Now update state vector (perform jump)
        state.tail(this->getState().cols()) = this->getState().row(sid);

        // Perform single SSA step and update tjump
        this->singleStep(tjump[sid],sid);

        // Complete integration from time of jump
        runHybridSingle(state, sid, oldjumptime, t_out);
      }
      else // Simply update state
      {
        this->runInternal(state,sid,t_in,t_out);
      }


  }

  /**
   * Run hybrid ensemble in continuous time.
   */
  void runHybrid(std::vector<Eigen::VectorXd> &stateMatrix, double t_in, double t_out)
  {

#pragma omp parallel for if(this->numThreads()>1) num_threads(this->numThreads()) schedule(dynamic)
    for(int sid=0; sid<ensembleSize; sid++)
      this->runHybridSingle(stateMatrix[sid], sid, t_in, t_out);

  }

 class lessVec {
    public:
      bool operator()(const Eigen::VectorXd &x,const Eigen::VectorXd &y) const
      {
        for(int i=0; i<x.size(); i++)
          if (x(i) != y(i)) return (x(i)<y(i));
        return false;
      }
  };

  typedef std::map<Eigen::VectorXd,double,lessVec> histType;
  typedef std::map<Eigen::VectorXd,Eigen::VectorXd,lessVec> condMeanType;
  typedef std::map<Eigen::VectorXd,Eigen::MatrixXd,lessVec> condVarType;


  /**
  * Error of transformed signal. E[V(Z|s)]
  */
  void transError(const std::vector<Eigen::VectorXd> &stateMatrix, Eigen::VectorXd &mean, Eigen::MatrixXd &transErr)

  {
      // Zero input
      mean = Eigen::VectorXd::Zero(intModel.numSpecies());
      transErr = Eigen::MatrixXd::Zero(intModel.numSpecies(),intModel.numSpecies());

      histType histExt;
      condMeanType condMean;
      condVarType condVar;
      condStat(stateMatrix, histExt, condMean, condVar);

      for(std::map<Eigen::VectorXd, Eigen::VectorXd,lessVec>::iterator item = condMean.begin();
          item!=condMean.end(); item++)
      {
          mean += (item->second)*histExt[item->first];
          transErr += (item->second)*(item->second.transpose())*histExt[item->first];
      }

      // Substract mean
      transErr -=  mean*mean.transpose();

  }


  void dynError(const std::vector<Eigen::VectorXd> &stateMatrix, Eigen::MatrixXd &dynErr)

  {

      // Zero input
      dynErr = Eigen::MatrixXd::Zero(intModel.numSpecies(),intModel.numSpecies());

      histType histExt;
      condMeanType condMean;
      condVarType condVar;

      condStat(stateMatrix, histExt, condMean, condVar);

      for(std::map<Eigen::VectorXd, Eigen::MatrixXd,lessVec>::iterator item = condVar.begin();
          item!=condVar.end(); item++)
      {
          dynErr += (item->second)*histExt[item->first];
      }

  }


  void condStat(const std::vector<Eigen::VectorXd> &stateMatrix, histType &histExt,
                condMeanType &condMean, condVarType &condVar)

  {

    Eigen::VectorXd concentration;
    Eigen::MatrixXd cov;

    // Clear input variables
    condVar.clear();
    condMean.clear();
    histExt.clear();

    std::map<Eigen::VectorXd, Eigen::MatrixXd,lessVec>::iterator item;
    for(int sid=0; sid<ensembleSize; sid++)
    {

      const Eigen::VectorXd &state = stateMatrix[sid];
      getInternalStats(state,sid,concentration,cov);

      // Evaluate signal of interest.
      Eigen::VectorXd SoI(extModel.numSpecies());
      SoIint.run(state,SoI); //state.tail(extModel.numSpecies()).head(1);

      // Collect mean, variance and histogram
      item = condVar.find(SoI);
      if(item == condVar.end())
      {
        condMean.insert(std::make_pair(SoI,concentration));
        condVar.insert(std::make_pair(SoI,concentration*(concentration.transpose())));
        histExt.insert(std::make_pair(SoI,1));
      }
      else
      {
        item->second += concentration*concentration.transpose();
        condMean[SoI] += concentration;
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


  void mechError(const std::vector<Eigen::VectorXd> &stateMatrix, Eigen::MatrixXd &mechErr)

  {

    // Zero matrix
    mechErr = Eigen::MatrixXd::Zero(intModel.numSpecies(),intModel.numSpecies());

    Eigen::VectorXd mean,emre,iosemre,skewness;
    Eigen::MatrixXd cov, iosCov;

    for(int sid=0; sid<ensembleSize; sid++)
    {

      const Eigen::VectorXd &state = stateMatrix[sid];
      getInternalStats(state,sid,mean,cov);

      mechErr += cov;

    }

    mechErr/=ensembleSize;

  }

  /**
   * Run hybrid method reaction-wise.
   *
   */
  void runReactionWise(Eigen::VectorXd state, double &t,
                              size_t numThreads = OpenMP::getMaxThreads())

  {

      double t_in;
      size_t sid=0;

      // Store initial time
      t_in=t;

      // Perform timestep of the external model, exists with updated time t
      this->singleStep(t,sid);

      // Do SSE step
      this->runInternal(state,sid,t_in,t);
      this->reset();

      // Update state vector
      state.tail(this->getState().cols()) = this->getState().row(sid);

  }






};





class HybridSimulator
    : public GenericHybridSimulator
{

protected:

  Models::IOSmodel sseModel;

  std::vector<GenericSSEinterpreter<IOSmodel,VectorEngine, MatrixEngine> *> SSEint;
  std::vector<ODE::LsodaDriver< GenericSSEinterpreter<IOSmodel,VectorEngine, MatrixEngine> > * > ODEint;

public:

  HybridSimulator(Models::HybridModel &model, size_t ensembleSize, double epsilon_abs, double epsilon_rel, size_t num_threads=OpenMP::getMaxThreads())
    : GenericHybridSimulator(model, ensembleSize, num_threads),
      sseModel(intModel),
      SSEint(num_threads),
      ODEint(num_threads)
  {

    for(size_t i=0; i<num_threads; i++)
    {
      SSEint[i] = new GenericSSEinterpreter<IOSmodel,VectorEngine, MatrixEngine> (sseModel,extModel,1);
      ODEint[i] = new ODE::LsodaDriver< GenericSSEinterpreter<IOSmodel,VectorEngine, MatrixEngine> >(*SSEint[i],1,epsilon_abs,epsilon_rel);
    }

    // Compile signal of interest
    std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> extIndex;
    for(size_t i=0; i<extModel.numSpecies(); i++)
      extIndex.insert(std::make_pair(extModel.getSpecies(i)->getSymbol(),sseModel.getDimension()+i));

    VectorEngine::Compiler compiler(extIndex);

    compiler.setCode(&SoIcode);
    const SignalOfInterest &soi = model.getSoI();
    for(size_t i=0; i<soi.size(); i++)
       compiler.compileExpressionAndStore(soi[i],i);
    compiler.finalize(1);
    SoIint.setCode(&SoIcode);

  }

  virtual ~HybridSimulator()
  {
    // Clean up after you ...
    for(size_t i=0; i<this->numThreads(); i++)
    {
      delete ODEint[i]; ODEint[i]=0;
      delete SSEint[i]; SSEint[i]=0;
    }
  }

  void getInitial(Eigen::VectorXd &state)
  {

    // Initialize with initial concentrations
    state.resize(sseModel.getDimension()+extModel.numSpecies());
    Eigen::VectorXd init(sseModel.getDimension());
    sseModel.getInitialState(init);
    state.head(sseModel.getDimension())=init;

    // Evaluate initial concentration of external species
    // TODO check units here
    Trafo::InitialValueFolder evICs(extModel);
    size_t n = sseModel.getDimension();
    for(size_t i=0; i<extModel.numSpecies();i++)
    {
       state(n++)=evICs.evaluate(extModel.getSpecies(i)->getSymbol());
    }

  }

  void runInternal(Eigen::VectorXd &state, const size_t &sid, const double &t_in, const double &t_out)
  {
    ODEint[OpenMP::getThreadNum()]->step(state,t_in,t_out);
  }


  void reset()
  {
    // Reset ODE integrator
    ODEint[OpenMP::getThreadNum()]->reset();
  }


  void getInternalStats(const Eigen::VectorXd &state, const size_t &sid, Eigen::VectorXd &mean, Eigen::MatrixXd &cov)
  {

    Eigen::VectorXd emre,iosemre,skewness;
    Eigen::MatrixXd iosCov;
    sseModel.fullState(state,mean,cov,emre,iosCov,skewness,iosemre);

  }



};





}
}

#endif
