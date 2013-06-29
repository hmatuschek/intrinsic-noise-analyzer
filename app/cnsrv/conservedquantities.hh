#ifndef __INA_CONSERVEDQUANTITIES_HH
#define __INA_CONSERVEDQUANTITIES_HH

#include "ginac/ginac.h"
#include "models/basemodel.hh"
#include "models/conservationanalysismixin.hh"

namespace iNA {
namespace Models {

class ConservedQuantities:
    public BaseModel,
    public ConservationAnalysisMixin
{
protected:

    /**
    *  \f$ \Omega \f$-vector for independent species
    */
    Eigen::VectorXex Omega_ind;

    /**
    *  \f$ \Omega \f$-vector for dependent species
    */
    Eigen::VectorXex Omega_dep;

    Eigen::VectorXex Omega;

public:

    /**
     * Constructor.
     */
    ConservedQuantities(const Ast::Model &model)
      : BaseModel(model),
        ConservationAnalysisMixin((BaseModel &)(*this)),
        Omega(numSpecies())

  {
      for(size_t i=0; i<numSpecies(); i++)
        Omega(i) = this->getSpecies(i)->getCompartment()->getSymbol();

      // get Omega vectors for dependent and independent species
      this->Omega_ind = (this->PermutationM.cast<GiNaC::ex>()*Omega).head(this->numIndSpecies());
      this->Omega_dep = (this->PermutationM.cast<GiNaC::ex>()*Omega).tail(this->numDepSpecies());

  }


    /** Yields the values of conserved cycles as as functions of the initial conditions. */
    Eigen::VectorXex getConservedCycles(const Eigen::VectorXex &ICs)
    {
      if(!this->speciesHaveSubstanceUnits())
      {
        return ( this->Omega_dep.asDiagonal().inverse()
                 *(conservation_matrix.cast<GiNaC::ex>())
                 *(this->Omega.asDiagonal())
                 *(this->PermutationM.cast<GiNaC::ex>())
                 *ICs);
      }
      else
      {
        return (conservation_matrix.cast<GiNaC::ex>())
                *(this->PermutationM.cast<GiNaC::ex>())
                *ICs;
      }

    }

};

}} // namespace

#endif // ConservedQuantities_HH
