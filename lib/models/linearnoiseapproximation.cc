#include "linearnoiseapproximation.hh"
#include "ode/ode.hh"

using namespace Fluc;
using namespace Fluc::Models;

LinearNoiseApproximation::LinearNoiseApproximation(libsbml::Model *model)
  : LNABaseModel(model),
    Link0CMatrixNumeric(numDepSpecies(), numIndSpecies()),
    LinkCMatrixNumeric(numSpecies(), numIndSpecies()),
    Omega(numSpecies()),
    stateVariables(getDimension()-numIndSpecies()),
    updateVector(getDimension()),
    interpreter(*this)
{
  postConstructor();
}


LinearNoiseApproximation::LinearNoiseApproximation(const Ast::Model &model)
  : LNABaseModel(model),
    Link0CMatrixNumeric(numDepSpecies(), numIndSpecies()),
    LinkCMatrixNumeric(numSpecies(), numIndSpecies()),
    Omega(numSpecies()),
    stateVariables(getDimension()-numIndSpecies()),
    updateVector(getDimension()),
    interpreter(*this)
{
  postConstructor();
}


void
LinearNoiseApproximation::postConstructor()
{
    // setup index table

    // get symbols of species
    for(size_t i = 0; i<this->numIndSpecies(); i++)
    {
        for(size_t j = 0; j<this->numSpecies(); j++)
        {
            // apply the permutation
            if(this->PermutationM(i,j)==1)
                this->stateIndex.insert(std::make_pair(this->getSpecies(j)->getSymbol(),i));
        }
    }

    // assign a set of new symbols
    // ... and add them to index table
    for(size_t i = 0; i<this->stateVariables.size(); i++)
    {
        stateVariables[i] = GiNaC::symbol();
        this->stateIndex.insert(std::make_pair(this->stateVariables[i],this->numIndSpecies()+i));
    }

    // form expressions with new symbols for remaining state variables
    size_t dimCOV = this->numIndSpecies()*(this->numIndSpecies()+1)/2;
    size_t dimSkew = this->numIndSpecies()*(this->numIndSpecies()+1)*(this->numIndSpecies()+2)/6;

    Eigen::VectorXex covVariables(dimCOV);
    Eigen::VectorXex emreVariables(this->numIndSpecies());
    Eigen::VectorXex iosVariables(dimCOV);
    Eigen::VectorXex iosemreVariables(this->numIndSpecies());
    std::vector< Eigen::MatrixXex > thirdmomentVariables(this->numIndSpecies());

    size_t idx = 0;
    for(size_t i = 0 ; i<dimCOV; i++)
        covVariables(i) = stateVariables[idx++];
    for(size_t i = 0 ; i<this->numIndSpecies(); i++)
        emreVariables(i) = stateVariables[idx++];

    for(size_t i=0;i<this->numIndSpecies();i++)
    {
        thirdmomentVariables[i].resize(this->numIndSpecies(),this->numIndSpecies());
        for(size_t j=0;j<=i;j++)
            for(size_t k=0;k<=j;k++)
            {
                thirdmomentVariables[i](j,k)=stateVariables[idx];
                thirdmomentVariables[j](i,k)=stateVariables[idx];
                thirdmomentVariables[k](i,j)=stateVariables[idx];
                idx++;
            }
    }

    for(size_t i = 0 ; i<dimCOV; i++)
        iosVariables(i) = stateVariables[idx++];
    for(size_t i = 0 ; i<this->numIndSpecies(); i++)
        iosemreVariables(i) = stateVariables[idx++];

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
    {
        for(size_t j=0;j<this->numIndSpecies();j++)
            this->LinkCMatrixNumeric(i,j) = GiNaC::ex_to<GiNaC::numeric>( this->LinkCMatrix(i,j) ).to_double();
    }

    for(size_t i=0;i<this->numDepSpecies();i++)
    {
        for(size_t j=0;j<this->numIndSpecies();j++)
            this->Link0CMatrixNumeric(i,j) = GiNaC::ex_to<GiNaC::numeric>( this->Link0CMatrix(i,j) ).to_double();
    }



    /**
    * @todo the omega business is ugly and workaround at the moment
    */

    if(numDepSpecies()>0){

        Eigen::VectorXd om_dep=this->Omega.tail(this->numDepSpecies());

        // compute conserved cycles in permutated base and store in conserved_cycles
        this->conserved_cycles = om_dep.asDiagonal().inverse()*this->conservation_matrix*(this->Omega.asDiagonal())*this->ICsPermuted;

        // now fold coverservation constants for all coefficients
        this->foldConservationConstants(this->conserved_cycles);

        // fold also conservation constants in updateVector
        this->foldConservationConstants(this->conserved_cycles,this->updateVector);

    }

    /////////////////////////////////////////////
    // construct update vector for covariances //
    /////////////////////////////////////////////

    // construct a covariance matrix

    Eigen::MatrixXex cov;
    constructCovarianceMatrix(cov);

    // determine update for covariances
    Eigen::MatrixXex cov_update;
    cov_update = (this->JacobianM*cov)+(cov*this->JacobianM.transpose())+this->DiffusionMatrix;

    // take only lower triangular and stack up to vector
    Eigen::VectorXex CovUpdate(dimCOV);

    idx=0;
    for(size_t i=0;i<this->numIndSpecies();i++)
    {
      for(size_t j=0;j<=i;j++)
      {
        CovUpdate(idx)=cov_update(i,j);
        idx++;
      }
    }

    // done with CovUpdate

    ////////////////////////
    // now calculate EMRE //
    ////////////////////////

    Eigen::VectorXex Delta(this->numIndSpecies());

    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
      Delta(i)=0.;
      idx=0;
      for (size_t j=0; j<this->numIndSpecies(); j++)
      {
        for (size_t k=0; k<=j; k++)
        {
            if(k==j)
            {
                Delta(i) += this->Hessian(i,idx)*cov(j,k);
            }
            else
            {
                // fac 2 by symmetry, saves summing over strictly upper cov matrix
                Delta(i) += 2.*this->Hessian(i,idx)*cov(j,k);
            }

            idx++;

        }
      }
      // divide by 2
      Delta(i)/=2.;
      // now add rate corrections
      Delta(i)+=this->REcorrections(i);
    }

    /////////////////////////
    // calculate EMRE update
    /////////////////////////

    Eigen::VectorXex EMREUpdate;
    EMREUpdate = ((this->JacobianM*emreVariables)+Delta);

    /////////////////////////
    // calculate third moment
    /////////////////////////


    std::vector< Eigen::MatrixXex > diffjac(this->numIndSpecies());

    for(size_t i=0;i<this->numIndSpecies();i++)
    {

        idx=0;
        diffjac[i].resize(this->numIndSpecies(),this->numIndSpecies());
        for(size_t j=0;j<this->numIndSpecies();j++)
        {
            for(size_t k=0;k<=j;k++)
            {
                diffjac[i](j,k)=this->DiffusionJacM(idx,i);
                diffjac[i](k,j)=this->DiffusionJacM(idx,i);
                idx++;
            }
        }

    }
    Eigen::VectorXex ThirdMomentUpdate(dimSkew);

    size_t ids = 0;
    idx=0;
    size_t idy;
    for(size_t i=0;i<this->numIndSpecies();i++)
        for(size_t j=0;j<=i;j++)
        {
            for(size_t k=0;k<=j;k++)
            {
                ThirdMomentUpdate(idx)+=this->Diffusion3Tensor(idx);

                ThirdMomentUpdate(idx)+=2*this->DiffusionMatrix(i,j)*emreVariables(k)+this->DiffusionMatrix(i,k)*emreVariables(j);

                idy = 0;
                for(size_t r=0; r<this->numIndSpecies(); r++)
                {

                    ThirdMomentUpdate(idx) += diffjac[r](i,j)*cov(r,k)+diffjac[r](i,k)*cov(r,j)+diffjac[r](j,k)*cov(r,i);

                    ThirdMomentUpdate(idx) += this->JacobianM(i,r)*thirdmomentVariables[r](j,k);
                    ThirdMomentUpdate(idx) += this->JacobianM(j,r)*thirdmomentVariables[r](i,k);
                    ThirdMomentUpdate(idx) += this->JacobianM(k,r)*thirdmomentVariables[r](i,j);

                    for (size_t s=0; s<=r; s++)
                    {
                        // use Wick's theorem to calculate 4-th moment
                        GiNaC::ex wick1 = (cov(s,r)*cov(j,k)+cov(s,j)*cov(r,k)+cov(s,k)*cov(r,j));
                        // i -> j
                        GiNaC::ex wick2 = (cov(s,r)*cov(i,k)+cov(s,i)*cov(r,k)+cov(s,k)*cov(r,i));
                        // j -> k
                        GiNaC::ex wick3 = (cov(s,r)*cov(i,j)+cov(s,i)*cov(r,j)+cov(s,j)*cov(r,i));

                        if(s==r)
                        {
                            ThirdMomentUpdate(idx)+=this->Hessian(i,idy)*wick1/2;
                            ThirdMomentUpdate(idx)+=this->Hessian(j,idy)*wick2/2;
                            ThirdMomentUpdate(idx)+=this->Hessian(k,idy)*wick3/2;
                        }
                        else
                        {
                            // fac 2 by symmetry, saves summing over symmetric indices of Hessian
                            ThirdMomentUpdate(idx)+=this->Hessian(i,idy)*wick1;
                            ThirdMomentUpdate(idx)+=this->Hessian(j,idy)*wick2;
                            ThirdMomentUpdate(idx)+=this->Hessian(k,idy)*wick3;
                        }

                    } //end s
                    idy++;
                } // end r loop

                idx++;
            } // end k loop

            } // end i,j,k loop

    ///////////////////////////////////////
    // calculate update for LNA correction
    ///////////////////////////////////////

    Eigen::VectorXex iosUpdate(dimCOV);

    Eigen::MatrixXex iosCov;
    constructSymmetricMatrix(iosVariables,iosCov);

    idx=0;
    idy=0;
    for(size_t i=0;i<this->numIndSpecies();i++)
    for(size_t j=0;j<=i;j++)
    {
        iosUpdate(idx)=0;

        idy=0;

        for(size_t k=0; k<this->numIndSpecies(); k++)
             {

                iosUpdate(idx) += this->DiffusionJacM(idx,k)*emreVariables(k);

                iosUpdate(idx) += this->JacobianM(i,k)*iosCov(k,j)+this->JacobianM(j,k)*iosCov(k,i);

                for(size_t l=0; l<k; l++)
                {
                    iosUpdate(idx) += this->Hessian(i,idy)*thirdmomentVariables[j](k,l)+this->Hessian(j,idy)*thirdmomentVariables[i](k,l);
                    iosUpdate(idx) += this->DiffusionHessianM(idx,idy)*iosVariables(idy);
                    idy++;
                }

                // same for l=k appears only once in sum
                iosUpdate(idx) += (this->Hessian(i,idy)+this->Hessian(j,idy))*thirdmomentVariables[j](k,k)/2;
                iosUpdate(idx) += this->DiffusionHessianM(idx,idy)*cov(k,k)/2;
             }


        idx++;
     }

    ////////////////////////
    // now calculate IOS-EMRE //
    ////////////////////////

    //Eigen::VectorXex Delta(this->numIndSpecies());

    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
      Delta(i)=0.;
      idx=0;
      for (size_t j=0; j<this->numIndSpecies(); j++)
      {
        for (size_t k=0; k<=j; k++)
        {
            if(k==j)
            {
                Delta(i) += this->Hessian(i,idx)*iosVariables(idx);
            }
            else
            {
                // fac 2 by symmetry, saves summing over strictly upper cov matrix
                Delta(i) += 2.*this->Hessian(i,idx)*iosVariables(idx);
            }

            idx++;

        }
      }
      // divide by 2
      Delta(i)/=2.;
    }

    // @todo add philippian!

    Eigen::VectorXex EMREiosUpdate = ((this->JacobianM*iosemreVariables)+Delta);

    // and combine to update vector
    this->updateVector << this->REs,CovUpdate,EMREUpdate,ThirdMomentUpdate,iosUpdate,EMREiosUpdate;
}



