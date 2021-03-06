#ifndef __INA_IOSMODEL_HH__
#define __INA_IOSMODEL_HH__

#include "LNAmodel.hh"

namespace iNA {
namespace Models {

/**
 * The IOS model.
 *
 * The model uses the System Size Expansion to derive corrections to mean and variances beyond the
 * Linear Noise Approximation \cite grima2011.
 *
 * @ingroup sse
 */

class IOSmodel :
    public LNAmodel
{
protected:
    size_t dim3M;

private:
  /**
   * Performs the common construction part, shared between all constructors.
   */
  void postConstructor();


public:
  /**
   * Constructor.
   */
  explicit IOSmodel(const Ast::Model &model);

  /**
   * Get initial state vector for specific initial conditions.
   */
  virtual void getInitial(InitialConditions &ICs, Eigen::VectorXd &x);

  void fluxAnalysis(const Eigen::VectorXd &state, Eigen::VectorXd &flux, Eigen::VectorXd &fluxEMRE,
                    Eigen::MatrixXd &fluxCovariance, Eigen::MatrixXd &fluxIOS);

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
  * Construct non-central moments.
  **/
  void getCentralMoments(const Eigen::VectorXd &state, Eigen::VectorXd &first,
                       Eigen::MatrixXd &second, Eigen::VectorXd &third, Eigen::VectorXd &fourth);


  void fullState(InitialConditions &context, const Eigen::VectorXd &state, Eigen::VectorXd &concentrations,
                 Eigen::MatrixXd &cov, Eigen::VectorXd &emre,
                 Eigen::MatrixXd &iosCov, Eigen::VectorXd &skewness, Eigen::VectorXd &iosemre);


};


}
}

#endif
