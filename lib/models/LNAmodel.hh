#ifndef __FLUC_LNAMODEL_HH__
#define __FLUC_LNAMODEL_HH__

#include "REmodel.hh"

namespace Fluc {
namespace Models {

/**
 * The Linear Noise Approximation model \cite elf2003 and \cite grima2010.
 *
 * @ingroup sse
 */

class LNAmodel
   : public REmodel
{

protected:

  /**
  * Construct a dummy covariance matrix
  */
  void constructCovarianceMatrix(Eigen::MatrixXex &cov);

  void
  constructSymmetricMatrix(const Eigen::VectorXex &covVec,Eigen::MatrixXex &cov);

  void
  flattenSymmetricMatrix(const Eigen::MatrixXex &mat,Eigen::VectorXex &vec);

  size_t dimCOV;

private:
  /**
   * Performs the common construction part, shared between all constructors.
   */
  void postConstructor();


public:
  /**
   * Constructor...
   */
  LNAmodel(libsbml::Model *model);

  /**
   * Constructor.
   */
  explicit LNAmodel(const Ast::Model &model);

  /**
   * Interface for the integrator: get initial state vector.
   */
  virtual void getInitialState(Eigen::VectorXd &x);

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

  friend class SpectralAnalysis;
  friend class SpectralAnalysisBase;

};


}
}

#endif // LINEARNOISEAPPROXIMATION_HH