void
LinearNoiseApproximation::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &full_state)
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


void
LinearNoiseApproximation::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations, Eigen::MatrixXd &cov)

{

    // reconstruct full concentration vector in original permutation order
    fullState(state,concentrations);

    // ... then begin reconstruction of covariance matrix

    // dim of reduced covariance vector
    int dimCOV=(this->numIndSpecies()*(this->numIndSpecies()+1))/2;

    // get reduced covariance vector
    Eigen::VectorXd covvec = state.segment(this->numIndSpecies(),dimCOV);

    // full cov permutated
    Eigen::MatrixXd cov_all(this->numSpecies(),this->numSpecies());
    // red cov permutated
    Eigen::MatrixXd cov_ind(this->numIndSpecies(),this->numIndSpecies());

   // fill upper triangular
   size_t idx=0;
   for(size_t i=0;i<this->numIndSpecies();i++)
   {
       for(size_t j=0;j<=i;j++)
       {
           cov_ind(i,j) = covvec(idx);
           // fill rest by symmetry
           cov_ind(j,i) = cov_ind(i,j);
           idx++;
       }
   }

   // so here it is:
   cov_all = this->LinkCMatrixNumeric*cov_ind*this->LinkCMatrixNumeric.transpose();

   // restore native permutation of covariance
   cov = (this->PermutationM.transpose()*cov_all)*this->PermutationM;

}

