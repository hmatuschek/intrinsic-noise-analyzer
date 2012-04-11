#include "lnamixin.hh"

using namespace Fluc;
using namespace Fluc::Models;

LNAMixin::LNAMixin(BaseModel &base):
  preLNAMixin(base), ConservationAnalysisMixin(base)
{

    // get Omega vectors for dependent and independent species
    this->Omega_ind = (this->PermutationM.cast<GiNaC::ex>()*this->volumes).head(this->numIndSpecies());
    this->Omega_dep = (this->PermutationM.cast<GiNaC::ex>()*this->volumes).tail(this->numDepSpecies());

    // immediately fold all volumes -- could be done later
    for (size_t i=0; i<base.numSpecies(); i++)
    {
      this->volumes(i)  = base.foldConstants(this->volumes(i));
    }
    // and fold volumes
    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
        this->Omega_ind(i)  = base.foldConstants(this->Omega_ind(i));
    }

    for (size_t i=0; i<this->numDepSpecies(); i++)
    {
        this->Omega_dep(i)  = base.foldConstants(this->Omega_dep(i));
    }

}
