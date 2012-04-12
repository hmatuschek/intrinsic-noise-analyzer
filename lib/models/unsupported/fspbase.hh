#ifndef __FLUC_MODELS_FSPBASE_H
#define __FLUC_MODELS_FSPBASE_H

#include "models/conservationanalysismixin.hh"
#include "models/particlenumbersmixin.hh"
#include "intprt/intprt.hh"


#define EIGEN_YES_I_KNOW_SPARSE_MODULE_IS_NOT_STABLE_YET
#include <eigen3/Eigen/Sparse>

namespace Fluc {
namespace Models {

/**
 * Prepare a model for FSP calculations.
 *
 * @ingroup unsupported
 */

class FSPBase :
  public BaseModel,
  public ConservationAnalysisMixin,
  public ParticleNumbersMixin

{
protected:

    /**
    * index map for bytecode interpreter
    **/
    std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> stateIndex;


    /**
    * Interpreter is always needed.
    */
    Intprt::Interpreter< Eigen::VectorXi ,Eigen::VectorXd > interpreter;

    /**
    * Collects all bytecode to evaluate all propensities.
    **/
    Intprt::ByteCode all_bytecode;

    /**
    * Sparse stoichiometric matrix.
    **/
    Eigen::SparseMatrix<double> sparseStoichiometry;

    /**
    * The dependency graph \f$ G_{ij} \f$ is nonzero if reaction \f$ i \f$ is affects the rates of reaction \f$ j \f$.
    **/
    Eigen::SparseMatrix<int> dependencyGraph;

    Eigen::VectorXex conservationConstants;

    Eigen::VectorXex rates;

    Eigen::VectorXd ICsPermuted;
    Eigen::VectorXd conserved_cycles;

public:

    /**
     * Is constructed from a SBML model.
     *
     **/
    FSPBase(libsbml::Model *model);

    /**
     * Generate the transition matrix.
     *
     **/
    Eigen::DynamicSparseMatrix<double> generateTransitionM();


    /**
     * Get the index for a state.
     *
     **/
    size_t index(Eigen::VectorXd indices);


    /**
     * returns the state from an index.
     *
     **/
    Eigen::VectorXd getState(int idx);

};

}
}

#endif