void
LinearNoiseApproximation::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations,
                                     Eigen::MatrixXd &cov, Eigen::VectorXd &emre)

{

    // reconstruct full concentration vector and covariances in original permutation order
    fullState(state,concentrations,cov);

    // reconstruct emre

    // dim of reduced covariance vector
    int dimCOV=(this->numIndSpecies()*(this->numIndSpecies()+1))/2;

    // get reduced emre vector (should better be a view rather then a copy)
    Eigen::VectorXd tail = state.segment(this->numIndSpecies()+dimCOV,this->numIndSpecies());

    // construct full emre vector, restore original order and return
    emre = this->PermutationM.transpose()*this->LinkCMatrixNumeric*tail;

}

void
LinearNoiseApproximation::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations,
                                     Eigen::MatrixXd &cov, Eigen::VectorXd &emre, Eigen::MatrixXd &iosCov, Eigen::VectorXd &thirdMoment, Eigen::VectorXd &iosemre)

{

    // reconstruct full concentration vector and covariances in original permutation order
    fullState(state,concentrations,cov,emre);

    // reconstruct thirdmoments


    // dim of reduced covariance vector
    int dimCOV=(this->numIndSpecies()*(this->numIndSpecies()+1))/2;
    // dim of reduced covariance vector
    int dimThird=(this->numIndSpecies()*(this->numIndSpecies()+1)*(this->numIndSpecies()+2))/6;

    // get reduced skewness vector (should better be a view rather then a copy)
    Eigen::VectorXd tail = state.segment(2*this->numIndSpecies()+dimCOV,dimThird);

    std::vector< Eigen::MatrixXd > thirdMomVariables(this->numIndSpecies());

    size_t idx = 0;
    for(size_t i=0;i<this->numIndSpecies();i++)
    {
        thirdMomVariables[i].resize(this->numIndSpecies(),this->numIndSpecies());
        for(size_t j=0;j<=i;j++)
            for(size_t k=0;k<=j;k++)
            {
                thirdMomVariables[i](j,k)=tail[idx];
                thirdMomVariables[j](i,k)=tail[idx];
                thirdMomVariables[k](i,j)=tail[idx];
                idx++;
            }
    }

    Eigen::MatrixXd cmat = this->PermutationM.transpose()*this->LinkCMatrixNumeric;

    // construct full third moment vector, restore original order and return
    for(size_t i=0; i<(unsigned)cmat.rows(); i++)
        for(size_t j=0; j<(unsigned)cmat.cols(); j++)
            for(size_t k=0; k<(unsigned)cmat.cols(); k++)
                for(size_t l=0; l<(unsigned)cmat.cols(); l++)
                    thirdMoment(i) += cmat(i,j) * cmat(i,k) * cmat(i,l) *thirdMomVariables[j](k,l);

    // get reduced covariance vector
    Eigen::VectorXd covvec = state.segment(2*this->numIndSpecies()+dimCOV+dimThird,dimCOV);
    // full cov permutated
    Eigen::MatrixXd cov_all(this->numSpecies(),this->numSpecies());
    // red cov permutated
    Eigen::MatrixXd cov_ind(this->numIndSpecies(),this->numIndSpecies());

   // fill upper triangular
   idx=0;
   for(size_t i=0;i<this->numIndSpecies();i++)
   {
       for(size_t j=0;j<=i;j++)
       {
           cov_ind(i,j) = covvec(idx);
           // fill rest by symmetry
           cov_ind(j,i) = cov_ind(i,j);
           idx++;
       }
   }

   // so here it is:
   cov_all = this->LinkCMatrixNumeric*cov_ind*this->LinkCMatrixNumeric.transpose();

   // restore native permutation of covariance
   iosCov = (this->PermutationM.transpose()*cov_all)*this->PermutationM;

   tail = state.segment(2*this->numIndSpecies()+2*dimCOV+dimThird,this->numIndSpecies());

   // construct full iosemre vector, restore original order and return
   iosemre = cmat*tail;


}

