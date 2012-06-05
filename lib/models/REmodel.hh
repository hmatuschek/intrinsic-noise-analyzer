#ifndef __FLUC_REmodel_HH__
#define __FLUC_REmodel_HH__

#include "ast/ast.hh"
#include "lnabasemodel.hh"

namespace Fluc {
namespace Models {

/**
 * The RE model.
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
  /**
  * Sets the state of the interpreter.
  */
  void setState(const Eigen::VectorXd &state);

  /**
  * Sets the state of the interpreter and gives rate equations.
  */
  void getREs(const Eigen::VectorXd &state, Eigen::VectorXd &REs);

  /**
  * Gives rate equations evaluated at current state.
  */
  void getREs(Eigen::VectorXd &REs);

  /**
  * Sets the state of the interpreter and gives Jacobian matrix.
  */
  void getJacobianMatrix(const Eigen::VectorXd &state, Eigen::MatrixXd &JacobianMatrix);

  /**
  * Gives Jacobian matrix at current state.
  */
  void getJacobianMatrix(Eigen::MatrixXd &JacobianMatrix);

  const Eigen::VectorXex &getUpdateVector() const;

private:
  /**
   * Performs the common construction part, shared between all constructors.
   */
  void postConstructor();

public:
  /**
   * Constructor...
   */
  REmodel(libsbml::Model *model);

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
   * Just dumps the LNA internals.
   */
  virtual void dump(std::ostream &str);

  friend class LNAinterpreter;
  friend class LNAevaluator;
  friend class SteadyStateAnalysis;

  friend class Steady;
  friend class SpectralAnalysis;
  friend class SpectralAnalysisBase;

};


}
}

#endif // LINEARNOISEAPPROXIMATION_HH
