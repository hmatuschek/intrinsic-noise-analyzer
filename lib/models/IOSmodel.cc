#include "IOSmodel.hh"

using namespace Fluc;
using namespace Fluc::Models;

IOSmodel::IOSmodel(libsbml::Model *model)
  : LNAmodel(model)
{
  postConstructor();
}


IOSmodel::IOSmodel(const Ast::Model &model)
  : LNAmodel(model)
{
  postConstructor();
}


void
IOSmodel::postConstructor()
{
    dim3M = (this->numIndSpecies()*(this->numIndSpecies()+1)*(this->numIndSpecies()+2))/6;

    size_t dimold = dim;

    // add dimension of third moment
    dim += dim3M;
    // add dimension of covariances
    dim += dimCOV;
    // add dimension of EMRE
    dim += this->numIndSpecies();

    // reserve some space
    updateVector.conservativeResize(dim);
    stateVariables.reserve(dim-this->numIndSpecies());

    // assign a set of new symbols
    // ... and add them to index table
    for(size_t i = dimold; i<this->dim; i++)
    {
        stateVariables.push_back( GiNaC::symbol() );
        this->stateIndex.insert(std::make_pair(this->stateVariables[i-this->numIndSpecies()],i));
    }

    // form expressions with new symbols for remaining state variables
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
        thirdmomentVariables[i] = Eigen::MatrixXex::Zero(this->numIndSpecies(),this->numIndSpecies());
        for(size_t j=0;j<=i;j++)
            for(size_t k=0;k<=j;k++)
            {
                thirdmomentVariables[i](j,k)=stateVariables[idx];
                thirdmomentVariables[i](k,j)=stateVariables[idx];

                thirdmomentVariables[j](i,k)=stateVariables[idx];
                thirdmomentVariables[j](k,i)=stateVariables[idx];

                thirdmomentVariables[k](i,j)=stateVariables[idx];
                thirdmomentVariables[k](j,i)=stateVariables[idx];

                idx++; // counts i,j,k loop
            }
    }
    for(size_t i = 0 ; i<dimCOV; i++)
        iosVariables(i) = stateVariables[idx++];
    for(size_t i = 0 ; i<this->numIndSpecies(); i++)
        iosemreVariables(i) = stateVariables[idx++];

    // construct a covariance matrix

    Eigen::MatrixXex cov;
    constructCovarianceMatrix(cov);


    /////////////////////////
    // calculate third moment
    /////////////////////////

    std::vector< Eigen::MatrixXex > diffjac(this->numIndSpecies());
    for(size_t i=0;i<this->numIndSpecies();i++)
        constructSymmetricMatrix(this->DiffusionJacM.col(i),diffjac[i]);

    Eigen::VectorXex ThirdMomentUpdate(dim3M);

    idx=0;
    size_t idy;
    for(size_t i=0;i<this->numIndSpecies();i++)
        for(size_t j=0;j<=i;j++)
        {
            for(size_t k=0;k<=j;k++)
            {
                ThirdMomentUpdate(idx)=this->Diffusion3Tensor(idx);

                ThirdMomentUpdate(idx)+=this->DiffusionMatrix(j,k)*emreVariables(i)+this->DiffusionMatrix(i,k)*emreVariables(j)+this->DiffusionMatrix(i,j)*emreVariables(k);

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

                        idy++; // counts the r,s loop
                    } //end s loop
                } // end r loop

                idx++; // counts i,j,k
            } // end k loop
            } // end i,j loop

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

                iosUpdate(idx) += diffjac[k](i,j)*emreVariables(k);//this->DiffusionJacM(idx,k)*emreVariables(k);
                iosUpdate(idx) += this->JacobianM(i,k)*iosCov(k,j)+this->JacobianM(j,k)*iosCov(k,i);

                for(size_t l=0; l<k; l++)
                {
                    iosUpdate(idx) += this->Hessian(i,idy)*thirdmomentVariables[j](k,l)+this->Hessian(j,idy)*thirdmomentVariables[i](k,l);
                    iosUpdate(idx) += this->DiffusionHessianM(idx,idy)*cov(k,l);
                    idy++; //counts l,k loop
                }

                // same for l=k appears only once in sum
                iosUpdate(idx) += (this->Hessian(i,idy)*thirdmomentVariables[j](k,k)/2+this->Hessian(j,idy)*thirdmomentVariables[i](k,k)/2);
                iosUpdate(idx) += this->DiffusionHessianM(idx,idy)*cov(k,k)/2;
                idy++;

             }

        idx++; // counts i,j loop
     }

    ////////////////////////////
    // now calculate IOS-EMRE //
    ////////////////////////////

    Eigen::VectorXex Delta(this->numIndSpecies());

    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
      Delta(i)=0.;
      idx=0;
      for (size_t j=0; j<this->numIndSpecies(); j++)
      {
        for (size_t k=0; k<j; k++)
        {
            // factor 2 is used to sum over strictly upper cov matrix
            Delta(i) += this->Hessian(i,idx)*iosVariables(idx);
            idx++; // counts j,k loop
        }
        Delta(i) += this->Hessian(i,idx)*iosVariables(idx)/2.;
        idx++;
      }
    }

    // @todo add philippian!

    Eigen::VectorXex EMREiosUpdate = ((this->JacobianM*iosemreVariables)+Delta);

    // and attach to update vector
    this->updateVector.segment(dimold, dim3M) = ThirdMomentUpdate;
    this->updateVector.segment(dimold+dim3M, dimCOV) = iosUpdate;
    this->updateVector.tail(this->numIndSpecies()) = EMREiosUpdate;

}