void
LinearNoiseApproximation::setState(const Eigen::VectorXd &state)
{

    // construct full concentration vector
    Eigen::VectorXd concentrations(this->numSpecies());
    this->fullState(state.head(this->numIndSpecies()),concentrations);

    // First update values for state:
    this->interpreter.setValues(concentrations);

    // ...done.

}

void
LinearNoiseApproximation::getREs(const Eigen::VectorXd &state, Eigen::VectorXd &REs)
{
    // set state
    this->setState(state);
    // and get rate equations
    this->getREs(REs);
}


void
LinearNoiseApproximation::getREs(Eigen::VectorXd &REs)
{
    // make sure vector is big enough
    REs.resize(this->numIndSpecies());

    // and evaluate:
    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
        REs(i) = this->interpreter.evaluate(this->REs(i));
    }
    // ... done.
}

void
LinearNoiseApproximation::getRateCorrections(Eigen::VectorXd &REcorr)
{
    // make sure vector is big enough
    REcorr.resize(this->numIndSpecies());

    // and evaluate:
    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
        REcorr(i) = this->interpreter.evaluate(this->REcorrections(i));
    }
    // ... done.
}

void
LinearNoiseApproximation::getJacobianMatrix(Eigen::MatrixXd &JacobianMatrix)
{

    // make sure Jacobian is big enough
    JacobianMatrix.resize(this->numIndSpecies(),this->numIndSpecies());

    // and evaluate:
    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
      for (size_t j=0; j<this->numIndSpecies(); j++)
      {
        JacobianMatrix(i,j) = this->interpreter.evaluate(this->JacobianM(i,j));
      }
    }

    // ... done.

}

