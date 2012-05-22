#ifndef __FLUC_MODELS_OPTIMIZEDSSA_H
#define __FLUC_MODELS_OPTIMIZEDSSA_H

#include "stochasticsimulator.hh"
#include "constantstoichiometrymixin.hh"
#include "extensivespeciesmixin.hh"
#include "eval/bci/bci.hh"
#include "openmp.hh"


#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <eigen3/Eigen/Sparse>

namespace Fluc {
namespace Models {

/**
 * Optimized SSA using a dependency graph for propensity updates.
 *
 * The dependency graph lists all propensities that are affected by a reaction and need to be updated.
 *
 * This is an implementation of the algorithm as described in \cite cao2004.
 *
 * @ingroup models
 */

class OptimizedSSA :
  public StochasticSimulator,
  public ConstantStoichiometryMixin

{
protected:

    /**
    * Holds the dependency graph in terms of bytecode.
    **/
    std::vector<Evaluate::bci::Code> byte_code;

    /**
    * Collects all bytecode to evaluate all propensities.
    **/
    Evaluate::bci::Code all_byte_code;

    /**
    * Sparse stoichiometric matrix.
    **/
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
     **/
    OptimizedSSA(libsbml::Model *model, int ensembleSize, int seed,
                 size_t opt_level=0, size_t num_threads=OpenMP::getMaxThreads());

    /**
    *  The stepper for the SSA
    **/
    void run(double step);

private:
    /**
    * Reserves space for propensities of each threads.
    */
    std::vector< Eigen::VectorXd > prop;

    /**
    * Interpreter for each thread.
    */
    std::vector< Evaluate::bci::Interpreter< Eigen::VectorXd > > interpreter;

};

}
}

#endif // __FLUC_OPTIMIZEDSSA_H
