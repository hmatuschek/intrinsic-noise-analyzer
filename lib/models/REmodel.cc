#include "REmodel.hh"
#include "trafo/constantfolder.hh"

using namespace iNA;
using namespace iNA::Models;

REmodel::REmodel(const Ast::Model &model)
  : SSEBaseModel(model),
    _sseLength(this->numSpecies()),_lnaLength(0), _iosLength(0)
{
    // set dimension
    dim = this->numIndSpecies();

    // setup index table
    for(size_t i = 0; i<this->numIndSpecies(); i++)
        this->stateIndex.insert(std::make_pair(this->species[PermutationVec(i)],i));

    // and combine to update vector
    this->updateVector = this->REs;
}

void
REmodel::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &full_state)
{

    InitialConditions context(*this);

    fullState(context,state,full_state);

//  // make space
//  full_state.resize(this->numSpecies());
//  full_state.head(this->numIndSpecies()) = state.head(this->numIndSpecies());

//  if(this->numDepSpecies()>0){
//      full_state.tail(this->numDepSpecies()) =
//              Eigen::ex2double(this->conserved_cycles)
//              + this->Link0CMatrixNumeric*state.head(this->numIndSpecies());
//  }

//  // restore original order and return
//  full_state = (this->PermutationM.transpose())*full_state;

}


void
REmodel::fullState(InitialConditions &context,const Eigen::VectorXd &state, Eigen::VectorXd &full_state)
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


void
REmodel::getInitialState(Eigen::VectorXd &x)
{

  InitialConditions ICs(*this);
  getInitial(ICs, x);

}

void
REmodel::getInitial(InitialConditions &ICs,Eigen::VectorXd &x)

{
  x = ICs.getInitialState();
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

size_t
REmodel::lnaLength()
{

    return _lnaLength;

}


size_t
REmodel::iosLength()
{

    return _iosLength;

}


size_t
REmodel::sseLength()
{

    return _iosLength+_lnaLength;

}