void
LinearNoiseApproximation::getJacobianMatrix(const Eigen::VectorXd &state, Eigen::MatrixXd &JacobianMatrix)
{
    // set state
    this->setState(state);
    // and get Jacobian matrix
    this->getJacobianMatrix(JacobianMatrix);
}


void
LinearNoiseApproximation::getDiffusionVec(Eigen::VectorXd &DiffusionVec)
{

    // make sure Diffusion vector is big enough
    DiffusionVec.resize(this->numIndSpecies()*this->numIndSpecies());

    // and evaluate:
    for (size_t i=0; i<(this->numIndSpecies()*this->numIndSpecies()); i++)
    {
        DiffusionVec(i) = this->interpreter.evaluate(this->DiffusionVec(i));
    }

    // ... done.

}

void
LinearNoiseApproximation::getDiffusionVec(const Eigen::VectorXd &state, Eigen::VectorXd &DiffusionVec)
{
    // set state
    this->setState(state);
    // and get Diffusion vector
    this->getDiffusionVec(DiffusionVec);
}


void
LinearNoiseApproximation::getDiffusionMatrix(Eigen::MatrixXd &DiffusionM)
{

    // make sure Diffusion vector is big enough
    DiffusionM.resize(this->numIndSpecies(),this->numIndSpecies());

    // just lower triangular matrix
    Eigen::MatrixXd lowerTriangD(this->numIndSpecies(),this->numIndSpecies());

    // and evaluate this:
    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
        for(size_t j=0;j<=i;j++)
        {
            lowerTriangD(i,j) = this->interpreter.evaluate(this->DiffusionMatrix(i,j));
        }
    }

    // conversion to dense matrix
    DiffusionM = lowerTriangD.selfadjointView<Eigen::Lower>();

    // ... done.

}

void
LinearNoiseApproximation::getDiffusionMatrix(const Eigen::VectorXd &state, Eigen::MatrixXd &DiffusionM)
{
    // set state
    this->setState(state);
    // and get Diffusion matrix
    this->getDiffusionMatrix(DiffusionM);
}


void
LinearNoiseApproximation::getHessian(Eigen::MatrixXd &HessianM)
{

    // make sure the matrix is big enough
    HessianM.resize(this->numIndSpecies(),this->numIndSpecies()*(this->numIndSpecies()+1)/2);

    // and evaluate this:
    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
        int idx=0;
        for (size_t j=0; j<this->numIndSpecies(); j++)
        {
            for(size_t k=0;k<=j;k++)
            {
                HessianM(i,idx) = this->interpreter.evaluate(this->Hessian(i,idx));
                idx++;
            }
        }
    }

    // ... done.

}