void
IOSmodel::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations,
                                     Eigen::MatrixXd &cov, Eigen::VectorXd &emre, Eigen::MatrixXd &iosCov, Eigen::VectorXd &skewness, Eigen::VectorXd &iosemre)

{

    // reconstruct full concentration vector and covariances in original permutation order
    LNAmodel::fullState(state,concentrations,cov,emre);

    // reconstruct thirdmoments
    // get reduced skewness vector (should better be a view rather then a copy)
    Eigen::VectorXd tail = state.segment(2*this->numIndSpecies()+dimCOV,dim3M);

    Eigen::VectorXd emreVal = state.segment(this->numIndSpecies()+dimCOV,this->numIndSpecies());

    std::vector< Eigen::MatrixXd > thirdMomVariables(this->numIndSpecies());

    double val = 0;

    size_t idx = 0;
    for(size_t i=0;i<this->numIndSpecies();i++)
    {
        thirdMomVariables[i].resize(this->numIndSpecies(),this->numIndSpecies());
        for(size_t j=0;j<=i;j++)
            for(size_t k=0;k<=j;k++)
            {
                val = tail[idx]-emreVal(i)*emreVal(j)*emreVal(k);

                thirdMomVariables[i](j,k)=val;
                thirdMomVariables[i](k,j)=val;

                thirdMomVariables[j](i,k)=val;
                thirdMomVariables[j](k,i)=val;

                thirdMomVariables[k](i,j)=val;
                thirdMomVariables[k](j,i)=val;

                idx++; // counts i,j,k loop
            }
    }

    Eigen::MatrixXd cmat = this->PermutationM.transpose()*this->LinkCMatrixNumeric;

    // construct full third moment vector, restore original order and return
    for(size_t i=0; i<(unsigned)cmat.rows(); i++)
    {
        skewness(i)=0.;
        for(size_t j=0; j<(unsigned)cmat.cols(); j++)
            for(size_t k=0; k<(unsigned)cmat.cols(); k++)
                for(size_t l=0; l<(unsigned)cmat.cols(); l++)
                    skewness(i) += cmat(i,j) * cmat(i,k) * cmat(i,l) *thirdMomVariables[j](k,l);

        skewness(i)/=cov(i,i)*sqrt(cov(i,i));
    }

   // get reduced covariance vector
   Eigen::VectorXd covvec = state.segment(2*this->numIndSpecies()+dimCOV+dim3M,dimCOV);
   // reduced covariance
   Eigen::MatrixXd cov_ind(this->numIndSpecies(),this->numIndSpecies());

   // fill upper triangular
   idx=0;
   for(size_t i=0;i<this->numIndSpecies();i++)
   {
       for(size_t j=0;j<=i;j++)
       {
           cov_ind(i,j) = covvec(idx)-emreVal(i)*emreVal(j);
           // fill rest by symmetry
           cov_ind(j,i) = cov_ind(i,j);
           idx++;
       }
   }

   // restore full covariance in native permutation
   iosCov = cmat*cov_ind*cmat.transpose();

   tail = state.segment(2*this->numIndSpecies()+2*dimCOV+dim3M,this->numIndSpecies());

   // construct full iosemre vector, restore original order and return
   iosemre = cmat*tail;

}

void
IOSmodel::getInitialState(Eigen::VectorXd &x)
{

  // deterministic initial conditions for state
  x<<(this->ICsPermuted).head(this->numIndSpecies()),
     // zero covariance
     Eigen::VectorXd::Zero(dimCOV),
     // zero EMRE
     Eigen::VectorXd::Zero(this->numIndSpecies()),
     // zero third moment
     Eigen::VectorXd::Zero(dim3M),
     // zero second moment correction
     Eigen::VectorXd::Zero(dimCOV),
     // zero IOS-EMRE
     Eigen::VectorXd::Zero(this->numIndSpecies());

}
