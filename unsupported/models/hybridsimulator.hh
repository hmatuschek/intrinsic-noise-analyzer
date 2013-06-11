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
class HybridSimulator
    : public OptimizedSSA

{



protected:

  Ast::Model& intModel;
  Ast::Model& extModel;

  Models::IOSmodel sseModel;

  GenericSSEinterpreter<IOSmodel,VectorEngine, MatrixEngine> SSEint;
  std::vector<ODE::LsodaDriver< GenericSSEinterpreter<IOSmodel,VectorEngine, MatrixEngine> > * > ODEint;

  // Vector for time of next jump
  std::vector<double> tjump;

public:
  /**
    Constructor
  **/
  HybridSimulator(Models::HybridModel &model, size_t ensembleSize, double epsilon_abs, double epsilon_rel)
    : OptimizedSSA(model.getExternalModel(), ensembleSize, time(0), 0, 1),
      intModel(model), extModel(model.getExternalModel()),
      sseModel(intModel),
      SSEint(sseModel,extModel,1),
      ODEint(ensembleSize),
      tjump(ensembleSize,0)
  {
    for(size_t i=0; i<ensembleSize; i++)
    {
      ODEint[i] = new ODE::LsodaDriver< GenericSSEinterpreter<IOSmodel,VectorEngine, MatrixEngine> >(SSEint,1,epsilon_abs,epsilon_rel);
    }
  }

  ~HybridSimulator()
  {

    // Clean up after you ...
    for(int i=0; i<ensembleSize; i++)
    {
      delete ODEint[i]; ODEint[i]=0;
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


  /**
   * Run hybrid method in continuous time.
   *
   */
  void runHybridSingle(Eigen::VectorXd &state, const int &sid, const double &t_in, const double &t_out)

  {

      // Perform jump it occurs before exit time
      if(t_out>tjump[sid])
      {
        double oldjumptime=tjump[sid];
        // Perform single SSA step and update tjump
        this->singleStep(tjump[sid],sid);
        // Reset Integrator and integrate until time of jump
        ODEint[sid]->step(state,t_in,oldjumptime);

        // Now update state vector (perform jump)
        state.tail(this->getState().cols()) = this->getState().row(sid);
        // Reset ODE integrator
        ODEint[sid]->reset();

        // Complete integration from time of jump
        runHybridSingle(state, sid, oldjumptime, t_out);
      }
      else // Simply update state
      {
        ODEint[sid]->step(state,t_in,t_out);
      }

  }

  /**
   * Run hybrid ensemble in continuous time.
   */
  void runHybrid(std::vector<Eigen::VectorXd> &stateMatrix, double t_in, double t_out)
  {

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

  typedef std::map<Eigen::VectorXd,size_t,lessVec> histType;
  typedef std::map<Eigen::VectorXd,Eigen::VectorXd,lessVec> condMeanType;
  typedef std::map<Eigen::VectorXd,Eigen::MatrixXd,lessVec> condVarType;

  void dynError(const std::vector<Eigen::VectorXd> &stateMatrix, condMeanType &condMean, condVarType &condVar)
  {

    histType histExt;
    dynError(stateMatrix, condMean, condVar);

  }

  void dynError(const std::vector<Eigen::VectorXd> &stateMatrix, condVarType &condVar)

  {

    histType histExt;
    condMeanType condMean;
    dynError(stateMatrix, condMean, condVar);

  }

  void dynError(const std::vector<Eigen::VectorXd> &stateMatrix, histType &histExt,
                condMeanType &condMean, condVarType &condVar)

  {

    Eigen::VectorXd concentration,emre,iosemre,skewness;
    Eigen::MatrixXd cov, iosCov;

    // Clear input variables
    condVar.clear();
    condMean.clear();
    histExt.clear();

    std::map<Eigen::VectorXd,Eigen::MatrixXd,lessVec>::iterator item;
    for(int sid=0; sid<ensembleSize; sid++)
    {

      const Eigen::VectorXd &state = stateMatrix[sid];
      sseModel.fullState(state,concentration,cov,emre,iosCov,skewness,iosemre);

      // Specify signal of interest.
      Eigen::VectorXd SoI = state.tail(extModel.numSpecies());

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

    // Normalize
    for(item = condVar.begin(); item!=condVar.end(); item++)
    {
      condMean[item->first] /= histExt[item->first];
      item->second /= histExt[item->first];
      item->second -= (condMean[item->first]*(condMean[item->first].transpose()));
    }


  }


  Eigen::MatrixXd mechError(const std::vector<Eigen::VectorXd> &stateMatrix, Eigen::MatrixXd &mechEcov)

  {

    // Zero matrix
    mechEcov = Eigen::MatrixXd::Zero(intModel.numSpecies(),intModel.numSpecies());

    Eigen::VectorXd mean,emre,iosemre,skewness;
    Eigen::MatrixXd cov, iosCov;

    for(int sid=0; sid<ensembleSize; sid++)
    {

      const Eigen::VectorXd &state = stateMatrix[sid];
      sseModel.fullState(state,mean,cov,emre,iosCov,skewness,iosemre);

      mechEcov += cov;

    }

    return mechEcov/ensembleSize;

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

      // Do SSE step and obtain updated t
      ODEint[sid]->step(state,t_in,t);

      // Update state vector
      state.tail(this->getState().cols()) = this->getState().row(sid);

    }






};


}
}

#endif
