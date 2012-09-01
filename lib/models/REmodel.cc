#include "REmodel.hh"
#include "ode/ode.hh"
#include "trafo/constantfolder.hh"

using namespace iNA;
using namespace iNA::Models;

ConservationConstantCollector::ConservationConstantCollector(SSEBaseModel &model)

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
ConservationConstantCollector::getInitialState(Eigen::VectorXd &x)
{
  // deterministic initial conditions for state
  //x<<(this->ICsPermuted).head(this->numIndSpecies());

}

/**
* A method that folds conservation constants in an expression.
*/
GiNaC::ex
ConservationConstantCollector::apply(const GiNaC::ex &exIn)
{
    return exIn.subs(substitutions);
}


/**
* A method that folds all constants in a vector or matrix.
*/
Eigen::MatrixXex
ConservationConstantCollector::apply(const Eigen::MatrixXex &vecIn)
{

    Eigen::MatrixXex vecOut(vecIn.rows(),vecIn.cols());
    // ... and fold all constants due to conservation laws
    for (int i=0; i<vecIn.rows(); i++)
    for (int j=0; j<vecIn.cols(); j++)
            vecOut(i,j)=vecIn(i,j).subs(substitutions);

    return vecOut;

}

const Eigen::VectorXd &
ConservationConstantCollector::getConservedCycles()
{
    return this->conserved_cycles;
}

void
REmodel::postConstructor()
{
    // set dimension
    dim = this->numIndSpecies();

    // setup index table
    for(size_t i = 0; i<this->numIndSpecies(); i++)
        this->stateIndex.insert(std::make_pair(this->species[PermutationVec(i)],i));

    // and combine to update vector
    this->updateVector = this->REs;

    this->foldConservationConstants(this->updateVector);
    this->foldConservationConstants(this->JacobianM);

}

REmodel::REmodel(const Ast::Model &model)
  : SSEBaseModel(model)
{
  postConstructor();
}

void
REmodel::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &full_state)
{

  // make space
  full_state.resize(this->numSpecies());
  full_state.head(this->numIndSpecies()) = state.head(this->numIndSpecies());

  if(this->numDepSpecies()>0){
      full_state.tail(this->numDepSpecies()) =
              Eigen::ex2double(this->conserved_cycles)
              + this->Link0CMatrixNumeric*state.head(this->numIndSpecies());
  }

  // restore original order and return
  full_state = (this->PermutationM.transpose())*full_state;

}


void
REmodel::fullState(ConservationConstantCollector &context,const Eigen::VectorXd &state, Eigen::VectorXd &full_state)
{

  // make space
  full_state.resize(this->numSpecies());
  full_state.head(this->numIndSpecies()) = state.head(this->numIndSpecies());

  if(this->numDepSpecies()>0){
      full_state.tail(this->numDepSpecies()) =
              context.getConservedCycles()
              + context.getLink0CMatrix()*state.head(this->numIndSpecies());
  }

  // restore original order and return
  full_state = (this->PermutationM.transpose())*full_state;

}

GiNaC::exmap
REmodel::getFlux(const Eigen::VectorXd &state, Eigen::VectorXd &flux)

{


    // collect all the values of constant parameters except variable parameters
    Trafo::ConstantFolder constants(*this);


    flux.resize(this->numReactions());

    GiNaC::exmap subtab;
    for(size_t s=0; s<this->numIndSpecies(); s++)
        subtab.insert( std::pair<GiNaC::ex,GiNaC::ex>( getREvar(s), state(s) ) );

    this->foldConservationConstants(this->rate_expressions);

    for(size_t i=0; i<numReactions();i++)
        flux(i)=GiNaC::ex_to<GiNaC::numeric>(constants.apply(this->rate_expressions(i)).subs(subtab)).to_double();

    return subtab;

}

size_t
REmodel::getDimension()
{
  // this is enough for deterministic RE models
  return dim;
}


//void
//REmodel::getOmega(Eigen::VectorXd &om)
//{
//  om = this->Omega;
//}

void
REmodel::getInitialState(Eigen::VectorXd &x)
{
  // deterministic initial conditions for state
  x<<(this->ICsPermuted).head(this->numIndSpecies());

}

const Eigen::VectorXex &
REmodel::getUpdateVector() const {
  return this->updateVector;
}

const GiNaC::symbol &
REmodel::getREvar(size_t s) const {
    return this->species[this->PermutationVec(s)];
}

const GiNaC::symbol &
REmodel::getSSEvar(size_t index) const {
    return this->stateVariables[index];
}

const Eigen::MatrixXex &
REmodel::getJacobian() const {
    return this->JacobianM;
}

