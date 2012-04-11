#include "lnabasemodel.hh"
#include "eigen3/Eigen/Core"

using namespace Fluc;
using namespace Fluc::Models;

LNABaseModel::LNABaseModel(libsbml::Model *model)
  : BaseModel(model), LNAMixin((BaseModel &)(*this)),
    rate_expressions(this->numReactions()),
    rate_corrections(this->numReactions()),
    rates_gradient(this->numReactions(),this->numSpecies()),
    rates_hessian(this->numReactions(),this->numSpecies()*(this->numSpecies()+1)/2),
    Link0CMatrix(this->numDepSpecies(),this->numIndSpecies()),
    LinkCMatrix(this->numSpecies(),this->numIndSpecies()),
    REs(this->numIndSpecies()), REcorrections(this->numIndSpecies()),
    JacobianM(this->numIndSpecies(),this->numIndSpecies()),
    Hessian(this->numSpecies(), (this->numSpecies()*(this->numSpecies()+1))/2),
    DiffusionMatrix(this->numIndSpecies(),this->numIndSpecies()),
    DiffusionVec(numIndSpecies()*numIndSpecies()),
    conservationConstants(this->numDepSpecies())
{
    /* @todo conservationConstants should be have a seperate class */

    /* @todo all LNA coeff should be calculated in unconstrained base */

    // initalize symbols as placeholders for constants arising from conservation laws
    for(size_t i=0;i<numDepSpecies();i++)
        conservationConstants(i) = GiNaC::symbol();

    // construct Link zero matrix for concentrations
    this->Link0CMatrix = this->Omega_dep.asDiagonal().inverse()*this->link_zero_matrix.cast<GiNaC::ex>()*this->Omega_ind.asDiagonal();

    // construct Link zero matrix for concentrations
    Eigen::VectorXex Omega = this->PermutationM.cast<GiNaC::ex>()*this->volumes;
    this->LinkCMatrix = Omega.asDiagonal().inverse()*this->link_matrix.cast<GiNaC::ex>()*this->Omega_ind.asDiagonal();

    Eigen::VectorXex spec(this->numSpecies());
    Eigen::VectorXex dep_species(this->numDepSpecies());
    Eigen::VectorXex ind_species(this->numIndSpecies());

    for(size_t j=0; j<this->species.size(); j++)
    {
        spec(j) = this->species[j];
    }

    ind_species = (this->PermutationM.cast<GiNaC::ex>() * spec).head(this->numIndSpecies());
    dep_species = (this->PermutationM.cast<GiNaC::ex>() * spec).tail(this->numDepSpecies());

    // reconstruct dependencies due to conservation laws
    // and substititute @c cconstants as constants
    Eigen::VectorXex dependence = conservationConstants + this->Link0CMatrix.cast<GiNaC::ex>()*ind_species;

    // generate substitution table to remove dependent species

    GiNaC::exmap subs_table;
    for (size_t s=0; s<this->numDepSpecies(); s++)
        subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( dep_species(s), dependence(s) ) );

    // get gradient/hessian of rates
    for (size_t i=0; i<this->numReactions(); i++)
    {

      // substitute conservation relations
      rate_expressions(i) = this->rates[i].subs(subs_table);
      // substitute conservation relations
      rate_corrections(i) = this->rates1[i].subs(subs_table);

      int idx=0;
      for (size_t j=0; j<this->numSpecies(); j++)
      {

        // differentiate
        rates_gradient(i,j) = GiNaC::diff(this->rates[i], this->species[j]);

        for (size_t k=0; k<=j; k++)
        {
            // differentiate again
            rates_hessian(i,idx) = GiNaC::diff(rates_gradient(i,j), this->species[k]);

            // substitute conservation relations
            rates_hessian(i,idx) = rates_hessian(i,idx).subs(subs_table);

            idx++;
        }

        // and now substitute conservation relations
        rates_gradient(i,j) = rates_gradient(i,j).subs(subs_table);

      }
    }

    // write rate equation vector and reduced Jacobian matrix
    this->REs = this->reduced_stoichiometry.cast< GiNaC::ex >()*rate_expressions;
    this->REcorrections = this->reduced_stoichiometry.cast< GiNaC::ex >()*rate_corrections;
    this->JacobianM = this->reduced_stoichiometry.cast< GiNaC::ex >()*
            rates_gradient *
            (this->PermutationM.transpose()).cast<GiNaC::ex>()*this->LinkCMatrix;
    this->Hessian = this->reduced_stoichiometry.cast< GiNaC::ex >()*rates_hessian;

    // divide by volume
    this->REs = this->Omega_ind.asDiagonal().inverse()*this->REs;
    this->REcorrections = this->Omega_ind.asDiagonal().inverse()*this->REcorrections;
    this->JacobianM = this->Omega_ind.asDiagonal().inverse()*this->JacobianM;
    this->Hessian = this->Omega_ind.asDiagonal().inverse()*this->Hessian;

    // construct diffusion matrix in vectorized and matrix form
    for(size_t i=0;i<this->numIndSpecies();i++)
    {
       for(size_t j=0;j<this->numIndSpecies();j++)
       {
           size_t idx=i+j*this->numIndSpecies();
           this->DiffusionVec(idx)=0.;
           for(size_t m=0;m<this->numReactions();m++)
           {
             this->DiffusionVec(idx)=this->DiffusionVec(idx)+
                       (this->reduced_stoichiometry.cast< GiNaC::ex >()(i,m)*rate_expressions(m)*this->reduced_stoichiometry.transpose().cast< GiNaC::ex >()(m,j));
           }
           //divide by volume squared
           this->DiffusionVec(idx) = this->DiffusionVec(idx)/this->Omega_ind(i)/this->Omega_ind(j);
           //store also in matrix form
           this->DiffusionMatrix(i,j) = this->DiffusionVec(idx);
        }
    }

    // assemble Hessian of objective function
    Eigen::VectorXex fh = REs.transpose()*Hessian;
    Eigen::MatrixXex HessianTemp(this->numSpecies(),this->numSpecies());

    size_t idx=0;
    for (size_t j=0; j<this->numSpecies(); j++)
    {
      for (size_t k=0; k<=j; k++)
      {
            HessianTemp(j,k)=fh(idx);
            HessianTemp(k,j)=fh(idx);
            idx++;
      }
    }


    Eigen::MatrixXex mapRed = (this->PermutationM.transpose()).cast<GiNaC::ex>()*this->LinkCMatrix;

    fHessian = mapRed.transpose()*HessianTemp*mapRed + this->JacobianM.transpose()*this->JacobianM;







};

