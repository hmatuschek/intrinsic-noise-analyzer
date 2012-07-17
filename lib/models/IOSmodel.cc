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

    GiNaC::ex temp;

    dim3M = (this->numIndSpecies()*(this->numIndSpecies()+1)*(this->numIndSpecies()+2))/6;

    size_t dimold = dim;

    // add dimension of third moment
    dim += dim3M;
    // add dimension of covariances
    dim += dimCOV;
    // add dimension of EMRE
    dim += this->numIndSpecies();

    // reserve some space

    Eigen::VectorXex LNAupdate = updateVector;
    updateVector.resize(dim);
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

                ThirdMomentUpdate(idx)+=this->REcorrections(i)*cov(j,k)+this->REcorrections(j)*cov(i,k)+this->REcorrections(k)*cov(i,j);

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

    GiNaC::ex wick1,wick2;

    for(size_t i=0;i<this->numIndSpecies();i++)
    for(size_t j=0;j<=i;j++)
    {
        iosUpdate(idx)=this->DiffusionMatrixO1(i,j);

        iosUpdate(idx)+=this->REcorrections(i)*emreVariables(j)+this->REcorrections(j)*emreVariables(i);

        idy=0;
        int idz=0;

        for(size_t k=0; k<this->numIndSpecies(); k++)
        {

            iosUpdate(idx) += JacobianMO1(i,k)*cov(k,j)+JacobianMO1(j,k)*cov(k,i);

            iosUpdate(idx) += diffjac[k](i,j)*emreVariables(k);//this->DiffusionJacM(idx,k)*emreVariables(k);
            iosUpdate(idx) += this->JacobianM(i,k)*iosCov(k,j)+this->JacobianM(j,k)*iosCov(k,i);

            for(size_t l=0; l<k; l++)
            {
                iosUpdate(idx) += this->Hessian(i,idy)*thirdmomentVariables[j](k,l)+this->Hessian(j,idy)*thirdmomentVariables[i](k,l);
                iosUpdate(idx) += this->DiffusionHessianM(idx,idy)*cov(k,l);
                idy++; //counts l,k loop

                temp=0;
                for(size_t m=0; m<l; m++)
                {

                    // use Wick's theorem to calculate 4-th moment + (i <-> j)
                    wick1 = (cov(k,l)*cov(m,j)+cov(k,m)*cov(l,j)+cov(k,j)*cov(l,m));
                    wick2 = (cov(k,l)*cov(m,i)+cov(k,m)*cov(l,i)+cov(k,i)*cov(l,m));
                    iosUpdate(i) += (this->PhilippianM(i,idz)*wick1+this->PhilippianM(j,idz)*wick2);
                    idz++;
                }
                // m=l
                wick1 = (2*cov(k,l)*cov(l,j)+cov(k,j)*cov(l,l));
                wick2 = (2*cov(k,l)*cov(l,i)+cov(k,i)*cov(l,l));
                iosUpdate(idx) += (this->PhilippianM(i,idz)*wick1+this->PhilippianM(j,idz)*wick2)/2;

                idz++;

            } // end l

            wick1 = 3*cov(k,k)*cov(k,j);
            wick2 = 3*cov(k,k)*cov(k,i);
            iosUpdate(idx) += (this->PhilippianM(i,idz)*wick1+this->PhilippianM(j,idz)*wick2)/6;
            idz++;

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
      idy=0;
      for (size_t j=0; j<this->numIndSpecies(); j++)
      {
        for (size_t k=0; k<j; k++)
        {
            // factor 2 is used to sum over strictly upper cov matrix
            Delta(i) += this->Hessian(i,idx)*iosVariables(idx);
            idx++; // counts j,k loop

            temp = 0;
            for (size_t l=0; l<k; l++)
            {
                Delta(i) += this->PhilippianM(i,idy)*thirdmomentVariables[j](k,l);
                idy++;
            }
            // k=l
            Delta(i) += this->PhilippianM(i,idy)*thirdmomentVariables[j](k,k)/2;
            idy++;
        }

        Delta(i) += this->PhilippianM(i,idy)*thirdmomentVariables[j](j,j)/6;

        idy++;

        Delta(i) += this->Hessian(i,idx)*iosVariables(idx)/2.;
        idx++;
        Delta(i) += this->JacobianMO1(i,j)*emreVariables(j);

      }


    }

    Eigen::VectorXex EMREiosUpdate = ((this->JacobianM*iosemreVariables)+Delta);

    // fold conservation constants
    this->foldConservationConstants(conserved_cycles,ThirdMomentUpdate);
    this->foldConservationConstants(conserved_cycles,iosUpdate);
    this->foldConservationConstants(conserved_cycles,EMREiosUpdate);

    // and attach to update vector
    this->updateVector.head(dimold) = LNAupdate;
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

void
IOSmodel::fluxAnalysis(const Eigen::VectorXd &state, Eigen::VectorXd &flux, Eigen::VectorXd &fluxEMRE,
                       Eigen::MatrixXd &fluxCovariance, Eigen::MatrixXd &fluxIOS)

{

    fluxEMRE.resize(this->numReactions());
    fluxIOS.resize(this->numReactions(),this->numReactions());

    // reconstruct full concentration vector and covariances in original permutation order
    GiNaC::exmap subtab = getFlux(state,flux);

    LNAmodel::fluxAnalysis(state,flux,fluxCovariance);

    // get reduced covariance vector
    Eigen::VectorXd covvec = state.segment(this->numIndSpecies(),dimCOV);
    Eigen::VectorXd emre = state.segment(this->numIndSpecies()+dimCOV,this->numIndSpecies());

    // red cov permutated
    Eigen::MatrixXd covLNA(this->numIndSpecies(),this->numIndSpecies());

       // fill upper triangular
       size_t idx=0;
       for(size_t i=0;i<this->numIndSpecies();i++)
       {
           for(size_t j=0;j<=i;j++)
           {
               covLNA(i,j) = covvec(idx);
               // fill rest by symmetry
               covLNA(j,i) = covLNA(i,j);
               idx++;
           }
       }


    // get reduced covariance vector
    covvec = state.segment(2*this->numIndSpecies()+dimCOV+dim3M,dimCOV);
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

    Eigen::MatrixXd rateJac(this->rates_gradient.rows(),this->rates_gradient.cols());
    Eigen::MatrixXd rateHessian(this->numReactions(),this->dimCOV);
    Eigen::MatrixXd rate1Jac(this->rates_gradient.rows(),this->rates_gradient.cols());

    this->foldConservationConstants(conserved_cycles,rates_gradient);
    this->foldConservationConstants(conserved_cycles,rate_corrections);

    for(int i=0;i<this->rates_gradient.rows();i++)
    {
      for(int j=0;j<this->rates_gradient.cols();j++)
      {
          rate1Jac(i,j)=GiNaC::ex_to<GiNaC::numeric>(rates_gradientO1(i,j).subs(subtab)).to_double();
          rateJac(i,j)=GiNaC::ex_to<GiNaC::numeric>(rates_gradient(i,j).subs(subtab)).to_double();
      }
      for(int j=0;j<this->rates_hessian.cols();j++)
      {
          rateHessian(i,j)=GiNaC::ex_to<GiNaC::numeric>(rates_hessian(i,j).subs(subtab)).to_double();
      }
    }


    fluxEMRE = rateJac*emre;

    for(size_t i=0;i<this->numReactions();i++)
      fluxEMRE(i) += GiNaC::ex_to<GiNaC::numeric>(rate_corrections(i).subs(subtab)).to_double();

    for(size_t j=0; j<this->numReactions(); j++)
    {
        idx=0;
        for(size_t s=0; s<this->numIndSpecies(); s++)
        {
            for(size_t t=0; t<s; t++)
            {
                fluxEMRE(j)+=rateHessian(j,idx)*covLNA(s,t);
                idx++;
            }
            // t=s
            fluxEMRE(j)+=rateHessian(j,idx)*covLNA(s,s)/2.;
            idx++;
        }
    }

    fluxIOS = rateJac*cov_ind*rateJac.transpose();

    // reconstruct thirdmoments
    // get reduced skewness vector (should better be a view rather then a copy)
    Eigen::VectorXd tail = state.segment(2*this->numIndSpecies()+dimCOV,dim3M);
    std::vector< Eigen::MatrixXd > thirdmoment(this->numIndSpecies());

    double val = 0;

    idx = 0;
    for(size_t i=0;i<this->numIndSpecies();i++)
    {
        thirdmoment[i].resize(this->numIndSpecies(),this->numIndSpecies());
        for(size_t j=0;j<=i;j++)
            for(size_t k=0;k<=j;k++)
            {
                val = tail[idx];

                thirdmoment[i](j,k)=val;
                thirdmoment[i](k,j)=val;

                thirdmoment[j](i,k)=val;
                thirdmoment[j](k,i)=val;

                thirdmoment[k](i,j)=val;
                thirdmoment[k](j,i)=val;

                idx++; // counts i,j,k loop
            }
    }


    for(size_t i=0;i<this->numReactions();i++)
        for(size_t j=0;j<this->numReactions();j++)
        {

            for(size_t s=0;s<this->numIndSpecies();s++)
                for(size_t t=0;t<this->numIndSpecies();t++)
                    fluxIOS(i,j)+= rateJac(i,s)*covLNA(s,t)*rate1Jac(j,t)+rateJac(j,s)*covLNA(s,t)*rate1Jac(i,t);

            for(size_t s=0;s<this->numIndSpecies();s++)
            {
                idx=0;
                for(size_t t=0;t<this->numIndSpecies();t++)
                {
                    for(size_t u=0;u<t;u++)
                    {
                        fluxIOS(i,j)+=thirdmoment[s](t,u)*(rateHessian(i,idx)*rateJac(j,s));//+rateHessian(j,idx)*rateJac(i,s));
                        idx++;
                    }
                    // t=u
                    fluxIOS(i,j)+=rateHessian(i,idx)*thirdmoment[s](t,t)*rateJac(j,s)/2.+rateHessian(j,idx)*thirdmoment[s](t,t)*rateJac(i,s)/2.;
                    idx++;
                }
            }




            idx=0;
            for(size_t s=0;s<this->numIndSpecies();s++)
            {
                for(size_t t=0;t<s;t++)
                {
                    int idy=0;
                    for(size_t u=0;u<this->numIndSpecies();u++)
                    {
                        for(size_t v=0;v<u;v++)
                        {
                            double wick = covLNA(s,u)*covLNA(t,v)+covLNA(s,v)*covLNA(t,u);
                            fluxIOS(i,j)+=rateHessian(i,idx)*rateHessian(j,idy)*wick;
                            idy++;
                        }
                        // u=v
                        double wick = 2.*covLNA(s,u)*covLNA(t,u);
                        fluxIOS(i,j)+=rateHessian(i,idx)*rateHessian(j,idy)*wick/2;
                    }

                    idx++;
                }

                // s=t
                int idy=0;
                for(size_t u=0;u<this->numIndSpecies();u++)
                {
                    for(size_t v=0;v<u;v++)
                    {
                        double wick = covLNA(s,s)*covLNA(u,v)+2.*covLNA(s,u)*covLNA(s,v);
                        fluxIOS(i,j)+=rateHessian(i,idx)*rateHessian(j,idy)*wick/2;
                        idy++;
                    }
                    // u=v
                    double wick = covLNA(s,s)*covLNA(u,u)+2.*covLNA(s,u)*covLNA(s,u);
                    fluxIOS(i,j)+=rateHessian(i,idx)*rateHessian(j,idy)*wick/4;
                }

                idx++;

            }


        }

    // done.
}

