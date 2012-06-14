#ifndef __FLUC_MODELS_SSEBASEMODEL_HH
#define __FLUC_MODELS_SSEBASEMODEL_HH

#include "basemodel.hh"
#include "propensityexpansion.hh"
#include "conservationanalysismixin.hh"

namespace Fluc {
namespace Models {


/**
 * The System Size Expansion base model.
 *
 * Provides all coefficients of the SSE in an uncontrained base.
 *
 * @ingroup sse
 */
class SSEBaseModel :
    public BaseModel,    
    public propensityExpansion,
    public ConservationAnalysisMixin
{

private:
    Eigen::VectorXex rate_expressions;
    Eigen::VectorXex rate_corrections;
    Eigen::MatrixXex rates_gradient;
    Eigen::MatrixXex rates_hessian;


    /**
    *  \f$ \Omega \f$-vector for independent species
    */
    Eigen::VectorXex Omega_ind;

    /**
    *  \f$ \Omega \f$-vector for dependent species
    */
    Eigen::VectorXex Omega_dep;


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
    * Expressions for 3-tensor matrix \f$ D_{i}^{jk} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionJacM;  

    /**
    * Expressions for 3-tensor matrix \f$ D_{ijk} \f$ in unconstrained base.
    */
    Eigen::VectorXex Diffusion3Tensor;

    /**
    * Expressions for 4-tensor matrix \f$ D_{ij}^{kl} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionHessianM;

    /**
    * Expressions for 4-tensor matrix \f$ D_{i}^{jkl} \f$ in unconstrained base.
    */
    Eigen::MatrixXex PhilippianM;

    /**
    * holds symbols of constants arising from conservation laws
    */
    Eigen::VectorXex conservationConstants;

    /**
    * Expressions of Hessian of objective function for optimization.
    */
    Eigen::MatrixXex fHessian;

    /**
    * THIS FUNCTION CALL IS REDUNDANT NOW!
    * A method that folds all constants arising from conservation laws
    */
    void foldConservationConstants(const Eigen::VectorXd &conserved_cycles);

    /**
    * THIS FUNCTION CALL IS REDUNDANT NOW!
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
    SSEBaseModel(libsbml::Model *model);

    /**
     * Constructor from @c Ast::Model.
     */
    explicit SSEBaseModel(const Ast::Model &model);
};

}
}

#endif // __FLUC_MODELS_LNABASEMODEL_HH