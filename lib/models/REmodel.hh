#ifndef __FLUC_REmodel_HH__
#define __FLUC_REmodel_HH__

#include "ast/ast.hh"
#include "ssebasemodel.hh"

namespace iNA {
namespace Models {

class ConservationConstantCollector

{

    /**
     * Constructor.
     */
    ConservationConstantCollector(SSEBaseModel &model);


    GiNaC::exmap substitutions;

    Eigen::MatrixXd Link0CMatrixNumeric;
    Eigen::MatrixXd LinkCMatrixNumeric;

    Eigen::VectorXd  Omega;

    Eigen::VectorXd  conserved_cycles;
    Eigen::VectorXd  ICsPermuted;


    /**
     * Interface for the integrator: get initial state vector.
     */
    virtual void getInitialState(Eigen::VectorXd &x);

    /**
     * Get the values of the conservation constants.
     */
    void getConservedCycles(Eigen::VectorXd &consc);

    /**
    * A method that folds all constants arising from conservation laws in a given expression
    */
    template<typename T>
    void foldConservationConstants(const Eigen::VectorXd &conserved_cycles, Eigen::MatrixBase<T> &vec)

    {

        // ... and fold all constants due to conservation laws
        for (int i=0; i<vec.rows(); i++)
        for (int j=0; j<vec.cols(); j++)
                vec(i,j)=vec(i,j).subs(this->substitutions);

    }

};

/**
 * The RE model.
 *
 * Simply computes the deterministic rate equations (REs).
 *
 * @ingroup sse
 */

class REmodel :
    public SSEBaseModel
{
protected:

  size_t dim;

  std::vector<GiNaC::symbol> stateVariables;

 // Eigen::MatrixXd Link0CMatrixNumeric;
 // Eigen::MatrixXd LinkCMatrixNumeric;

  Eigen::VectorXex updateVector;

public:

  const Eigen::VectorXex &getUpdateVector() const;

  const Eigen::MatrixXex &getJacobian() const;

  const GiNaC::symbol &getREvar(size_t s) const;

  const GiNaC::symbol &getSSEvar(size_t index) const;

private:
  /**
   * Performs the common construction part, shared between all constructors.
   */
  void postConstructor();

public:
  /**
   * Constructor.
   */
  explicit REmodel(const Ast::Model &model);

  /**
   * Maps a symbol (species) to an index in the state vector.
   */
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> stateIndex;

  /**
   * Interface for the integrator: Size of the state vector.
   */
  size_t getDimension();

  /**
   * Reconstruct concentration vector from state vector.
   *
   * @param state The reduced state.
   * @param full_state Vector of concentrations.
   */
  void fullState(const Eigen::VectorXd &state, Eigen::VectorXd &fullState);

  GiNaC::exmap getFlux(const Eigen::VectorXd &state,  Eigen::VectorXd &flux);

  /**
   * Evaluate the full Omega (volumes) vector.
   */
//  void getOmega(Eigen::VectorXd &om);

  double foldVertex(std::list<int> lower, std::list<int> upper);

  /**
   * Interface for the integrator: get initial state vector.
   */
  virtual void getInitialState(Eigen::VectorXd &x);

};


}
}

#endif // REMODEL_HH
