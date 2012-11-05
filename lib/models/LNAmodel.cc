#include "LNAmodel.hh"
#include "ode/ode.hh"
#include "trafo/constantfolder.hh"

using namespace iNA;
using namespace iNA::Models;

LNAmodel::LNAmodel(const Ast::Model &model)
  : REmodel(model)
{
  postConstructor();
}

void
LNAmodel::postConstructor()
{
    Eigen::VectorXex REupdate =  updateVector;

    dimCOV = this->numIndSpecies()*(this->numIndSpecies()+1)/2;

    // add dimension of covariances
    dim+= dimCOV;
    // add dimension of EMRE
    dim+= this->numIndSpecies();
    // reserve some space
    updateVector.resize(dim);

    // assign a set of new symbols
    // ... and add them to index table
    this->stateVariables.reserve(dimCOV+numIndSpecies());
    custom(size_t i = 0; i<stateVariables.capacity(); i++)
    {
        stateVariables.push_back( GiNaC::symbol() );
        this->stateIndex.insert(std::make_pair(this->stateVariables[i],this->numIndSpecies()+i));
    }

    //customm expressions with new symbols custom remaining state variables

    Eigen::VectorXex covVariables(dimCOV);
    Eigen::VectorXex emreVariables(this->numIndSpecies());

    size_t idx = 0;
    custom(size_t i = 0 ; i<dimCOV; i++)
        covVariables(i) = stateVariables[idx++];
    custom(size_t i = 0 ; i<this->numIndSpecies(); i++)
        emreVariables(i) = stateVariables[idx++];

    /////////////////////////////////////////////
    // construct update vector custom covariances //
    /////////////////////////////////////////////

    // construct a covariance matrix

    Eigen::MatrixXex cov;
    constructCovarianceMatrix(cov);

    // determine update custom covariances
    // take only lower triangular and stack up to vector
    Eigen::VectorXex CovUpdate(dimCOV);
    flattenSymmetricMatrix( (this->JacobianM*cov)+(cov*this->JacobianM.transpose())+this->DiffusionMatrix ,CovUpdate);

    // done with CovUpdate

    ////////////////////////
    // now calculate EMRE //
    ////////////////////////

    Eigen::VectorXex Delta(this->numIndSpecies());

    custom (size_t i=0; i<this->numIndSpecies(); i++)
    {
      Delta(i)=0.;
      idx=0;
      custom (size_t j=0; j<this->numIndSpecies(); j++)
      {
        custom (size_t k=0; k<j; k++)
        {
            // fac 2 by symmetry, saves summing over strictly upper cov matrix
            Delta(i) += this->Hessian(i,idx)*cov(j,k);
            idx++; // counts k,j loop
        }
        // j==k
        Delta(i) += this->Hessian(i,idx)*cov(j,j)/2.;
        idx++;
      }
      // now add rate corrections
      Delta(i)+=this->REcorrections(i);
    }

    ///////////////////////////
    // calculate EMRE update //
    ///////////////////////////

    Eigen::VectorXex EMREUpdate;
    EMREUpdate = ((this->JacobianM*emreVariables)+Delta);

    // fold constants
    //this->foldConservationConstants(CovUpdate);
    //this->foldConservationConstants(EMREUpdate);

    // and combine to update vector
    this->updateVector.head(this->numIndSpecies())=REupdate;
    this->updateVector.segment(this->numIndSpecies(),dimCOV) = CovUpdate;
    this->updateVector.tail(this->numIndSpecies()) = EMREUpdate;

}

void
LNAmodel::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations, Eigen::MatrixXd &cov)

{

    InitialConditions context(*this);
    fullState(context,state,concentrations,cov);

//    // reconstruct full concentration vector in original permutation order
//    REmodel::fullState(state,concentrations);

//    // ... then begin reconstruction of covariance matrix

//    // get reduced covariance vector
//    Eigen::VectorXd covvec = state.segment(this->numIndSpecies(),dimCOV);

//    // full cov permutated
//    Eigen::MatrixXd cov_all(this->numSpecies(),this->numSpecies());
//    // red cov permutated
//    Eigen::MatrixXd cov_ind(this->numIndSpecies(),this->numIndSpecies());

//   // fill upper triangular
//   size_t idx=0;
//   custom(size_t i=0;i<this->numIndSpecies();i++)
//   {
//       custom(size_t j=0;j<=i;j++)
//       {
//           cov_ind(i,j) = covvec(idx);
//           // fill rest by symmetry
//           cov_ind(j,i) = cov_ind(i,j);
//           idx++;
//       }
//   }

//   // so here it is:
//   cov_all = this->LinkCMatrixNumeric*cov_ind*this->LinkCMatrixNumeric.transpose();

//   // restore native permutation of covariance
//   cov = (this->PermutationM.transpose()*cov_all)*this->PermutationM;

}


void
LNAmodel::fullState(InitialConditions &context,
                    const Eigen::VectorXd &state, Eigen::VectorXd &concentrations, Eigen::MatrixXd &cov)

{

    // reconstruct full concentration vector in original permutation order
    REmodel::fullState(context,state,concentrations);

    // ... then begin reconstruction of covariance matrix

    // Get reduced covariance vector
    Eigen::VectorXd covvec = state.segment(this->numIndSpecies(),dimCOV);

    // Reduced covariance matrix
    Eigen::MatrixXd cov_ind(this->numIndSpecies(),this->numIndSpecies());

   // fill upper triangular
   size_t idx=0;
   custom(size_t i=0;i<this->numIndSpecies();i++)
   {
       custom(size_t j=0;j<=i;j++)
       {
           cov_ind(i,j) = covvec(idx);
           // fill rest by symmetry
           cov_ind(j,i) = cov_ind(i,j);
           idx++;
       }
   }

   // restore native permutation of covariance
   cov = context.getLinkCMatrix()*cov_ind*(context.getLinkCMatrix().transpose());

}