void
LNABaseModel::foldConservationConstants(const Eigen::VectorXd &conserved_cycles)
{

    // generate substitution table

    GiNaC::exmap subs_table;

    for (size_t s=0; s<this->numDepSpecies(); s++)
    {
        subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( this->conservationConstants(s), conserved_cycles(s) ) );
    }


    int idx=0;

    // ... and fold all constants due to conservation laws
    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
        this->REs(i)=this->REs(i).subs(subs_table);        
        this->REcorrections(i)=this->REcorrections(i).subs(subs_table);

        for (size_t j=0; j<this->numIndSpecies(); j++)
        {
            this->JacobianM(i,j)=this->JacobianM(i,j).subs(subs_table);
            this->DiffusionMatrix(i,j)=this->DiffusionMatrix(i,j).subs(subs_table);
            this->DiffusionVec(idx) = this->DiffusionVec(idx).subs(subs_table);
            idx++;

        }
    }

    // ... the same thing is still a bit special for the Hessian.
    for (size_t i=0; i<this->numIndSpecies(); i++)
    {

        int idy=0;
        for (size_t j=0; j<this->numSpecies(); j++)
        {

            for (size_t k=0; k<=j; k++)
            {
                this->Hessian(i,idy) = this->Hessian(i,idy).subs(subs_table);
                idy++;
            }
        }

        for (size_t j=0; j<this->numIndSpecies(); j++)
        {
            this->fHessian(i,j) = this->fHessian(i,j).subs(subs_table);
        }
    }

}

void
LNABaseModel::foldConservationConstants(const Eigen::VectorXd &conserved_cycles, Eigen::VectorXex &vec)
{

//    // generate substitution table

    GiNaC::exmap subs_table = generateConservationConstantsTable(conserved_cycles);

    // ... and fold all constants due to conservation laws
    for (int i=0; i<vec.size(); i++)
            vec(i)=vec(i).subs(subs_table);

}


GiNaC::exmap
LNABaseModel::generateConservationConstantsTable(const Eigen::VectorXd &conserved_cycles)
{

    // generate substitution table
    GiNaC::exmap subs_table;
    for (size_t s=0; s<this->numDepSpecies(); s++)
    {
        subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( this->conservationConstants(s), conserved_cycles(s) ) );
    }
    return subs_table;

}

GiNaC::ex
LNABaseModel::vertex(std::list<size_t> &lower, std::list<size_t> &upper, size_t order)
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
