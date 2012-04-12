#ifndef __FLUC_GILLESPIESSAGINAC_H
#define __FLUC_GILLESPIESSAGINAC_H

#include "models/stochasticsimulator.hh"
#include "models/constantstoichiometrymixin.hh"
#include "models/extensivespeciesmixin.hh"

namespace Fluc {
namespace Models {

/**
 * Gillespie's Stochastic Simulation Algorithm with Ginac evaluation.
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
class GillespieSSAginac :
  public StochasticSimulator,
  public ConstantStoichiometryMixin
{
protected:
  /**
   * returns the index of a reaction selected randomly according to the probability determined from
   * the vector of propensities @c prop
   **/
  int selectReaction(const Eigen::VectorXd &prop,const double &sum_propensity);

  /**
   * Updates the population vector of realization @c sid when a reaction occurs.
   **/
  void updateState(int &sid, int &reaction);


public:
  /**
   * is initialized with a model, the number of realization @c ensembleSize and a seed for the
   * random number generator
   **/
  GillespieSSAginac(libsbml::Model *model, int ensembleSize, int seed);

  /**
   *  the stepper for the SSA
   **/
  void run(double step);

};

}
}

#endif // __FLUC_GILLESPIESSAGINAC_H
