#ifndef __FLUC_MODELS_PARTIALSUMSSA_H
#define __FLUC_MODELS_PARTIALSUMSSA_H

#include "models/stochasticsimulator.hh"
#include "models/constantstoichiometrymixin.hh"
#include "models/extensivespeciesmixin.hh"
#include "intprt/intprt.hh"
#include "openmp.hh"


#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <eigen3/Eigen/Sparse>

namespace Fluc {
namespace Models {

/**
 * Optimized SSA using 2D search on partial sum.
 *
 * The dependency graph lists all propensities that are affected by a reaction and need to be updated.
 *
 * This is an implementation of the algorithm as described in \cite cao2004.
 *
 * @ingroup unsupported
 */

class PartialSumSSA :
  public StochasticSimulator,
  public ConstantStoichiometryMixin

{
protected:

    /**
    * Holds the dependency graph in terms of bytecode.
    **/
    std::vector<Intprt::ByteCode> byte_code;

    /**
    * Collects all bytecode to evaluate all propensities.
    **/
    Intprt::ByteCode all_byte_code;

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
    PartialSumSSA(libsbml::Model *model, int ensembleSize, int seed,
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
    std::vector< Intprt::Interpreter< Eigen::VectorXd > > interpreter;

    /**
    * Sqrt of numReactions().
    **/
    size_t mSize;

    std::vector< Eigen::VectorXd > partialSums;

    /**
    * priorityList maps a matrix to the vector of propensities which want to permutate on the lattice.
    **/
    Eigen::MatrixXi priorityList;
    /**
    * yields group for reaction.
    **/
    Eigen::VectorXi groupList;

    /**
    * The dependency graph \f$ G_{ij} \f$ is nonzero if reaction \f$ i \f$ is affects the rates of reaction \f$ j \f$.
    **/
    Eigen::SparseMatrix<int> dependencyGraph;

    size_t linSearch(const double &propensitySum);

    size_t twoDimSearch(const double &propensitySum);

};

}
}

#endif // __FLUC_OPTIMIZEDSSA_H
