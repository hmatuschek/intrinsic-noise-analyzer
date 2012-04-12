#ifndef __FLUC_GILLESPIECLE_H
#define __FLUC_GILLESPIECLE_H

#include "models/stochasticsimulator.hh"
#include "models/conservationanalysismixin.hh"
#include "models/extensivespeciesmixin.hh"

namespace Fluc {
namespace Models {

/**
 * Stochastic Simulator for the Chemical Langevin Equation \cite gillespie2000.
 *
 * @ingroup unsupported
 */

class GillespieCLE :
  public StochasticSimulator,
  public ConservationAnalysisMixin

{
protected:

    /**
    * holds the values of the conservation constants
    */
    Eigen::VectorXd  conserved_cycles;

    /**
    * initial conditions with independent species first
    */
    Eigen::VectorXd  ICsPermutated;

    /**
    * reconstruct from unconstrained base
    */
    Eigen::VectorXd full_state(const Eigen::VectorXd &observationMatrix);

    /**
    * uniform random number generator on interval (0,1]
    */
    double uniform();

    /**
    * normal distributed N(0,1) random numbers Box-Mueller scheme
    */
    double normal();

    /**
    * vector of length @c size containing independent random numbers with normal distribution N(0,1)
    */
    Eigen::VectorXd normal(int size);

    /**
    * internal state of the integrator
    */
    Eigen::MatrixXd reducedState;

    /**
    * updates the internal state for realization @c sid
    */
    void update_state(int &sid);

public:

    /**
    * is initialized with a model, the number of realization @c ensembleSize and a seed for the random number generator
    **/
    GillespieCLE(libsbml::Model *model, int ensembleSize, int seed);

    /**
    *  CLE stepper
    **/
    void run(double step);

};

}
}

#endif // __FLUC_GILLESPIECLE_H
