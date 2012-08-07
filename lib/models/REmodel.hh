#ifndef __FLUC_REmodel_HH__
#define __FLUC_REmodel_HH__

#include "ast/ast.hh"
#include "ssebasemodel.hh"

namespace Fluc {
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

  Eigen::MatrixXd Link0CMatrixNumeric;
  Eigen::MatrixXd LinkCMatrixNumeric;

  Eigen::VectorXd  Omega;

  Eigen::VectorXd  conserved_cycles;
  Eigen::VectorXd  ICsPermuted;

  std::vector<GiNaC::symbol> stateVariables;
  Eigen::VectorXex updateVector;

  /**
   * Used to evaluate the initial values.
   */
  Ast::EvaluateModel interpreter;


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
  /* REmodel(libsbml::Model *model); */

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
   * Interface for the integrator: get initial state vector.
   */
  virtual void getInitialState(Eigen::VectorXd &x);

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
  void getOmega(Eigen::VectorXd &om);

  /**
   * Get the conservation laws.
   */
  void getConservedCycles(std::vector<GiNaC::ex> &cLaw);

  /**
   * Get the values of the conservation constants.
   */
  void getConservedCycles(Eigen::VectorXd &consc);

  double foldVertex(std::list<int> lower, std::list<int> upper);

  /**
   * Just dumps the RE internals.
   */
  virtual void dump(std::ostream &str);

};


}
}

#endif // REMODEL_HH
