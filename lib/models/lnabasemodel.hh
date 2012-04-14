#ifndef __FLUC_MODELS_LNABASEMODEL_HH
#define __FLUC_MODELS_LNABASEMODEL_HH

#include <eigen3/Eigen/Eigen>
#include <sbml/SBMLTypes.h>

#include <cln/exception.h>

#include "ast/ast.hh"
#include <ginac/ginac.h>

#include "basemodel.hh"
#include "lnamixin.hh"
#include "ginacsupportforeigen.hh"

namespace Fluc {
namespace Models {


/**
 * The Linear Noise Approximation base model.
 *
 * Provides all coefficients of the Linear Noise Approximation in an uncontrained base.
 *
 * @ingroup models
 */
class LNABaseModel :
    public BaseModel,
    public LNAMixin
{

private:
    Eigen::VectorXex rate_expressions;
    Eigen::VectorXex rate_corrections;
    Eigen::MatrixXex rates_gradient;
    Eigen::MatrixXex rates_hessian;

protected:

    /**
    * Expression for Link zero matrix linking independent and dependent concentrations.
    */
    Eigen::MatrixXex Link0CMatrix;

    /**
    * Expression for Link matrix linking concentrations.
    */
    Eigen::MatrixXex LinkCMatrix;

    /**
    * Expressions of Rate Equations in unconstrained base.
    */
    Eigen::VectorXex REs;

    /**
    * Expressions of Rate Equation corrections in unconstrained base.
    */
    Eigen::VectorXex REcorrections;

    /**
    * Expressions of Jacobian matrix in unconstrained base.
    */
    Eigen::MatrixXex JacobianM;

    /**
    * Expressions of Hessian in unconstrained base.
    */
    Eigen::MatrixXex Hessian;

    /**
    * Expressions for Diffusion matrix \f$ \underline{D} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionMatrix;

    /**
    * Expressions for Diffusion matrix in vectorized form \f$ \text{vec}(\underline{D}) \f$ and unconstrained base.
    */
    Eigen::VectorXex DiffusionVec;

    /**
    * holds symbols of constants arising from conservation laws
    */
    Eigen::VectorXex conservationConstants;

    /**
    * Expressions of Hessian of objective function for optimization.
    */
    Eigen::MatrixXex fHessian;

    /**
    * A method that folds all constants arising from conservation laws
    */
    void foldConservationConstants(const Eigen::VectorXd &conserved_cycles);

    /**
    * A method that folds all constants arising from conservation laws in a given expression
    */
    void foldConservationConstants(const Eigen::VectorXd &conserved_cycles, Eigen::VectorXex &vec);

    /**
    * A method that generates a substituation table for all conservation laws arising from the model
    */
    GiNaC::exmap generateConservationConstantsTable(const Eigen::VectorXd &conserved_cycles);

    /**
    * Generates a vertex of the system size expansion from list of lower and upper indices.
    */
    GiNaC::ex vertex(std::list<size_t> &lower, std::list<size_t> &upper, size_t order=0);


private:
    /**
     * Performs a common construction part, shared between all constructors.
     */
    void postConstructor();


public:
    /**
     * Constructor from SBML model.
     */
    LNABaseModel(libsbml::Model *model);

    /**
     * Constructor from @c Ast::Model.
     */
    explicit LNABaseModel(const Ast::Model &model);
};

}
}

#endif // __FLUC_MODELS_LNABASEMODEL_HH
