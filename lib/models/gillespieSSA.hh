#ifndef __FLUC_GILLESPIESSA_H
#define __FLUC_GILLESPIESSA_H

#include "stochasticsimulator.hh"
#include "constantstoichiometrymixin.hh"
#include "extensivespeciesmixin.hh"

#include "eval/bci/engine.hh"

#include <omp.h>


namespace Fluc {
namespace Models {

/**
 * Gillespie's Stochastic Simulation Algorithm using a bytecode interpreter.
 *
 * This is a implementation of the original algorithm as described in \cite gillespie1976 and
 * \cite gillespie1977.
 *
 * @ingroup models
 */
template <class Engine>
class GenericGillespieSSA :
  public StochasticSimulator,
  public ConstantStoichiometryMixin

{
private:

    /** Byte code for propensity evaluation. */
    typename Engine::Code bytecode;

    /**
     * Interpreter for each thread.
     */
    std::vector< typename Engine::Interpreter > interpreter;

    /**
     * Reserves space for propensities of each threads.
     */
    std::vector< Eigen::VectorXd > prop;

public:
    /**
     * Is initialized with a model, the number of realization @c ensembleSize and a seed for the
     * random number generator
     */
    GenericGillespieSSA(libsbml::Model *model, int ensembleSize, int seed, size_t opt_level=0, size_t num_threads=1)
      : StochasticSimulator(model, ensembleSize, seed, num_threads),
        ConstantStoichiometryMixin((BaseModel &)(*this)),
        interpreter(this->numThreads()), prop( this->numThreads(), Eigen::VectorXd::Zero(this->numReactions()) )
    {
      typename Engine::Compiler compiler(this->stateIndex);
      compiler.setCode(&bytecode);

      // and compile propensities for byte code evaluation
      for(size_t i=0; i<this->numReactions(); i++)
          compiler.compileExpressionAndStore(this->propensities[i],i);

      // optimize and store
      compiler.finalize(opt_level);
      for(size_t i=0; i<this->numThreads(); i++) {
          this->interpreter[i].setCode(&bytecode);
      }
    }

    /**
     *  the stepper for the SSA
     */
    void run(double step)
    {
      // initialization
      double propensitySum;	// sum of propensities
      double t,tau;			// time between reactions
      size_t reaction;		// reaction number selected

#pragma omp parallel for if(this->numThreads()>1) num_threads(this->numThreads()) schedule(dynamic) private(propensitySum,tau,t,reaction)
      for(int sid=0;sid<this->ensembleSize;sid++)
      {
        t=0;
        while(t < step)
        {
          // update propensity vector
          interpreter[OpenMP::getThreadNum()].run(this->observationMatrix.row(sid), this->prop[OpenMP::getThreadNum()]);

          // evaluate propensity sum
          propensitySum = this->prop[OpenMP::getThreadNum()].sum();

          // sample tau
          if(propensitySum > 0) {
            tau = -std::log(this->rand[OpenMP::getThreadNum()].rand()) / propensitySum;
          } else {
            break;
          }

          // select reaction
          double r = this->rand[OpenMP::getThreadNum()].rand()*propensitySum;
          double sum = this->prop[OpenMP::getThreadNum()](0);
          reaction = 0;
          while(sum < r)
            sum += this->prop[OpenMP::getThreadNum()](++reaction);

          // update chemical species
          this->observationMatrix.row(sid)+=this->stoichiometry.col(reaction);

          // time
          t += tau;
        } //end time step loop
      } // end ensemble loop
    }
};


/**
 * Implements the default Gillespie SSA implementation usign the byte-code interpreter.
 */
typedef GenericGillespieSSA< Evaluate::bci::Engine<Eigen::VectorXd> > GillespieSSA;

}
}

#endif // __FLUC_GILLESPIESSA_H
