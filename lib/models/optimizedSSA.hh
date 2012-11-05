#ifndef __INA_MODELS_OPTIMIZEDSSA_H
#define __INA_MODELS_OPTIMIZEDSSA_H

#include "stochasticsimulator.hh"
#include "constantstoichiometrymixin.hh"
#include "extensivespeciesmixin.hh"
#include "eval/bci/engine.hh"
#include "openmp.hh"


#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <eigen3/Eigen/Sparse>

namespace iNA {
namespace Models {

/**
 * Optimized SSA using a dependency graph for propensity updates.
 *
 * The dependency graph lists all propensities that are affected by a reaction and need to be updated.
 *
 * This is an implementation of the algorithm as described in \cite cao2004.
 *
 * @ingroup ssa
 */
template <class Engine>
class GenericOptimizedSSA :
  public StochasticSimulator,
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
   * Is constructed from a SBML model.
   *
   * @param model Specifies the model, the construct the SSA analysis for.
   * @param ensembleSize Specifies the ensemble size to use.
   * @param seed A seed for the random number generator.
   * @param opt_level Specifies the byte-code optimization level.
   * @param num_threads Specifies the number of threads to use.
   */
  GenericOptimizedSSA(const Ast::Model &model, int ensembleSize, int seed,
               size_t opt_level=0, size_t num_threads=OpenMP::getMaxThreads())
    : StochasticSimulator(model, ensembleSize, seed, num_threads),
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
  ~GenericOptimizedSSA()
  {
    for (size_t i=0; i < this->byte_code.size(); i++) {
      delete byte_code[i];
    }
  }

  /** Reimplement evaluate using the generic interpreter. */
  void
  evaluate(const Eigen::VectorXd &state, Eigen::VectorXd &propensities)

  {

    interpreter[0].setCode(&all_byte_code);
    interpreter[0].run(state, propensities);

  }


  /**
   * The stepper for the SSA
   */
  void run(double step)
  {
    // initialization
    double propensitySum;	        // sum of propensities
    double t,tau;			// time between reactions
    size_t reaction;			// reaction number selected

#pragma omp parallel for if(this->numThreads()>1) num_threads(this->numThreads()) schedule(dynamic) private(propensitySum,tau,t,reaction)
    for(int sid=0;sid<this->ensembleSize;sid++)
    {
      t=0;

      interpreter[OpenMP::getThreadNum()].setCode(&all_byte_code);
      interpreter[OpenMP::getThreadNum()].run(this->observationMatrix.row(sid), prop[OpenMP::getThreadNum()]);

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

        // select reaction
        double r = this->rand[OpenMP::getThreadNum()].rand()*propensitySum;
        double sum = prop[OpenMP::getThreadNum()](0);
        reaction = 0;
        while(sum < r)
          sum += prop[OpenMP::getThreadNum()](++reaction);

        // update population of chemical species
        for (Eigen::SparseMatrix<double>::InnerIterator it(this->sparseStoichiometry,reaction); it; ++it) {
          this->observationMatrix(sid,it.row())+=it.value();
        }

        // update only propensities that are changed in reaction
        interpreter[OpenMP::getThreadNum()].setCode(byte_code[reaction]);
        interpreter[OpenMP::getThreadNum()].run(this->observationMatrix.row(sid),prop[OpenMP::getThreadNum()]);

        propensitySum = prop[OpenMP::getThreadNum()].sum();

        // update time
        t += tau;
      } //end time step loop
    } // end ensemble loop
  }

private:
    /** Reserves space for propensities of each threads. */
    std::vector< Eigen::VectorXd > prop;

    /** Interpreter for each thread. */
    std::vector< typename Engine::Interpreter > interpreter;
};


/** Defines the default implementation of the optimized SSA, using the byte-code interpreter. */
typedef GenericOptimizedSSA< Eval::bci::Engine<Eigen::VectorXd> > OptimizedSSA;

}
}

#endif // __FLUC_OPTIMIZEDSSA_H
