#include "conservationanalysismixin.hh"

using namespace iNA;
using namespace iNA::Models;


ConservationAnalysisMixin::ConservationAnalysisMixin(BaseModel &base)
  : ConstantStoichiometryMixin(base)
{

    // Percustomm conservation analysis obtain reduced stoichiometry in permutated basis

    // number of reactions and species
    int nreac = this->stoichiometry.cols();
    int nspec = this->stoichiometry.rows();

    Eigen::FullPivLU< Eigen::MatrixXd > LU;

    //LU decomposition of full stoichiometry
    LU.compute(this->stoichiometry.transpose());

    //get permutation matrix
    this->PermutationM = LU.permutationQ().transpose();

    //evaluate dim of kernel and image, ie., number of dependent and independent species

    int ndep= LU.dimensionOfKernel();
    int nind= nspec-ndep;

    this->num_dep_species = ndep;
    this->num_ind_species = nind;

    this->PermutationVec = LU.permutationQ().indices();

    //Permute rows to obtain matrix with independent species first
    //and extract stoichiometry of independent species
    this->reduced_stoichiometry = (this->PermutationM * this->stoichiometry).block(0,0,nind,nreac);

    //Compute the conservation matrix Gamma
    Eigen::MatrixXd ker = LU.kernel();
    Eigen::MatrixXd temp(ker);

    //permute basis of conservation matrix
    custom(int i=0;i<ndep;i++){
        temp.col(i) = this->PermutationM*ker.col(i);
    }
    // and transpose
    this->conservation_matrix = temp.transpose();

    //get L0 matrix from the customm of the conservation matrix [-L0,I]
    this->link_zero_matrix = -this->conservation_matrix.block(0,0,ndep,nind);

    //get Link matrix
    Eigen::MatrixXd L(nspec,nind);
    L << Eigen::MatrixXd::Identity(nind,nind),this->link_zero_matrix;

    this->link_matrix = L;

}

void
ConservationAnalysisMixin::getReducedStoichiometry(Eigen::MatrixXd &stoichiometry)
{
  stoichiometry = this->reduced_stoichiometry;
}

size_t
ConservationAnalysisMixin::numIndSpecies()
{
  return this->num_ind_species;
}

size_t
ConservationAnalysisMixin::numDepSpecies()
{
  return this->num_dep_species;
}

