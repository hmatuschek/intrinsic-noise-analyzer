#ifndef __FLUC_IOSMODEL_HH__
#define __FLUC_IOSMODEL_HH__

#include "LNAmodel.hh"

namespace Fluc {
namespace Models {

/**
 * The IOS model \cite grima2011.
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
   * Constructor...
   */
  IOSmodel(libsbml::Model *model);

  /**
   * Constructor.
   */
  explicit IOSmodel(const Ast::Model &model);

  /**
   * Interface for the integrator: get initial state vector.
   */
  virtual void getInitialState(Eigen::VectorXd &x);

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

  friend class LNAinterpreter;

};


}
}

#endif