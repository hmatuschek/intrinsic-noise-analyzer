#ifndef CONSERVATIONANALYSIS_HH
#define CONSERVATIONANALYSIS_HH

#include "basemodel.hh"
#include "propensityexpansion.hh"
#include "conservationanalysismixin.hh"

namespace iNA {
namespace Models {

class ConservationAnalysis:
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
    * Holds symbols for dependence on initial conditions.
    */
    Eigen::VectorXex ICs;

    /**
    * Holds symbols of constants arising from conservation laws
    */
    Eigen::Matrix<GiNaC::symbol,Eigen::Dynamic,1> conservationConstants;

    /**
    * Expression for Link zero matrix linking independent and dependent concentrations.
    */
    Eigen::MatrixXex Link0CMatrix;

    /**
    * Expression for Link matrix linking concentrations.
    */
    Eigen::MatrixXex LinkCMatrix;

    GiNaC::exmap dependentSpecies;

private:

    GiNaC::exmap substitutions;

public:

    /**
     * Constructor.
     */
    ConservationAnalysis(const Ast::Model &model);

    /**
    * A method that generates a substitution table for all conservation laws arising from the model
    */
    GiNaC::exmap getConservationConstants(const Eigen::VectorXex &conserved_cycles);

    /**
    * A method that generates a substitution table for all conservation laws arising from the model
    */
    GiNaC::exmap getConservationConstants(const Eigen::VectorXd &conserved_cycles);

    /**
    * A method that generates a substitution table for the initial conditions.
    */
    GiNaC::exmap getICconstants(const Eigen::VectorXd &initialcondition);

    /** Links concentration to full state */
    const Eigen::MatrixXex & getLink0CMatrix();

    /** Links concentrations to full state */
    const Eigen::MatrixXex getLinkCMatrix();

    /** Yields the values of conserved amounts. */
    Eigen::VectorXex getConservedAmounts(const Eigen::VectorXd &InitialAmount);

    /** Yields the conserved amounts as functions of the initial amounts. */
    Eigen::VectorXex getConservedAmounts(const Eigen::VectorXex &InitialAmount);

    /** Yields the values of conserved cycles as as functions of the initial conditions. */
    Eigen::VectorXex getConservedCycles(const Eigen::VectorXd &ICs);

    /** Yields the expressions for conserved cycles as functions of the initial conditions. */
    Eigen::VectorXex getConservedCycles(const Eigen::VectorXex &ICs);

    /** Yields the conservation matrix in concentration space */
    Eigen::MatrixXex getConservationMatrix();

    Eigen::VectorXex conserved_cycles;


    Eigen::VectorXex Omega;

    Eigen::VectorXd ICsPermuted;

    /**
    * A method that folds all constants arising from conservation laws in a given expression
    */
    template<typename T>
    void foldConservationConstants(Eigen::MatrixBase<T> &vec)

    {
        // ... and fold all constants due to conservation laws
        for (int i=0; i<vec.rows(); i++)
        for (int j=0; j<vec.cols(); j++)
                vec(i,j) = vec(i,j).subs(this->substitutions);
    }

    const Eigen::Matrix<GiNaC::symbol,Eigen::Dynamic,1> &getConservationConstants();

};

}} // namespace

#endif // CONSERVATIONANALYSIS_HH
