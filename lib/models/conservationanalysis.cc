#include "conservationanalysis.hh"
#include "trafo/constantfolder.hh"


using namespace iNA;
using namespace iNA::Models;

ConservationAnalysis::ConservationAnalysis(const Ast::Model &model)
    : BaseModel(model),
      propensityExpansion((BaseModel &)(*this)),
      ConservationAnalysisMixin((BaseModel &)(*this)),
      ICs(species.size()),
      conservationConstants(this->numDepSpecies()),
      Link0CMatrix(this->numDepSpecies(),this->numIndSpecies()),
      LinkCMatrix(this->numSpecies(),this->numIndSpecies()),
      Omega(this->numSpecies())

{

    // get Omega vectors for dependent and independent species
    this->Omega_ind = (this->PermutationM.cast<GiNaC::ex>()*this->volumes).head(this->numIndSpecies());
    this->Omega_dep = (this->PermutationM.cast<GiNaC::ex>()*this->volumes).tail(this->numDepSpecies());

    // initalize symbols as placeholders for constants arising from conservation laws
    for(size_t i=0;i<numDepSpecies();i++)
      conservationConstants(i) = GiNaC::symbol("conservation constant");

    // construct Link zero matrix for concentrations
    this->Link0CMatrix = this->Omega_dep.asDiagonal().inverse()*this->link_zero_matrix.cast<GiNaC::ex>()*this->Omega_ind.asDiagonal();

    // construct Link matrix for concentrations
    Omega = this->PermutationM.cast<GiNaC::ex>()*this->volumes;
    this->LinkCMatrix = Omega.asDiagonal().inverse()*this->link_matrix.cast<GiNaC::ex>()*this->Omega_ind.asDiagonal();

    Eigen::VectorXex spec(this->numSpecies());
    for(size_t j=0; j<this->species.size(); j++)
        spec(j) = this->species[j];

    Eigen::VectorXex ind_species = (this->PermutationM.cast<GiNaC::ex>() * spec).head(this->numIndSpecies());
    Eigen::VectorXex dep_species = (this->PermutationM.cast<GiNaC::ex>() * spec).tail(this->numDepSpecies());

    // reconstruct dependencies due to conservation laws
    // and substititute @c cconstants as constants
    Eigen::VectorXex dependence = conservationConstants.cast<GiNaC::ex>() + this->Link0CMatrix.cast<GiNaC::ex>()*ind_species;

    // generate substitution table to remove dependent species
    for (size_t s=0; s<this->numDepSpecies(); s++)
        this->dependentSpecies.insert( std::pair<GiNaC::ex,GiNaC::ex>( dep_species(s), dependence(s) ) );

    /////////////////////////////
    // resolve conservation laws

    for(size_t i=0; i<species.size();i++)
      ICs(i) = GiNaC::symbol("ICs");

    Eigen::VectorXex conserved_cycles;
    if(numDepSpecies()>0) conserved_cycles = getConservedCycles(ICs);

    // generate substitution table
    substitutions = getConservationConstants(conserved_cycles);

}

Eigen::MatrixXex
ConservationAnalysis::getConservationMatrix()

{
    return this->Omega_dep.asDiagonal().inverse()*(conservation_matrix.cast<GiNaC::ex>())*(this->Omega.asDiagonal())*(this->PermutationM.cast<GiNaC::ex>());
}

Eigen::VectorXex
ConservationAnalysis::getConservedAmounts(const Eigen::VectorXex &InitialAmount)

{
    return (conservation_matrix.cast<GiNaC::ex>())*(this->PermutationM.cast<GiNaC::ex>())*(InitialAmount);
}

Eigen::VectorXex
ConservationAnalysis::getConservedAmounts(const Eigen::VectorXd &InitialAmount)

{
    return (conservation_matrix.cast<GiNaC::ex>())*(this->Omega.asDiagonal())*(this->PermutationM.cast<GiNaC::ex>())*(InitialAmount.cast<GiNaC::ex>());
}

Eigen::VectorXex
ConservationAnalysis::getConservedCycles(const Eigen::VectorXd &ICs)

{
    return this->getConservationMatrix()*(ICs.cast<GiNaC::ex>());
}


Eigen::VectorXex
ConservationAnalysis::getConservedCycles(const Eigen::VectorXex &ICs)

{
    return this->getConservationMatrix()*ICs;
}

const Eigen::MatrixXex &
ConservationAnalysis::getLink0CMatrix()

{
    return this->Link0CMatrix;
}

const Eigen::MatrixXex
ConservationAnalysis::getLinkCMatrix()

{
    return (this->PermutationM.cast<GiNaC::ex>().transpose())*this->LinkCMatrix;
}

GiNaC::exmap
ConservationAnalysis::getConservationConstants(const Eigen::VectorXex &conserved_cycles)

{

    // generate substitution table
    GiNaC::exmap table;
    for (size_t s=0; s<this->numDepSpecies(); s++)
        table.insert( std::pair<GiNaC::ex,GiNaC::ex>( this->conservationConstants(s), conserved_cycles(s) ) );

    return table;

}


GiNaC::exmap
ConservationAnalysis::getConservationConstants(const Eigen::VectorXd &conserved_cycles)

{

    // generate substitution table
    GiNaC::exmap table;
    for (size_t s=0; s<this->numDepSpecies(); s++)
        table.insert( std::pair<GiNaC::ex,GiNaC::ex>( this->conservationConstants(s), conserved_cycles(s) ) );

    return table;

}


const Eigen::Matrix<GiNaC::symbol,Eigen::Dynamic,1> &
ConservationAnalysis::getConservationConstants()

{
    return this->conservationConstants;
}

GiNaC::exmap
ConservationAnalysis::getICconstants(const Eigen::VectorXd &initialcondition)

{

    // generate substitution table
    GiNaC::exmap table;
    for (size_t s=0; s<this->numDepSpecies(); s++)
        table.insert( std::pair<GiNaC::ex,GiNaC::ex>( this->ICs(s), initialcondition(s) ) );

    return table;

}
