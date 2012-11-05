#ifndef __INA_MODELS_SSEBASEMODEL_HH
#define __INA_MODELS_SSEBASEMODEL_HH

#include "basemodel.hh"
#include "propensityexpansion.hh"
#include "conservationanalysis.hh"

namespace iNA {
namespace Models {


typedef std::map<std::string,double> ParameterSet;

/**
 * The System Size Expansion base model.
 *
 * Provides all coefficients of the SSE in an uncontrained base.
 *
 * @ingroup sse
 */
class SSEBaseModel :
    public ConservationAnalysis
{

protected:

    Eigen::VectorXex rate_expressions;
    Eigen::VectorXex rate_corrections;
    Eigen::MatrixXex rates_gradient;
    Eigen::MatrixXex rates_gradientO1;
    Eigen::MatrixXex rates_hessian;
    Eigen::MatrixXex rates_3rd;

protected:

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
    * Expressions of \f$ J_{i}^{j(1)} \f$ in unconstrained base.
    */
    Eigen::MatrixXex JacobianMO1;

    /**
    * Expressions of Hessian in unconstrained base.
    */
    Eigen::MatrixXex Hessian;

    /**
    * Expressions custom Diffusion matrix \f$ \underline{D} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionMatrix;

    /**
    * Expressions custom Diffusion matrix \f$ \underline{D}^{(1)} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionMatrixO1;

    /**
    * Expressions custom Diffusion matrix in vectorized customm \f$ \text{vec}(\underline{D}) \f$ and unconstrained base.
    */
    Eigen::VectorXex DiffusionVec;

    /**
    * Expressions custom 3-tensor matrix \f$ D_{i}^{jk} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionJacM;  

    /**
    * Expressions custom 3-tensor matrix \f$ D_{i}^{(1)jk} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionJacMO1;

    /**
    * Expressions custom 3-tensor matrix \f$ D_{ijk} \f$ in unconstrained base.
    */
    Eigen::VectorXex Diffusion3Tensor;

    /**
    * Expressions custom 4-tensor matrix \f$ D_{ij}^{kl} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionHessianM;

    /**
    * Expressions custom 4-tensor matrix \f$ D_{i}^{jkl} \f$ in unconstrained base.
    */
    Eigen::MatrixXex PhilippianM;


    /**
    * Expressions of Hessian of objective function custom optimization.
    */
    Eigen::MatrixXex fHessian;


    /**
    * Generates a vertex of the system size expansion from list of lower and upper indices.
    */
    GiNaC::ex vertex(std::list<size_t> &lower, std::list<size_t> &upper, size_t order=0);


private:
    /**
     * Percustomms a common construction part, shared between all constructors.
     */
    void postConstructor();

public:

    /**
     * Constructor from @c Ast::Model.
     */
    explicit SSEBaseModel(const Ast::Model &model);

    /**
     * Translate a parameter list to substitution map (used by ParameterScan method).
     */
    GiNaC::exmap makeExclusionTable(const ParameterSet &parameters);

};

}
}

#endif // __FLUC_MODELS_SSEBASEMODEL_HH
