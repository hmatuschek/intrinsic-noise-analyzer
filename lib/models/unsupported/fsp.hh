#ifndef __FLUC_MODELS_FSP_H
#define __FLUC_MODELS_FSP_H

#include "fspbase.hh"
#include "intprt/intprt.hh"
#include "openmp.hh"

namespace Fluc {
namespace Models {

/**
 * Could perform some FSP calculations.
 *
 * @ingroup unsupported
 */

class FSP :
  public FSPBase

{

protected:

    /**
    * Projection domain.
    **/
    Eigen::VectorXi domain;

    /**
    * Infinitesimal generator of the CME.
    **/
    Eigen::MatrixXd cmeM;

    size_t dim;

    /**
    * The sink state probability.
    **/
    double epsilon;

public:

    /**
     * Is constructed from a SBML model.
     *
     **/
    FSP(libsbml::Model *model);

    /**
     * Generate the transition matrix.
     *
     **/
    Eigen::MatrixXd generateTransitionM(double &epsilon);

    /**
     * Check if state is within the FSP range.
     *
     **/
    bool InRange(Eigen::VectorXi indices);

    /**
     * Get the index for a state.
     *
     **/
    int getIndex(Eigen::VectorXi indices);

    /**
     * Returns the state from an index.
     *
     **/
    Eigen::VectorXi getState(int idx);

    /**
     * returns the dimension of the problem.
     *
     **/
    size_t getDimension();


};

}
}

#endif // __FLUC_FSP_HH
