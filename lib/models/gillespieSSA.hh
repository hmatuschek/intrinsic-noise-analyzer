#ifndef __FLUC_GILLESPIESSA_H
#define __FLUC_GILLESPIESSA_H

#include "stochasticsimulator.hh"
#include "constantstoichiometrymixin.hh"
#include "extensivespeciesmixin.hh"
#include "eval/bci/bci.hh"
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

class GillespieSSA :
  public StochasticSimulator,
  public ConstantStoichiometryMixin

{
private:

    /** Byte code for propensity evaluation. */
    Evaluate::bci::Code bytecode;

    /**
     * Interpreter for each thread.
     */
    std::vector< Evaluate::bci::Interpreter<Eigen::VectorXd> > interpreter;

    /**
     * Reserves space for propensities of each threads.
     */
    std::vector< Eigen::VectorXd > prop;

public:

    /**
     * Is initialized with a model, the number of realization @c ensembleSize and a seed for the
     * random number generator
     */
    GillespieSSA(libsbml::Model *model, int ensembleSize, int seed, size_t opt_level=0, size_t num_threads=1);

    /**
    *  the stepper for the SSA
    **/
    void run(double step);

};

}
}

#endif // __FLUC_GILLESPIESSA_H