void
LinearNoiseApproximation::getHessian(const Eigen::VectorXd &state, Eigen::MatrixXd &HessianM)
{
    // set state
    this->setState(state);
    // and get the Hessian in matrix matrix form
    this->getHessian(HessianM);
}

size_t
LinearNoiseApproximation::getDimension()
{
  // this is enough for deterministic RE models
  int dim= this->numIndSpecies();

  // add dimension of covariances
  dim+= (this->numIndSpecies()*(this->numIndSpecies()+1))/2;

  // add dimension of EMRE
  dim+= this->numIndSpecies();

  // add dimension of third moments
  dim+= (this->numIndSpecies()*(this->numIndSpecies()+1)*(this->numIndSpecies()+2))/6;

  // add dimension of second moment correction
  dim+= (this->numIndSpecies()*(this->numIndSpecies()+1))/2;

  // add dimension of IOS-EMRE
  dim+= this->numIndSpecies();

  return dim;
}

void
LinearNoiseApproximation::getInitialState(Eigen::VectorXd &x)
{
  int dimCov=(this->numIndSpecies()*(this->numIndSpecies()+1))/2; 
  int dimThird=(this->numIndSpecies()*(this->numIndSpecies()+1)*(this->numIndSpecies()+2))/6;

  // deterministic initial conditions for state
  x<<(this->ICsPermuted).head(this->numIndSpecies()),
     // zero covariance
     Eigen::VectorXd::Zero(dimCov),
     // zero EMRE
     Eigen::VectorXd::Zero(this->numIndSpecies()),
     // zero third moment
     Eigen::VectorXd::Zero(dimThird),
     // zero second moment correction
     Eigen::VectorXd::Zero(dimCov),
     // zero IOS-EMRE
     Eigen::VectorXd::Zero(this->numIndSpecies());

}

void
LinearNoiseApproximation::getConservedCycles(Eigen::VectorXd &consc)
{
  consc = this->conserved_cycles;
}

void
LinearNoiseApproximation::getOmega(Eigen::VectorXd &om)
{
  om = this->Omega;
}

void
LinearNoiseApproximation::getConservedCycles(std::vector<GiNaC::ex> &cLaw)
{
  // permute species vector
  std::vector<GiNaC::ex> SpecPermutated(this->species.size(),0);
  for (size_t i=0; i<this->species.size(); i++)
  {
    for (size_t j=0; j<this->species.size(); j++)
    {
      SpecPermutated[i]+=this->PermutationM(i,j)*this->species[j];
    }
  }

  // number of conservation laws
  size_t dimKernel = this->conservation_matrix.rows();

  if(dimKernel)
  {
    //report conservation laws
    std::vector<GiNaC::ex> cand(dimKernel);

    for (size_t i=0; i<dimKernel; i++)
    {
      for (size_t j=0; j<this->species.size(); j++)
      {
        cand[i]=cand[i]+this->conservation_matrix(i,j)*SpecPermutated[j];
      }
    }

    cLaw = cand;
  }

}

void
LinearNoiseApproximation::constructCovarianceMatrix(Eigen::MatrixXex &cov)
{

    // make sure it is big enough
    cov.resize(this->numIndSpecies(),this->numIndSpecies());

    // fill symmetric covariance of independent species
    size_t idx=0;
    for(size_t i=0;i<this->numIndSpecies();i++)
    {
      for(size_t j=0;j<=i;j++)
      {
        cov(i,j) = this->stateVariables[idx];
        cov(j,i) = cov(i,j); //< fill rest by symmetry
        idx++;
      }
    }

    return;

}


void
LinearNoiseApproximation::constructSymmetricMatrix(Eigen::VectorXex covVec,Eigen::MatrixXex &cov)
{

    // make sure it is big enough
    cov.resize(this->numIndSpecies(),this->numIndSpecies());

    // fill symmetric covariance of independent species
    size_t idx=0;
    for(size_t i=0;i<this->numIndSpecies();i++)
    {
      for(size_t j=0;j<=i;j++)
      {
        cov(i,j) = covVec[idx];
        cov(j,i) = cov(i,j); //< fill rest by symmetry
        idx++;
      }
    }

    return;

}

void
LinearNoiseApproximation::dump(std::ostream &str)
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
