#include "REmodel.hh"
#include "ode/ode.hh"

using namespace Fluc;
using namespace Fluc::Models;

REmodel::REmodel(const Ast::Model &model)
  : SSEBaseModel(model),
    Link0CMatrixNumeric(numDepSpecies(), numIndSpecies()),
    LinkCMatrixNumeric(numSpecies(), numIndSpecies()),
    Omega(numSpecies()),
    updateVector(numIndSpecies()),
    interpreter(*this)
{
  postConstructor();
}


void
REmodel::postConstructor()
{
    // set dimension
    dim = this->numIndSpecies();

    // setup index table
    for(size_t i = 0; i<this->numIndSpecies(); i++)
        this->stateIndex.insert(std::make_pair(this->species[PermutationVec(i)],i));

    // Evaluate initial concentrations and evaluate volumes:
    Ast::EvaluateInitialValue evICs(*this);
    Eigen::VectorXd ICs(species.size());
    for(size_t i=0; i<species.size();i++){
       ICs(i)=evICs.evaluate(this->species[i]);
       this->Omega(i)=evICs.evaluate(this->volumes(i));
    }

    //store in permutated base
    this->Omega = this->PermutationM*this->Omega;
    this->ICsPermuted = this->PermutationM*ICs;

    for(size_t i=0;i<this->numSpecies();i++)
        for(size_t j=0;j<this->numIndSpecies();j++)
            this->LinkCMatrixNumeric(i,j) = GiNaC::ex_to<GiNaC::numeric>( this->LinkCMatrix(i,j) ).to_double();

    for(size_t i=0;i<this->numDepSpecies();i++)
        for(size_t j=0;j<this->numIndSpecies();j++)
            this->Link0CMatrixNumeric(i,j) = GiNaC::ex_to<GiNaC::numeric>( this->Link0CMatrix(i,j) ).to_double();

    /**
    * @todo the omega business is ugly and workaround at the moment
    */

    if(numDepSpecies()>0){
        Eigen::VectorXd om_dep=this->Omega.tail(this->numDepSpecies());
        // compute conserved cycles in permutated base and store in conserved_cycles
        this->conserved_cycles = om_dep.asDiagonal().inverse()*this->conservation_matrix*(this->Omega.asDiagonal())*this->ICsPermuted;
    }

    // and combine to update vector
    this->updateVector = this->REs;
    this->foldConservationConstants(conserved_cycles,this->updateVector);

    this->foldConservationConstants(this->conserved_cycles,this->JacobianM);

}


void
REmodel::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &full_state)
{
  // full state vector permutated
  Eigen::VectorXd Species_all(this->numSpecies());

  if(this->numDepSpecies()>0){

    // reconstruct dependent species
    Eigen::VectorXd Species_dep;
    Species_dep=this->conserved_cycles+this->Link0CMatrixNumeric*state.head(this->numIndSpecies());

    // fill full state
    Species_all<<state.head(this->numIndSpecies()),Species_dep;

   }
   else
   {
     Species_all<<state.head(this->numIndSpecies());
   }

   // restore original order and return
   full_state = (this->PermutationM.transpose())*Species_all;

}

GiNaC::exmap
REmodel::getFlux(const Eigen::VectorXd &state, Eigen::VectorXd &flux)

{

    flux.resize(this->numReactions());

    GiNaC::exmap subtab;
    for(size_t s=0; s<this->numIndSpecies(); s++)
        subtab.insert( std::pair<GiNaC::ex,GiNaC::ex>( getREvar(s), state(s) ) );

    this->foldConservationConstants(conserved_cycles,this->rate_expressions);

    for(size_t i=0; i<numReactions();i++)
        flux(i)=GiNaC::ex_to<GiNaC::numeric>(this->rate_expressions(i).subs(subtab)).to_double();

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
  // deterministic initial conditions for state
  x<<(this->ICsPermuted).head(this->numIndSpecies());

}

void
REmodel::getConservedCycles(Eigen::VectorXd &cycles)
{
  cycles = this->conserved_cycles;
}

void
REmodel::getOmega(Eigen::VectorXd &om)
{
  om = this->Omega;
}

//void
//REmodel::getConservedCycles(std::vector<GiNaC::ex> &cLaw)
//{
//  // permute species vector
//  std::vector<GiNaC::ex> SpecPermutated(this->species.size(),0);
//  for (size_t i=0; i<this->species.size(); i++)
//  {
//    for (size_t j=0; j<this->species.size(); j++)
//    {
//      SpecPermutated[i]+=this->PermutationM(i,j)*this->species[j];
//    }
//  }

//  // number of conservation laws
//  size_t dimKernel = this->conservation_matrix.rows();

//  if(dimKernel)
//  {
//    //report conservation laws
//    std::vector<GiNaC::ex> cand(dimKernel);

//    for (size_t i=0; i<dimKernel; i++)
//    {
//      for (size_t j=0; j<this->species.size(); j++)
//      {
//        cand[i]=cand[i]+this->conservation_matrix(i,j)*SpecPermutated[j];
//      }
//    }

//    cLaw = cand;
//  }

//}

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


void
REmodel::dump(std::ostream &str)
{

  str << std::endl << std::endl;

  str << "Species (" << this->species.size() << "):\t ";
  for (size_t i=0; i<this->species.size(); i++)
  {
    str << this->species[i] << "\t";
  }
  str << std::endl;

  str << std::endl << std::endl;

  Eigen::VectorXd ics;
  ics = this->PermutationM*this->ICsPermuted;

  str << "ICs (" << this->species.size() << "):\t ";
  for (size_t i=0; i<this->species.size(); i++)
  {
    str << ics(i) << "\t";
  }
  str << std::endl;

  str << std::endl << std::endl;

  str << "Volumes (" << this->Omega.size() << "): ";
  for (int i=0; i<this->Omega.size(); i++)
  {
    str << this->Omega(i) << "\t";
  }
  str << std::endl<< std::endl;

  str << "Reactions (" << this->reactions.size() << "): ";
  for (size_t i=0; i<this->reactions.size(); i++)
  {
    str << this->reactions[i]->getIdentifier() << " ";
  }
  str << std::endl << std::endl;

  str << "Reaction rates (" << this->rates.size() << "): ";
  for (size_t i=0; i<this->rates.size(); i++)
  {
    str << this->rates[i] << " ";
  }
  str << std::endl << std::endl;

  str << "Rate Equations (" << this->REs.size() << "): ";
  for (int i=0; i<this->REs.size(); i++)
  {
    str << this->REs(i) << " ";
  }
  str << std::endl << std::endl;

}
