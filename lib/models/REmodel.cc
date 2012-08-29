#include "REmodel.hh"
#include "ode/ode.hh"
#include "trafo/constantfolder.hh"

using namespace Fluc;
using namespace Fluc::Models;

ConservationConstantCollector::ConservationConstantCollector(SSEBaseModel &model)

{

    // Evaluate initial concentrations and evaluate volumes:
    Ast::EvaluateInitialValue evICs(model);
    Eigen::VectorXd ICs(model.numSpecies());
    for(size_t i=0; i<model.numSpecies();i++){
       ICs(i)=evICs.evaluate(model.getSpecies(i)->getSymbol());
       this->Omega(i)=evICs.evaluate(model.getSpecies(i)->getCompartment()->getSymbol());
    }

    //store in permutated base
    //this->Omega = model.PermutationM*this->Omega;
    //this->ICsPermuted = model.PermutationM*ICs;

    for(size_t i=0;i<model.numSpecies();i++)
        for(size_t j=0;j<model.numIndSpecies();j++)
            this->LinkCMatrixNumeric(i,j) = GiNaC::ex_to<GiNaC::numeric>( model.getLinkCMatrix()(i,j) ).to_double();

    for(size_t i=0;i<model.numDepSpecies();i++)
        for(size_t j=0;j<model.numIndSpecies();j++)
            this->Link0CMatrixNumeric(i,j) = GiNaC::ex_to<GiNaC::numeric>( model.getLink0CMatrix()(i,j) ).to_double();

//    /**
//    * @todo the omega business is ugly and workaround at the moment
//    */

//    if(model.numDepSpecies()>0){
//        Eigen::VectorXd om_dep=this->Omega.tail(model.numDepSpecies());
//        // compute conserved cycles in permutated base and store in conserved_cycles
//        this->conserved_cycles = om_dep.asDiagonal().inverse()*model.conservation_matrix*(this->Omega.asDiagonal())*this->ICsPermuted;
//    }

    // generate substitution table
    //substitutions = model.getConservationConstants(conserved_cycles);

    //this->foldConservationConstants(conserved_cycles,this->updateVector);
    //this->foldConservationConstants(this->conserved_cycles,this->JacobianM);

}


void
ConservationConstantCollector::getInitialState(Eigen::VectorXd &x)
{
  // deterministic initial conditions for state
  //x<<(this->ICsPermuted).head(this->numIndSpecies());

}

void
ConservationConstantCollector::getConservedCycles(Eigen::VectorXd &cycles)
{
  //cycles = this->conserved_cycles;
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

