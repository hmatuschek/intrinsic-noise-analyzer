#ifndef __FLUC_MODELS_CONSERVATIONANALYSISMIXIN_HH__
#define __FLUC_MODELS_CONSERVATIONANALYSISMIXIN_HH__

#include "constantstoichiometrymixin.hh"

namespace Fluc {
namespace Models {

/**
 * Performs conservation analysis and
 * and assembles the reduced stoichiometry matrix of the model.
 *
 * The mixin uses the LU decomposition of the full stoichiometric matrix \f$ S \f$:
 * \f[ S=PAQ=LU\f]
 * The matrix \f$ Q \f$ is the permutation matrix (\f$ Q^T=Q^{-1} \f$) for which the permutation of the vector of concentrations
 * is the vector with the first \f$n\f$ elements being independent species. Here \f$n\f$ is
 * the dimension of the image of \f$S\f$.
 *
 * The method has been described by Vallabhajosyula, Chickarmane and Sauro \cite vallabhajosyula2006.
 *
 * @ingroup models
 */

class ConservationAnalysisMixin :
   public ConstantStoichiometryMixin
{
protected:


    /**
    * holds the reduced stoichiometric matrix after conservation analysis
    **/
    Eigen::MatrixXd  reduced_stoichiometry;

    /**
    * the permutation matrix sorts a concentration vector with the independent species first
    **/
    Eigen::MatrixXd  PermutationM;

    /**
    * @todo: implement PermutationM as proper permutation matrix from which the index vector can be accessed.
    **/
    Eigen::VectorXi  PermutationVec;

    /**
    * this is the kernel of the stoichiometric matrix in the permutated base
    **/
    Eigen::MatrixXd  conservation_matrix;

    /**
    * \f$ {L}_0 \f$ matrix, see Vallabhajosyula et al. \cite vallabhajosyula2006.
    * @todo document this matrix
    **/
    Eigen::MatrixXd  link_zero_matrix;

    /**
    * the link matrix \f${L}\f$, see Vallabhajosyula et al. \cite vallabhajosyula2006.
    * @todo document this matrix
    **/
    Eigen::MatrixXd  link_matrix;

private:

    /**
    * internal storage for number of dependent species
    */
    size_t num_dep_species;

    /**
    * internal storage for number of independent species
    */
    size_t num_ind_species;

public:

   /**
   * Constructor performs a conservation analysis on base model.
   **/
   ConservationAnalysisMixin(BaseModel &base);
   
   /**
   * yields reduced stoichiometric matrix after conservation analysis
   **/

   void getReducedStoichiometry(Eigen::MatrixXd &stoichiometry);

   /**
   * Returns number of independent species after conservation analysis
   **/
   size_t numIndSpecies();

   /**
   * Returns number of dependent species (equals the number of conservation laws)
   **/
   size_t numDepSpecies();

};


}
}

#endif // __FLUC_MODELS_CONSERVATIONANALYSISMIXIN_HH__
