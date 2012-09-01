#ifndef __FLUC_MODELS_CONSTANTSTOICHIOMETRYMIXIN_HH__
#define __FLUC_MODELS_CONSTANTSTOICHIOMETRYMIXIN_HH__

#include "basemodel.hh"
#include "eigen3/Eigen/Eigen"
#include "ginacsupportforeigen.hh"


namespace iNA {
namespace Models {


/**
 * Asserts that the stoichiometry of the BaseModel (given to the constructor) is constant
 * and assembles the stoichiometry matrices for the model.
 *
 * @ingroup models
 */
class ConstantStoichiometryMixin
{
protected:
  /**
   * Holds the stoichiometry matrix of the reactants.
   */
  Eigen::MatrixXd  reactants_stoichiometry;

  /**
   * Holds the stoichiometry matrix of the products.
   */
  Eigen::MatrixXd  products_stoichiometry;

  /**
   * Holds the modifier stoichiometry.
   */
  Eigen::MatrixXd modifier_stoichiometry;

  /**
   * Holds the stoichiometry matrix as the difference of the product and rectant stoichiometry.
   */
  Eigen::MatrixXd  stoichiometry;


public:
  /**
   * Performs the checks on the given base model.
   */
  ConstantStoichiometryMixin(BaseModel &base);

  /**
   * Stores the stoichiometry matrix in the given @c Eigen::Matrix.
   */
  void getStoichiometry(Eigen::MatrixXd &stoichiometry);

  /**
   * Stores the reactants stoichiometry matrix in the given @c Eigen::Matrix.
   */
  void getReactantsStoichiometry(Eigen::MatrixXd &stoichiometry);

  /**
   * Stores the products stoichiometry matrix in the given @c Eigen::Matrix.
   */
  void getProductsStoichiometry(Eigen::MatrixXd &stoichiometry);

  /**
   * Stores the modifier stoichiometry matrix in the geivne @c Eigen::Matrix.
   */
  void getModifierStoichiometry(Eigen::MatrixXd &stoichiometry);
};


}
}

#endif // __FLUC_MODELS_CONSTANTSTOICHIOMETRYMIXIN_HH__
