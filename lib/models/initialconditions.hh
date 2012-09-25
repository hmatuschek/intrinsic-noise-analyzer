#ifndef __INA_INITIALCONDITIONS_HH
#define __INA_INITIALCONDITIONS_HH

#include "ast/ast.hh"
#include "ssebasemodel.hh"

namespace iNA {
namespace Models {

class InitialConditions

{

    /**
     * Constructor.
     */
    InitialConditions(SSEBaseModel &model);

private:

    GiNaC::exmap substitutions;

    Eigen::MatrixXd Link0CMatrixNumeric;
    Eigen::MatrixXd LinkCMatrixNumeric;

    Eigen::VectorXd Omega;

    Eigen::VectorXd conserved_cycles;
    Eigen::VectorXd ICsPermuted;

    Eigen::VectorXd ICs;

    SSEBaseModel &model;


public:

    /**
     * Interface for the integrator: get initial state vector.
     */
    virtual void getInitialState(Eigen::VectorXd &x);

    /**
     * Get the values of the conservation constants.
     */
    const Eigen::VectorXd & getConservedCycles();

    /**
    * A method that folds conservation constants in an expression.
    */
    GiNaC::ex apply(const GiNaC::ex &exIn);

    /**
    * A method that folds all constants in a vector or matrix.
    */
    Eigen::MatrixXex apply(const Eigen::MatrixXex &vecIn);


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

    const Eigen::MatrixXd &
    getLink0CMatrix()

    {
        return this->Link0CMatrixNumeric;
    }


    const Eigen::MatrixXd &
    getLinkCMatrix()

    {
        return this->LinkCMatrixNumeric;
    }




};

}}

#endif // INITIALCONDITIONS_HH
