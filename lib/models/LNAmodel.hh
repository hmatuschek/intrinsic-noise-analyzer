#ifndef __FLUC_LNAMODEL_HH__
#define __FLUC_LNAMODEL_HH__

#include "REmodel.hh"

namespace Fluc {
namespace Models {

/**
 * The Linear Noise Approximation model.
 *
 * The LNA has been described in \cite vanKampen and \cite elf2003. The EMRE has been derived in \cite grima2010.
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

  void constructSymmetricMatrix(const Eigen::VectorXex &covVec,Eigen::MatrixXex &cov);

  void flattenSymmetricMatrix(const Eigen::MatrixXex &mat,Eigen::VectorXex &vec);

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

  friend class SpectralAnalysis;
  friend class SpectralAnalysisBase;

};


}
}

#endif // LINEARNOISEAPPROXIMATION_HH
