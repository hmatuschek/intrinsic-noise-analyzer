#include "conservationanalysis.hh"
#include "trafo/constantfolder.hh"


using namespace iNA;
using namespace iNA::Models;

ConservationAnalysis::ConservationAnalysis(const Ast::Model &model)
    : BaseModel(model),
      propensityExpansion((BaseModel &)(*this)),
      ConservationAnalysisMixin((BaseModel &)(*this)),
      conservationConstants(this->numDepSpecies()),
      Link0CMatrix(this->numDepSpecies(),this->numIndSpecies()),
      LinkCMatrix(this->numSpecies(),this->numIndSpecies()),
      Omega(this->numSpecies()),
      Link0CMatrixNumeric(this->numDepSpecies(),this->numIndSpecies()),
      LinkCMatrixNumeric(this->numSpecies(),this->numIndSpecies())

{

    // get Omega vectors for dependent and independent species
    this->Omega_ind = (this->PermutationM.cast<GiNaC::ex>()*this->volumes).head(this->numIndSpecies());
    this->Omega_dep = (this->PermutationM.cast<GiNaC::ex>()*this->volumes).tail(this->numDepSpecies());

    // initalize symbols as placeholders for constants arising from conservation laws
    for(size_t i=0;i<numDepSpecies();i++)
        conservationConstants(i) = GiNaC::symbol();

    // construct Link zero matrix for concentrations
    this->Link0CMatrix = this->Omega_dep.asDiagonal().inverse()*this->link_zero_matrix.cast<GiNaC::ex>()*this->Omega_ind.asDiagonal();

    // construct Link zero matrix for concentrations
    Omega = this->PermutationM.cast<GiNaC::ex>()*this->volumes;
    this->LinkCMatrix = Omega.asDiagonal().inverse()*this->link_matrix.cast<GiNaC::ex>()*this->Omega_ind.asDiagonal();

    Eigen::VectorXex spec(this->numSpecies());
    for(size_t j=0; j<this->species.size(); j++)
        spec(j) = this->species[j];

    Eigen::VectorXex ind_species = (this->PermutationM.cast<GiNaC::ex>() * spec).head(this->numIndSpecies());
    Eigen::VectorXex dep_species = (this->PermutationM.cast<GiNaC::ex>() * spec).tail(this->numDepSpecies());

    // reconstruct dependencies due to conservation laws
    // and substititute @c cconstants as constants
    Eigen::VectorXex dependence = conservationConstants + this->Link0CMatrix.cast<GiNaC::ex>()*ind_species;

    // generate substitution table to remove dependent species
    for (size_t s=0; s<this->numDepSpecies(); s++)
        this->dependentSpecies.insert( std::pair<GiNaC::ex,GiNaC::ex>( dep_species(s), dependence(s) ) );

    /////////////////////////////
    //plugin some constants

    // Evaluate initial concentrations and evaluate volumes:
    Trafo::InitialValueFolder evICs(*this);
    Eigen::VectorXd ICs(species.size());
    for(size_t i=0; i<species.size();i++){
       ICs(i)=evICs.evaluate(this->species[i]);
    }

    //store in permutated base
    this->ICsPermuted = this->PermutationM*ICs;

    Trafo::ConstantFolder constants(*this);

    //evaluate the link matrices
    Link0CMatrixNumeric = Eigen::ex2double(constants.apply(this->Link0CMatrix));
    LinkCMatrixNumeric = Eigen::ex2double(constants.apply(this->LinkCMatrix));

    if(numDepSpecies()>0) this->conserved_cycles = getConservedCycles(ICs);

    // generate substitution table
    substitutions = getConservationConstants(conserved_cycles);

}

//Eigen::MatrixXex
//ConservationAnalysis::getConservedCycles(const Eigen::VectorXd &ICs)
//{

//    Trafo::ConstantFolder constants(*this);
//    return constants.apply(this->Omega_dep.asDiagonal().inverse()*(conservation_matrix.cast<GiNaC::ex>())*(this->Omega.asDiagonal())*(this->PermutationM.cast<GiNaC::ex>())*(ICs.cast<GiNaC::ex>()));

//}

Eigen::MatrixXex
ConservationAnalysis::getConservedCycles(const Eigen::VectorXd &ICs)
{

    return this->Omega_dep.asDiagonal().inverse()*(conservation_matrix.cast<GiNaC::ex>())*(this->Omega.asDiagonal())*(this->PermutationM.cast<GiNaC::ex>())*(ICs.cast<GiNaC::ex>());

}

Eigen::MatrixXex
ConservationAnalysis::getConservationMatrix()
{

    return this->Omega_dep.asDiagonal().inverse()*(conservation_matrix.cast<GiNaC::ex>())*(this->Omega.asDiagonal())*(this->PermutationM.cast<GiNaC::ex>());

}

const Eigen::MatrixXex &
ConservationAnalysis::getLink0CMatrix()
{

    return this->Link0CMatrix;

}

const Eigen::MatrixXex &
ConservationAnalysis::getLinkCMatrix()
{

    return this->PermutationM.transpose().cast<GiNaC::ex>()*this->LinkCMatrix;

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
