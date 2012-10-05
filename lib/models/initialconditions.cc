#include "initialconditions.hh"
#include "trafo/constantfolder.hh"

using namespace iNA;
using namespace iNA::Models;

/**
* Constructs the initial conditions of a model.
*/
InitialConditions::InitialConditions(SSEBaseModel &model)
    : model(model)
{

    // Evaluate initial concentrations and evaluate volumes:
    Trafo::InitialValueFolder evICs(model);
    Eigen::VectorXd ICs(model.numSpecies());
    for(size_t i=0; i<model.numSpecies();i++)
           ICs(i)=evICs.evaluate(model.getSpecies(i)->getSymbol());

    // Store in permutated base
    this->ICsPermuted = (model.getPermutationMatrix()*ICs).head(model.numIndSpecies());

    Trafo::ConstantFolder constants(model);

    // Evaluate the link matrices
    Link0CMatrixNumeric = Eigen::ex2double(constants.apply(model.getLink0CMatrix()));
    LinkCMatrixNumeric = Eigen::ex2double(constants.apply(model.getLinkCMatrix()));

    if(model.numDepSpecies()>0) this->conserved_cycles = Eigen::ex2double(constants.apply(model.getConservationMatrix()))*ICs;

    // generate substitution table
    substitutions = model.getConservationConstants(conserved_cycles);

}


/**
* Returns the initial state vector of the model.
*/
void
InitialConditions::getInitialState(Eigen::VectorXd &x)
{
  // deterministic initial conditions for state
    x.head(ICsPermuted.size())=this->ICsPermuted;
}


/**
* A method that folds conservation constants in an expression.
*/
GiNaC::ex
InitialConditions::apply(const GiNaC::ex &exIn)
{
    return exIn.subs(substitutions);
}


/**
* A method that folds all constants in a vector or matrix.
*/
Eigen::MatrixXex
InitialConditions::apply(const Eigen::MatrixXex &vecIn)
{

    Eigen::MatrixXex vecOut(vecIn.rows(),vecIn.cols());
    // ... and fold all constants due to conservation laws
    for (int i=0; i<vecIn.rows(); i++)
    for (int j=0; j<vecIn.cols(); j++)
            vecOut(i,j)=vecIn(i,j).subs(substitutions);

    return vecOut;

}

/**
* Returns the conserved moieties of the model.
*/
const Eigen::VectorXd &
InitialConditions::getConservedCycles()
{
    return this->conserved_cycles;
}
