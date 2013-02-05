#ifndef __INA_INITIALCONDITIONS_HH
#define __INA_INITIALCONDITIONS_HH

#include "ast/ast.hh"
#include "ssebasemodel.hh"

#include "trafo/constantfolder.hh"

namespace iNA {
namespace Models {

/**
* Folds a set of given constants from an parameter set.
*/
class ParameterFolder
{

    /**
    * Holds a reference to the substitutions.
    */
    GiNaC::exmap &_substitutions;

public:
    /**
    * Constructor
    */
    ParameterFolder(GiNaC::exmap &constants)
        : _substitutions(constants)
    {

    }



    /**
    * A method that folds all constants in an expression.
    */
    GiNaC::ex apply(const GiNaC::ex &expression)

    {
        return expression.subs(_substitutions);
    }

    /**
    * A method that folds all constants in a vector or matrix.
    */
    Eigen::MatrixXex apply(const Eigen::MatrixXex &vecIn)

    {

        Eigen::MatrixXex vecOut(vecIn.rows(),vecIn.cols());
        // ... and fold all constants due to conservation laws
        for (int i=0; i<vecIn.rows(); i++)
        for (int j=0; j<vecIn.cols(); j++)
                vecOut(i,j)=vecIn(i,j).subs(_substitutions);

        return vecOut;

    }


    const GiNaC::exmap &
    getSubstitutions()
    {
        return _substitutions;
    }


};

/**
* This module handles the initial conditions of a model and can be used to reconstruct a model.
*/

class InitialConditions

{

public:
    /**
     * Constructor.
     */
    InitialConditions(SSEBaseModel &model, Trafo::excludeType excludes=Trafo::excludeType());

protected:

    GiNaC::exmap substitutions;

    Eigen::MatrixXd Link0CMatrixNumeric;
    Eigen::MatrixXd LinkCMatrixNumeric;

    Eigen::VectorXd Omega;

    Eigen::VectorXd conserved_cycles;
    Eigen::VectorXd ICsPermuted;

    Eigen::VectorXd ICs;

    SSEBaseModel &model;

    Trafo::InitialValueFolder evICs;

public:

    /**
     * Returns the deterministic initial state.
     */
    const Eigen::VectorXd & getInitialState();

    /**
     * Get the values of the conservation constants.
     */
    const Eigen::VectorXd & getConservedCycles();

    /**
    * A method that folds conservation constants in an expression.
    */
    GiNaC::ex apply(const GiNaC::ex &exIn);

    /**
    * A method that folds all conservation constants in a vector or matrix.
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

    const GiNaC::exmap &
    getSubstitutions()
    {
        return substitutions;
    }

};

}}

#endif // INITIALCONDITIONS_HH
