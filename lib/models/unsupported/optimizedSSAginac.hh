#ifndef __FLUC_MODELS_OPTIMIZEDSSAginac_H
#define __FLUC_MODELS_OPTIMIZEDSSAginac_H

#include "models/stochasticsimulator.hh"
#include "models/constantstoichiometrymixin.hh"
#include "models/extensivespeciesmixin.hh"

#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <eigen3/Eigen/Sparse>

namespace Fluc {
namespace Models {

/**
 * Gillespie's Stochastic Simulation Algorithm.
 *
 * This is a implementation of the original algorithm as described in
 *
 * Gillespie, D.T., A general method for numerically simulating the stochastic time evolution
 *  of coupled chemical reactions, J. Comp. Phys 22 (1976), 403--434
 *
 * and
 *
 * Gillespie, D.T., Exact stochastic simulation of coupled chemical reactions, J. Phys. Chem. 81 (1977) 2340–2361
 * doi: 10.1021/j100540a008
 *
 * @ingroup unsupported
 */

class OptimizedSSAginac :
  public StochasticSimulator,
  public ConstantStoichiometryMixin

{
protected:

    /**
    * An instance of the byte code interpreter.
    */
    //Intprt::Interpreter<Eigen::VectorXd> interpreter;

    /**
    * Sparse stoichiometric matrix.
    **/
    Eigen::SparseMatrix<double> sparseStoichiometry;

    /**
    * The dependency graph \f$ G_{ij} \f$ is nonzero if reaction \f$ i \f$ is affects the rates of reaction \f$ j \f$.
    **/
    Eigen::SparseMatrix<int> dependencyGraph;

    /**
    * Priority list. A list of the most frequently updated reactions in descending order.
    * Note that all realizations work on the same list.
    **/
    Eigen::VectorXi priorityList;

    /**
    * returns the index of a reaction selected randomly according to the probability determined from the vector of propensities @c prop
    **/
    int select_reaction(const Eigen::VectorXd &prop,double &sum_propensity);

    /**
    * updates the population vector of realization @c sid when a @c reaction occurs
    **/
    void update_state(int &sid, int &reaction);


public:

    /**
    * is initialized with a model, the number of realization @c ensembleSize and a seed for the random number generator
    **/
    OptimizedSSAginac(libsbml::Model *model, int ensembleSize, int seed);

    /**
    *  the stepper for the Gillespie algorithm
    **/
    void run(double step);

};

}
}

#endif // __FLUC_OPTIMIZEDSSA_H
