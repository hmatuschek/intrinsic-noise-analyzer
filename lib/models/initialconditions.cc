#include "initialconditions.hh"
#include "trafo/constantfolder.hh"

using namespace iNA;
using namespace iNA::Models;

InitialConditions::InitialConditions(SSEBaseModel &model)

{

    // Evaluate initial concentrations and evaluate volumes:
    Ast::EvaluateInitialValue evICs(model);
    Eigen::VectorXd ICs(model.numSpecies());
    for(size_t i=0; i<model.numSpecies();i++)
           ICs(i)=evICs.evaluate(model.getSpecies(i)->getSymbol());

    //store in permutated base
    //this->ICsPermuted = this->PermutationM*ICs;

    Trafo::ConstantFolder constants(model);

    //evaluate the link matrices
    Link0CMatrixNumeric = Eigen::ex2double(model.getLink0CMatrix());
    LinkCMatrixNumeric = Eigen::ex2double(model.getLinkCMatrix());

    if(model.numDepSpecies()>0) this->conserved_cycles = Eigen::ex2double(constants.apply(model.getConservationMatrix()))*ICs;

    // generate substitution table
    substitutions = model.getConservationConstants(conserved_cycles);

}


void
InitialConditions::getInitialState(Eigen::VectorXd &x)
{
  // deterministic initial conditions for state
  //x<<(this->ICsPermuted).head(this->numIndSpecies());

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

const Eigen::VectorXd &
InitialConditions::getConservedCycles()
{
    return this->conserved_cycles;
}

