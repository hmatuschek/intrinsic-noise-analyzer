#include "ssebasemodel.hh"
#include "trafo/constantfolder.hh"

using namespace iNA;
using namespace iNA::Models;


SSEBaseModel::SSEBaseModel(const Ast::Model &model)
  : ConservationAnalysis(model),
    rate_expressions(this->numReactions()),
    rate_corrections(this->numReactions()),
    rates_gradient(this->numReactions(),this->numIndSpecies()),
    rates_gradientO1(this->numReactions(),this->numIndSpecies()),
    rates_hessian(this->numReactions(),this->numIndSpecies()*(this->numIndSpecies()+1)/2),
    rates_3rd(this->numReactions(),this->numIndSpecies()*(this->numIndSpecies()+1)*(this->numIndSpecies()+2)/6),
    REs(this->numIndSpecies()), REcorrections(this->numIndSpecies()),
    JacobianM(this->numIndSpecies(),this->numIndSpecies()),
    Hessian(this->numIndSpecies(), (this->numIndSpecies()*(this->numIndSpecies()+1))/2),
    DiffusionMatrix(this->numIndSpecies(),this->numIndSpecies()),
    DiffusionMatrixO1(this->numIndSpecies(),this->numIndSpecies()),
    DiffusionVec(numIndSpecies()*numIndSpecies()),
    DiffusionJacM(numIndSpecies()*(numIndSpecies()+1)/2,numIndSpecies()),
    DiffusionJacMO1(numIndSpecies()*(numIndSpecies()+1)/2,numIndSpecies()),
    Diffusion3Tensor(numIndSpecies()*(numIndSpecies()+1)*(numIndSpecies()+2)/6),
    DiffusionHessianM(numIndSpecies()*(numIndSpecies()+1)/2,numIndSpecies()*(numIndSpecies()+1)/2),
    PhilippianM(numIndSpecies()*(numIndSpecies()+1)*(numIndSpecies()+2)/6,numIndSpecies()*(numIndSpecies()+1)*(numIndSpecies()+2)/6)
{
  postConstructor();
}

GiNaC::exmap SSEBaseModel::makeExclusionTable(const ParameterSet & parameters)
{
    GiNaC::exmap result;
    for(ParameterSet::const_iterator it=parameters.begin(); it!=parameters.end(); it++)
        result.insert(std::pair<GiNaC::ex,GiNaC::ex>( this->getParameter((*it).first)->getSymbol(), (*it).second ));
    return result;
}

