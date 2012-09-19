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

private:

    GiNaC::exmap substitutions;

public:


    /**
    * A method that generates a substituation table for all conservation laws arising from the model
    */
    GiNaC::exmap getConservationConstants(const Eigen::VectorXex &conserved_cycles);

    /**
    * A method that generates a substituation table for all conservation laws arising from the model
    */
    GiNaC::exmap getConservationConstants(const Eigen::VectorXd &conserved_cycles);

    ConservationAnalysis(const Ast::Model &model);

    /* Links concentration to full state */
    const Eigen::MatrixXex & getLink0CMatrix();

    /* Links concentrations to full state */
    const Eigen::MatrixXex & getLinkCMatrix();

    Eigen::MatrixXex getConservedCycles(const Eigen::VectorXd &ICs);

    /* Yields the conservation matrix in concentration space */
    Eigen::MatrixXex getConservationMatrix();

    Eigen::VectorXex conserved_cycles;

    Eigen::VectorXex Omega;

    Eigen::VectorXd ICsPermuted;

    Eigen::MatrixXd Link0CMatrixNumeric;

    Eigen::MatrixXd LinkCMatrixNumeric;

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

};

}} // namespace

#endif // CONSERVATIONANALYSIS_HH