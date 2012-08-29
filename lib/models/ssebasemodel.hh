#ifndef __FLUC_MODELS_SSEBASEMODEL_HH
#define __FLUC_MODELS_SSEBASEMODEL_HH

#include "basemodel.hh"
#include "propensityexpansion.hh"
#include "conservationanalysismixin.hh"

namespace Fluc {
namespace Models {

class ConservationConstants :
    public BaseModel,
    public propensityExpansion,
    public ConservationAnalysisMixin
{
protected:

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
    * Holds symbols of constants arising from conservation laws
    */
    Eigen::VectorXex conservationConstants;

    /**
    * Expression for Link zero matrix linking independent and dependent concentrations.
    */
    Eigen::MatrixXex Link0CMatrix;

    /**
    * Expression for Link matrix linking concentrations.
    */
    Eigen::MatrixXex LinkCMatrix;

    GiNaC::exmap dependentSpecies;

    /**
    * A method that generates a substituation table for all conservation laws arising from the model
    */
    GiNaC::exmap getConservationConstants(const Eigen::VectorXd &conserved_cycles);


public:

    ConservationConstants(const Ast::Model &model);


    GiNaC::exmap substitutions;

    Eigen::VectorXd ICsPermuted;

    Eigen::VectorXd conserved_cycles;

    Eigen::VectorXd Omega;

    Eigen::MatrixXd Link0CMatrixNumeric;

    Eigen::MatrixXd LinkCMatrixNumeric;

    /**
    * A method that folds all constants arising from conservation laws in a given expression
    */
    template<typename T>
    void foldConservationConstants(const Eigen::VectorXd &conserved_cycles, Eigen::MatrixBase<T> &vec)

    {

        // ... and fold all constants due to conservation laws
        for (int i=0; i<vec.rows(); i++)
        for (int j=0; j<vec.cols(); j++)
                vec(i,j)=vec(i,j).subs(this->substitutions);

    }

};

/**
 * The System Size Expansion base model.
 *
 * Provides all coefficients of the SSE in an uncontrained base.
 *
 * @ingroup sse
 */
class SSEBaseModel :
    public ConservationConstants
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
    * Expressions for Diffusion matrix \f$ \underline{D} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionMatrix;

    /**
    * Expressions for Diffusion matrix \f$ \underline{D}^{(1)} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionMatrixO1;

    /**
    * Expressions for Diffusion matrix in vectorized form \f$ \text{vec}(\underline{D}) \f$ and unconstrained base.
    */
    Eigen::VectorXex DiffusionVec;

    /**
    * Expressions for 3-tensor matrix \f$ D_{i}^{jk} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionJacM;  

    /**
    * Expressions for 3-tensor matrix \f$ D_{i}^{(1)jk} \f$ in unconstrained base.
    */
    Eigen::MatrixXex DiffusionJacMO1;

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
    * Expressions of Hessian of objective function for optimization.
    */
    Eigen::MatrixXex fHessian;


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
     * Constructor from @c Ast::Model.
     */
    explicit SSEBaseModel(const Ast::Model &model);
};

}
}

#endif // __FLUC_MODELS_SSEBASEMODEL_HH
