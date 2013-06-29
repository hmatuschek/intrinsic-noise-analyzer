#ifndef __INA_MODELS_ARRAYSSA_HH
#define __INA_MODELS_ARRAYSSA_HH

#include "unsupported.hh"
#include "models/constantstoichiometrymixin.hh"
#include "eval/bci/engine.hh"
#include "openmp.hh"

#include <eigen3/Eigen/Sparse>

namespace iNA {
namespace Models {

class ParametricSSABase :
    public BaseModel, ParticleNumbersMixin, ReasonableModelMixin

{
private:
  /**
   * Number of OpenMP threads to be used.
   */
  size_t num_threads;

protected:
  /**
   * A vector of thread-private RNGs.
   */
  std::vector<MersenneTwister> rand;

  /**
  * index map for bytecode interpreter
  **/
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> stateIndex;

  /**
  * data matrix storing each individual observation
  **/
  Eigen::MatrixXd observationMatrix;

  /**
  * Stores the initial conditions of a simulator.
  **/
  Eigen::VectorXd ics;

  /**
  * Vector containing the values of the compartment volumes for each reactant
  **/
  Eigen::VectorXd Omega;

  /**
   * Internal storage of ensemble size.
   **/
  int ensembleSize;

public :

  ParametricSSABase(Models::HybridModel &model, int size, int seed, size_t threads,
                                   const std::vector<const GiNaC::symbol *> &parameters)
  : BaseModel(model),
    ParticleNumbersMixin((BaseModel &)(*this)),
    ReasonableModelMixin((BaseModel &)(*this)),
    num_threads(threads),
    rand(1),
    observationMatrix(size,numSpecies()+parameters.size()),
    ics(numSpecies()), Omega(numSpecies()), ensembleSize(size)

  {

      // set number of threads
      if(num_threads > OpenMP::getMaxThreads())
      this->num_threads = OpenMP::getMaxThreads();

      // seed random number generators with a single seed
      rand.resize(this->num_threads);
      rand[0].seed(seed);
      for(size_t i=1;i<rand.size();i++)
      rand[i].seed(rand[i-1].rand_int());

      // make index table
      for(size_t i=0; i<this->numSpecies(); i++)
      this->stateIndex.insert(std::make_pair(this->getSpecies(i)->getSymbol(),i));

      // add parameters
      for(size_t i=0; i<parameters.size(); i++)
      this->stateIndex.insert(std::make_pair(*(parameters[i]),this->numSpecies()+i));

      // fold all constants
      Trafo::ConstantFolder constants(*this);
      for(size_t i=0;i<this->propensities.size();i++)
        this->propensities[i] = constants.apply(this->propensities[i]);

      // evaluate initial concentrations & get volumes
      Trafo::InitialValueFolder evICs(*this);

      for(size_t i=0; i<species.size();i++)
      {
        ics(i)=evICs.evaluate(this->species[i]);
        if(ics(i)>0.)
        {
          ics(i)=std::floor( ics(i) + 0.5 );
          if(ics(i)==0.) {
              InternalError err;
              err << "Cannot initiate Stochastic Simulation since initial particle number of species <i>"
                  << this->getSpecies(i)->getLabel() << "</i> evaluated to zero.";
              throw err;
              throw InternalError();
          }
        }
        else if(ics(i)<0.)
        {
           InternalError err;
           err << "Cannot initiate Stochastic Simulation since initial particle number of species <i>"
               << this->getSpecies(i)->getLabel() << "</i> evaluated to a value < 0.";
           throw err;
           throw InternalError();
        }

        this->Omega(i)=evICs.evaluate(this->volumes(i));

        if (this->Omega(i) <= 0) {
           InternalError err;
           err << "Cannot initiate Stochastic Simulation since compartment <i>"
               << this->getSpecies(i)->getCompartment()->getLabel()
               << "</i> evaluated to non-positive value.";
           throw err;
           throw InternalError();
        }
      }

      // initialize ensemble
      for(int i=0; i<this->ensembleSize;i++) {
        this->observationMatrix.row(i).head(this->numSpecies()) = ics;
      }

      // Initialize external parameter
      Trafo::InitialValueFolder extICs(model.getExternalModel());
      Eigen::VectorXd eICs(model.getExternalModel().numSpecies());

      for(size_t i=0; i<model.getExternalModel().numSpecies(); i++)
        eICs(i)=extICs.evaluate(model.getExternalModel().getSpecies(i)->getSymbol());

      for(int i=0; i<this->ensembleSize;i++) {
        this->observationMatrix.row(i).tail(model.getExternalModel().numSpecies()) = eICs;
      }

  }

  virtual ~ParametricSSABase()
  {
  //...
  }

  void getState(Eigen::MatrixXd &state)
  {
    state = (observationMatrix*this->Omega.asDiagonal().inverse());
  }

  Eigen::MatrixXd getState() const
  {
    return (observationMatrix.leftCols(this->numSpecies())*this->Omega.asDiagonal().inverse());
  }


  Eigen::MatrixXd & getObservationMatrix()
  {
    return this->observationMatrix;
  }

  size_t size()
  {
    return this->ensembleSize;
  }

  const size_t numThreads()
  {
    return this->num_threads;
  }

  Ast::Unit getConcentrationUnit() const
  {
    return concentrationUnit;
  }


};


/**
 * @ingroup unsupported
 */
template <class Engine>
class GenericParametricSSA :
  public ParametricSSABase,
  public ConstantStoichiometryMixin
{

protected:
  /** Holds the dependency graph in terms of bytecode. **/
  std::vector<typename Engine::Code *> byte_code;

  /** Collects all bytecode to evaluate all propensities. **/
  typename Engine::Code all_byte_code;

  /** Sparse stoichiometric matrix. **/
  Eigen::SparseMatrix<double> sparseStoichiometry;

public:

  /**
   * Is constructed from a Models::HybridModel.
   *
   * @param model Specifies the model, the construct the SSA analysis for.
   * @param seed A seed for the random number generator.
   * @param opt_level Specifies the byte-code optimization level.
   * @param num_threads Specifies the number of threads to use.
   * @param params External parameter vector.
   */
  GenericParametricSSA(Models::HybridModel &model, int seed,
               size_t opt_level=0, size_t num_threads=OpenMP::getMaxThreads(),
               const std::vector<const GiNaC::symbol *> params = std::vector<const GiNaC::symbol *>())
    : ParametricSSABase(model, 2, seed, num_threads, params),
      ConstantStoichiometryMixin((BaseModel &)(*this)),
      byte_code(this->numReactions()), all_byte_code(),
      sparseStoichiometry(numSpecies(),numReactions()),
      prop( this->numThreads(), Eigen::VectorXd::Zero(this->numReactions()) ),
      interpreter( this->numThreads() )
  {

    // First, allocate and initialize byte-code instances:
    for (size_t i=0; i<byte_code.size(); i++) {
      byte_code[i] = new typename Engine::Code();
    }

    typename Engine::Compiler compiler(this->stateIndex);

    // setup the interpreter from a dependency graph
    int d;
    for (size_t j=0;j<this->numReactions();j++)
    {
      // Set byte-code to compile into:
      compiler.setCode(byte_code[j]);

      for (size_t i=0;i<this->numReactions();i++)
      {
        d=0;
        for(size_t k=0;k<this->numSpecies();k++) {
          // ask if species k is changed by reaction j
          // which affects the propensities of reaction i in which species k is a modifier or a reactant
          d = d || ( (this->reactants_stoichiometry(k,i)!=0 || this->propensities[i].has(this->species[k]) ) && this->stoichiometry(k,j)!=0 ) ;
        }

        if (d!=0) {
          compiler.compileExpressionAndStore(this->propensities[i],i);
        }
      }

      // optimize bytecode
      compiler.finalize(opt_level);
    }

    compiler.setCode(&all_byte_code);
    // and compile propensities for byte code evaluation
    for(size_t i=0; i<this->numReactions(); i++)
      compiler.compileExpressionAndStore(this->propensities[i],i);

    // optimize and store
    compiler.finalize(opt_level);

    // fill sparse stoichiometry
    for(size_t j=0; j<this->numReactions(); ++j)
    {
      this->sparseStoichiometry.startVec(j);
      for(size_t i=0; i<this->numSpecies(); i++)
        if (this->stoichiometry(i,j)!=0) this->sparseStoichiometry.insertBack(i,j) = this->stoichiometry(i,j);
    }
    this->sparseStoichiometry.finalize();
  }


  /** Destructor, also frees byte-code instances. */
  virtual ~GenericParametricSSA()
  {
    for (size_t i=0; i < this->byte_code.size(); i++) {
      delete byte_code[i];
    }
  }

  /**
   * The stepper
   */
  void run(double step, Eigen::MatrixXd &state)
  {
    // initialization
    double propensitySum;	        // sum of propensities
    double t,tau;			// time between reactions
    size_t reaction;			// reaction number selected

    for(int sid=0;sid<state.rows();sid++)
    {
      //reset time
      t=0;

      interpreter[OpenMP::getThreadNum()].setCode(&all_byte_code);
      interpreter[OpenMP::getThreadNum()].run(state.row(sid), prop[OpenMP::getThreadNum()]);

      // initialize sum propensities
      propensitySum = prop[OpenMP::getThreadNum()].sum();

      while(t < step)
      {
        // sample tau
        if(propensitySum > 0) {
          tau = -std::log(this->rand[OpenMP::getThreadNum()].rand()) / propensitySum;
        } else {
          break;
        }

        // update time
        t += tau;
        if(t > step) break;

        // select reaction
        double r = this->rand[OpenMP::getThreadNum()].rand()*propensitySum;
        double sum = prop[OpenMP::getThreadNum()](0);
        reaction = 0;
        while(sum < r)
          sum += prop[OpenMP::getThreadNum()](++reaction);

        // update population of chemical species
        for (Eigen::SparseMatrix<double>::InnerIterator it(this->sparseStoichiometry,reaction); it; ++it) {
          state(sid,it.row())+=it.value();
        }

        // update only propensities that are changed in reaction
        interpreter[OpenMP::getThreadNum()].setCode(byte_code[reaction]);
        interpreter[OpenMP::getThreadNum()].run(state.row(sid),prop[OpenMP::getThreadNum()]);

        propensitySum = prop[OpenMP::getThreadNum()].sum();

      } //end time step loop
    } // end ensemble loop
  }

  Eigen::VectorXd &getOmega()
  {
    return Omega;
  }


private:
    /** Reserves space for propensities of each threads. */
    std::vector< Eigen::VectorXd > prop;

    /** Interpreter for each thread. */
    std::vector< typename Engine::Interpreter > interpreter;
};


/** Defines the default implementation of the optimized SSA, using the byte-code interpreter. */
typedef GenericParametricSSA< Eval::bci::Engine<Eigen::VectorXd> > ParametricSSA;

}
}

#endif // __INA_ARRAYSSA_HH
