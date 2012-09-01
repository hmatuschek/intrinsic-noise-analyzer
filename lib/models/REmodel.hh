#ifndef __FLUC_REmodel_HH__
#define __FLUC_REmodel_HH__

#include "ast/ast.hh"
#include "ssebasemodel.hh"
#include "initialconditions.hh"

namespace iNA {
namespace Models {

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

  void fullState(InitialConditions &context,const Eigen::VectorXd &state, Eigen::VectorXd &full_state);

  GiNaC::exmap getFlux(const Eigen::VectorXd &state,  Eigen::VectorXd &flux);

  double foldVertex(std::list<int> lower, std::list<int> upper);

  /**
   * Interface for the integrator: get initial state vector.
   */
  virtual void getInitialState(Eigen::VectorXd &x);

};


}
}

#endif // REMODEL_HH