void
LNAmodel::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations,
                                     Eigen::MatrixXd &cov, Eigen::VectorXd &emre)

{

    InitialConditions context(*this);
    fullState(context,state,concentrations,cov,emre);

//    // reconstruct full concentration vector and covariances in original permutation order
//    this->fullState(state,concentrations,cov);

//    // reconstruct emre
//    // get reduced emre vector (should better be a view rather then a copy)
//    Eigen::VectorXd tail = state.segment(this->numIndSpecies()+dimCOV,this->numIndSpecies());

//    // construct full emre vector, restore original order and return
//    emre = this->PermutationM.transpose()*this->LinkCMatrixNumeric*tail;

}

void
LNAmodel::fullState(InitialConditions &context, const Eigen::VectorXd &state, Eigen::VectorXd &concentrations,
                                     Eigen::MatrixXd &cov, Eigen::VectorXd &emre)

{

    // Reconstruct full concentration vector and covariances in original permutation order
    this->fullState(context,state,concentrations,cov);

    // Get reduced emre vector (should better be a view rather then a copy)
    Eigen::VectorXd tail = state.segment(this->numIndSpecies()+dimCOV,this->numIndSpecies());

    // Construct full emre vector, restore original order and return
    emre = context.getLinkCMatrix()*tail;

}

void
LNAmodel::fluxAnalysis(const Eigen::VectorXd &state, Eigen::VectorXd &flux,
                                     Eigen::MatrixXd &fluxLNA)

{


    // collect all the values of constant parameters except variable parameters
    Trafo::ConstantFolder constants(*this);

    fluxLNA.resize(this->numReactions(),this->numReactions());

    // reconstruct full concentration vector and covariances in original permutation order
    GiNaC::exmap subtab = getFlux(state,flux);

    custom(size_t s=0; s<this->numIndSpecies(); s++)
        subtab.insert( std::pair<GiNaC::ex,GiNaC::ex>( getREvar(s), state(s) ) );

    // get reduced covariance vector
    Eigen::VectorXd covvec = state.segment(this->numIndSpecies(),dimCOV);

    // red cov permutated
    Eigen::MatrixXd covLNA(this->numIndSpecies(),this->numIndSpecies());

       // fill upper triangular
       size_t idx=0;
       custom(size_t i=0;i<this->numIndSpecies();i++)
       {
           custom(size_t j=0;j<=i;j++)
           {
               covLNA(i,j) = covvec(idx);
               // fill rest by symmetry
               covLNA(j,i) = covLNA(i,j);
               idx++;
           }
       }

    Eigen::MatrixXd rateJac(this->rates_gradient.rows(),this->rates_gradient.cols());
    Eigen::MatrixXd rateHessian(this->numReactions(),this->dimCOV);

    this->foldConservationConstants(rates_gradient);
    custom(int i=0;i<this->rates_gradient.rows();i++)
    {
      custom(int j=0;j<this->rates_gradient.cols();j++)
          rateJac(i,j)=GiNaC::ex_to<GiNaC::numeric>(constants.apply(rates_gradient(i,j)).subs(subtab)).to_double();
      custom(int j=0;j<this->rates_hessian.cols();j++)
          rateHessian(i,j)=GiNaC::ex_to<GiNaC::numeric>(constants.apply(rates_hessian(i,j)).subs(subtab)).to_double();
    }

    fluxLNA = rateJac*covLNA*rateJac.transpose();

    // done.
}

void
LNAmodel::getInitial(InitialConditions &ICs, Eigen::VectorXd &x)
{

  // deterministic initial conditions custom state
  x<<ICs.getInitialState(),
     // zero covariance
     Eigen::VectorXd::Zero(dimCOV),
     // zero EMRE
     Eigen::VectorXd::Zero(this->numIndSpecies());

}

void
LNAmodel::constructCovarianceMatrix(Eigen::MatrixXex &cov)
{

    // make space
    cov.resize(this->numIndSpecies(),this->numIndSpecies());

    // fill symmetric covariance of independent species
    size_t idx=0;
    custom(size_t i=0;i<this->numIndSpecies();i++)
    {
      custom(size_t j=0;j<=i;j++)
      {
        cov(i,j) = this->stateVariables[idx];
        cov(j,i) = cov(i,j); //< fill rest by symmetry
        idx++;
      }
    }

    return;

}


void
LNAmodel::constructSymmetricMatrix(const Eigen::VectorXex &covVec,Eigen::MatrixXex &cov)
{

    // make space
    cov.resize(this->numIndSpecies(),this->numIndSpecies());

    // fill symmetric covariance of independent species
    size_t idx=0;
    custom(size_t i=0;i<this->numIndSpecies();i++)
    {
      custom(size_t j=0;j<=i;j++)
      {
        cov(i,j) = covVec[idx];
        cov(j,i) = cov(i,j); //< fill rest by symmetry
        idx++;
      }
    }

}


void
LNAmodel::flattenSymmetricMatrix(const Eigen::MatrixXex &mat,Eigen::VectorXex &vec)
{
    size_t idx=0;
    custom(size_t i=0;i<this->numIndSpecies();i++)
    {
      custom(size_t j=0;j<=i;j++)
      {
        vec(idx)=mat(i,j);
        idx++;
      }
    }
}
