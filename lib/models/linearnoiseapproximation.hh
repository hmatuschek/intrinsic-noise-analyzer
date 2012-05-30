#ifndef __FLUC_LINEARNOISEAPPROXIMATION_HH__
#define __FLUC_LINEARNOISEAPPROXIMATION_HH__

#include <sbml/SBMLTypes.h>
#include <eigen3/Eigen/Eigen>

//#include <complex>
#include "kroneckerproduct.hh"
//#include "nlesolve/newtonraphson.hh"

#include <cln/exception.h>
#include <ginac/ginac.h>

#include "ast/ast.hh"
#include "lnabasemodel.hh"

namespace Fluc {
namespace Models {

/**
 * The Linear Noise Approximation model \cite elf2003 and \cite grima2010.
 *
 * @ingroup sse
 */

class LinearNoiseApproximation :
    public LNABaseModel
{
protected:

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

  void getfHessianM(Eigen::MatrixXd &fHessianM);

  void getfHessianM(const Eigen::VectorXd &state, Eigen::MatrixXd &fHessianM);

  const Eigen::VectorXex &getUpdateVector() const;

protected:
  /**
  * Sets the state of the interpreter and gives correction terms for macroscopic REs.
  */
  void getRateCorrections(const Eigen::VectorXd &state, Eigen::MatrixXd &REcorr);

  /**
  * Gives correction terms to macroscopic REs at current state.
  */
  void getRateCorrections(Eigen::VectorXd &REcorr);

  /**
  * Sets the state of the interpreter and gives Hessian.
  */
  void getHessian(const Eigen::VectorXd &state, Eigen::MatrixXd &Hessian);

  /**
  * Gives Hessian at current state.
  */
  void getHessian(Eigen::MatrixXd &Hessian);

  /**
  * Sets the state of the interpreter and gives Diffusion matrix in vectorized form.
  */
  void getDiffusionVec(const Eigen::VectorXd &state, Eigen::VectorXd &DiffusionVec);

  /**
  * Gives Diffusion matrix in vectorized form.
  */
  void getDiffusionVec(Eigen::VectorXd &DiffusionVec);

  /**
  * Sets the state of the interpreter and gives Diffusion matrix.
  */
  void getDiffusionMatrix(const Eigen::VectorXd &state, Eigen::MatrixXd &DiffusionM);

  /**
  * Gives Diffusion matrix.
  */
  void getDiffusionMatrix(Eigen::MatrixXd &DiffusionM);

  /**
  * Construct a dummy covariance matrix
  */
  void constructCovarianceMatrix(Eigen::MatrixXex &cov);

  void
  constructSymmetricMatrix(const Eigen::VectorXex &covVec,Eigen::MatrixXex &cov);

  void
  flattenSymmetricMatrix(const Eigen::MatrixXex &mat,Eigen::VectorXex &vec);

private:
  /**
   * Performs the common construction part, shared between all constructors.
   */
  void postConstructor();


public:
  /**
   * Constructor...
   */
  LinearNoiseApproximation(libsbml::Model *model);

  /**
   * Constructor.
   */
  explicit LinearNoiseApproximation(const Ast::Model &model);

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
  void getInitialState(Eigen::VectorXd &x);

  /**
   * Reconstruct concentration vector from state vector.
   *
   * @param state The reduced state.
   * @param full_state Vector of concentrations.
   */
  void fullState(const Eigen::VectorXd &state, Eigen::VectorXd &fullState);

  /**
   * Reconstruct concentration vector and covariance matrix from state vector.
   *
   * @param state The current (reduced) state.
   * @param concentrations Concentrations vector.
   * @param cov The covariance matrix.
   */
  void fullState(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations, Eigen::MatrixXd &cov);

  /**
   * Reconstruct concentration vector, covariance matrix and EMRE correction vector from state vector.
   *
   * input:
   * @param state
   * output:
   * @param concentrations vector, @param covariance matrix and @param emre correction vector.
   */
  void fullState(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations, Eigen::MatrixXd &covariance, Eigen::VectorXd &emre);

  /**
   * Reconstruct concentration vector, covariance matrix and EMRE & IOS correction vector from state vector.
   *
   * input:
   * @param state
   * output:
   * @param concentrations vector, @param covariance matrix and @param emre correction vector.
   * @param IOS correction to covariance matrix, @param vector of skewness of distribution
   */
  void fullState(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations,
                 Eigen::MatrixXd &cov, Eigen::VectorXd &emre,  Eigen::MatrixXd &iosCov, Eigen::VectorXd &skewness, Eigen::VectorXd &iosemre);


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
  friend class SpectralAnalysis;
  friend class SpectralAnalysisBase;

};


}
}

#endif // LINEARNOISEAPPROXIMATION_HH