void
SSEBaseModel::postConstructor()
{


    // get gradient/hessian of rates
    for (size_t i=0; i<this->numReactions(); i++)
    {

      // substitute conservation relations
      rate_expressions(i) = this->rates[i].subs(dependentSpecies);
      // substitute conservation relations
      rate_corrections(i) = this->rates1[i].subs(dependentSpecies);

      int idx=0;
      int idy=0;
      for (size_t j=0; j<this->numIndSpecies(); j++)
      {

        // differentiate
        rates_gradient(i,j) = GiNaC::diff(rate_expressions(i), species[PermutationVec(j)]);
        rates_gradientO1(i,j) = GiNaC::diff(rate_corrections(i), species[PermutationVec(j)]);

        for (size_t k=0; k<=j; k++)
        {
            // differentiate again
            rates_hessian(i,idx) = GiNaC::diff(rates_gradient(i,j), species[PermutationVec(k)]);
            for(size_t l=0; l<=k; l++)
            {
                rates_3rd(i,idy) =
                        GiNaC::diff( rates_hessian(i,idx), species[PermutationVec(l)]);
                idy++;
            }
            idx++;

        }

      }
    }

    // write rate equation vector and reduced Jacobian matrix
    this->REs = this->reduced_stoichiometry.cast< GiNaC::ex >()*rate_expressions;
    this->REcorrections = this->reduced_stoichiometry.cast< GiNaC::ex >()*rate_corrections;
    this->JacobianM = this->reduced_stoichiometry.cast< GiNaC::ex >()*rates_gradient;
    this->JacobianMO1 = this->reduced_stoichiometry.cast< GiNaC::ex >()*rates_gradientO1;
    this->Hessian = this->reduced_stoichiometry.cast< GiNaC::ex >()*rates_hessian;
    this->PhilippianM = this->reduced_stoichiometry.cast< GiNaC::ex >()*rates_3rd;

    // divide by volume
    this->REs = this->Omega_ind.asDiagonal().inverse()*this->REs;
    this->REcorrections = this->Omega_ind.asDiagonal().inverse()*this->REcorrections;
    this->JacobianM = this->Omega_ind.asDiagonal().inverse()*this->JacobianM;
    this->JacobianMO1 = this->Omega_ind.asDiagonal().inverse()*this->JacobianMO1;
    this->Hessian = this->Omega_ind.asDiagonal().inverse()*this->Hessian;
    this->PhilippianM = this->Omega_ind.asDiagonal().inverse()*this->PhilippianM;

    size_t idy = 0;
    size_t idz = 0;
    // construct diffusion matrix in vectorized and matrix form
    for(size_t i=0;i<this->numIndSpecies();i++)
    {
       for(size_t j=0;j<this->numIndSpecies();j++)
       {
           size_t idx=i+j*this->numIndSpecies();
           this->DiffusionVec(idx)=0.;
           for(size_t m=0;m<this->numReactions();m++)
           {
             this->DiffusionMatrixO1(i,j)+=(this->reduced_stoichiometry.cast< GiNaC::ex >()(i,m)*rate_corrections(m)*this->reduced_stoichiometry.transpose().cast< GiNaC::ex >()(m,j));
             this->DiffusionVec(idx)=this->DiffusionVec(idx)+
                       (this->reduced_stoichiometry.cast< GiNaC::ex >()(i,m)*rate_expressions(m)*this->reduced_stoichiometry.transpose().cast< GiNaC::ex >()(m,j));
           }
           //divide by volume squared
           this->DiffusionMatrixO1(i,j)/=this->Omega_ind(i)*this->Omega_ind(j);
           this->DiffusionVec(idx)/=this->Omega_ind(i)*this->Omega_ind(j);
           //store also in matrix form
           this->DiffusionMatrix(i,j) = this->DiffusionVec(idx);
       }

       for(size_t j=0;j<=i;j++)
       {
           for(size_t k=0;k<numIndSpecies();k++)
           {
               for(size_t m=0;m<this->numReactions();m++)
               {
                    this->DiffusionJacM(idy,k)+=rates_gradient(m,k)
                      *this->reduced_stoichiometry.cast< GiNaC::ex >()(i,m)
                      *this->reduced_stoichiometry.cast< GiNaC::ex >()(j,m);
                   this->DiffusionJacMO1(idy,k)+=rates_gradientO1(m,k)
                     *this->reduced_stoichiometry.cast< GiNaC::ex >()(i,m)
                     *this->reduced_stoichiometry.cast< GiNaC::ex >()(j,m);
               }
               this->DiffusionJacM(idy,k)/=this->Omega_ind(i)*this->Omega_ind(j);
               this->DiffusionJacMO1(idy,k)/=this->Omega_ind(i)*this->Omega_ind(j);
           }
           idy++;

           for(size_t k=0;k<=j;k++)
           {
               for(size_t m=0;m<this->numReactions();m++)
               {
                    this->Diffusion3Tensor(idz)+=
                         rate_expressions(m)
                         *this->reduced_stoichiometry.cast< GiNaC::ex >()(i,m)
                         *this->reduced_stoichiometry.cast< GiNaC::ex >()(j,m)
                         *this->reduced_stoichiometry.cast< GiNaC::ex >()(k,m);
               }
               this->Diffusion3Tensor(idz) /= this->Omega_ind(i)*this->Omega_ind(j)*this->Omega_ind(k);
               idz++;
           }
       }



    }

    size_t idx = 0;
    for(size_t i=0;i<this->numIndSpecies();i++)
       for(size_t j=0;j<=i;j++)
       {

           idy = 0;
           for(size_t k=0;k<this->numIndSpecies();k++)
              for(size_t l=0;l<=k;l++)
              {
                  for(size_t m=0;m<this->numReactions();m++)
                      this->DiffusionHessianM(idx,idy)+=
                           rates_hessian(m,idy)
                           *this->reduced_stoichiometry.cast< GiNaC::ex >()(i,m)
                           *this->reduced_stoichiometry.cast< GiNaC::ex >()(j,m);

                  this->DiffusionHessianM(idx,idy) /= this->Omega_ind(i)*this->Omega_ind(j);
                  idy++;
              } // end idy

           idx++;

       } // end idx


}


GiNaC::ex
SSEBaseModel::vertex(std::list<size_t> &lower, std::list<size_t> &upper, size_t order)

{

    // this is not at all the be best way to do it!!!

    std::vector<GiNaC::symbol> indSpecies(numIndSpecies());

    for(size_t i=0; i<numIndSpecies(); i++){
    for(size_t j=0; j<numSpecies(); j++){
       if(PermutationM(i,j)){
           indSpecies[i] = this->species[j];
           break;
       }
    }}

    GiNaC::ex vertex=0;
    GiNaC::ex tempv;

    std::list<size_t>::iterator i;

    for(size_t j=0; j < this->numReactions() ; j++)
    {
        switch(order)
        {
          case 0:
            tempv = this->rate_expressions(j); break;
          case 1:
            tempv = this->rate_corrections(j); break;
          default:
            throw InternalError("order not defined");
        }

        for(i=lower.begin(); i != lower.end(); ++i)
            tempv *= this->reduced_stoichiometry(*i,j)/this->Omega_ind(*i);
        vertex += tempv;
    }

    for(i = upper.begin(); i != upper.end(); ++i)
        vertex = GiNaC::diff(vertex, indSpecies[*i]);

    return vertex;

}
