#include "LNAmodel.hh"
#include "ode/ode.hh"

using namespace Fluc;
using namespace Fluc::Models;

LNAmodel::LNAmodel(libsbml::Model *model)
  : REmodel(model)
{
  postConstructor();
}


LNAmodel::LNAmodel(const Ast::Model &model)
  : REmodel(model)
{
  postConstructor();
}


void
LNAmodel::postConstructor()
{

    dimCOV = this->numIndSpecies()*(this->numIndSpecies()+1)/2;

    // add dimension of covariances
    dim+= dimCOV;
    // add dimension of EMRE
    dim+= this->numIndSpecies();
    // reserve some space
    updateVector.conservativeResize(dim);

    // assign a set of new symbols
    // ... and add them to index table
    this->stateVariables.reserve(dimCOV+numIndSpecies());
    for(size_t i = 0; i<stateVariables.capacity(); i++)
    {
        stateVariables.push_back( GiNaC::symbol() );
        this->stateIndex.insert(std::make_pair(this->stateVariables[i],this->numIndSpecies()+i));
    }

    //form expressions with new symbols for remaining state variables

    Eigen::VectorXex covVariables(dimCOV);
    Eigen::VectorXex emreVariables(this->numIndSpecies());

    size_t idx = 0;
    for(size_t i = 0 ; i<dimCOV; i++)
        covVariables(i) = stateVariables[idx++];
    for(size_t i = 0 ; i<this->numIndSpecies(); i++)
        emreVariables(i) = stateVariables[idx++];

    /////////////////////////////////////////////
    // construct update vector for covariances //
    /////////////////////////////////////////////

    // construct a covariance matrix

    Eigen::MatrixXex cov;
    constructCovarianceMatrix(cov);

    // determine update for covariances
    // take only lower triangular and stack up to vector
    Eigen::VectorXex CovUpdate(dimCOV);
    flattenSymmetricMatrix( (this->JacobianM*cov)+(cov*this->JacobianM.transpose())+this->DiffusionMatrix ,CovUpdate);

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
        for (size_t k=0; k<j; k++)
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

    // and combine to update vector
    this->updateVector.segment(this->numIndSpecies(),dimCOV) = CovUpdate;
    this->updateVector.tail(this->numIndSpecies()) = EMREUpdate;

}

void
LNAmodel::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations, Eigen::MatrixXd &cov)

{

    // reconstruct full concentration vector in original permutation order
    REmodel::fullState(state,concentrations);

    // ... then begin reconstruction of covariance matrix

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
LNAmodel::fullState(const Eigen::VectorXd &state, Eigen::VectorXd &concentrations,
                                     Eigen::MatrixXd &cov, Eigen::VectorXd &emre)

{

    // reconstruct full concentration vector and covariances in original permutation order
    this->fullState(state,concentrations,cov);

    // reconstruct emre
    // get reduced emre vector (should better be a view rather then a copy)
    Eigen::VectorXd tail = state.segment(this->numIndSpecies()+dimCOV,this->numIndSpecies());

    // construct full emre vector, restore original order and return
    emre = this->PermutationM.transpose()*this->LinkCMatrixNumeric*tail;

}

void
LNAmodel::getInitialState(Eigen::VectorXd &x)
{

  // deterministic initial conditions for state
  x<<(this->ICsPermuted).head(this->numIndSpecies()),
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
LNAmodel::constructSymmetricMatrix(const Eigen::VectorXex &covVec,Eigen::MatrixXex &cov)
{

    // make space
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

}


//void
//LNAmodel::getRateCorrections(Eigen::VectorXd &REcorr)
//{
//    // make sure vector is big enough
//    REcorr.resize(this->numIndSpecies());
//    // and evaluate:
//    for (size_t i=0; i<this->numIndSpecies(); i++)
//        REcorr(i) = this->interpreter.evaluate(this->REcorrections(i));
//    // ... done.
//}



void
LNAmodel::flattenSymmetricMatrix(const Eigen::MatrixXex &mat,Eigen::VectorXex &vec)
{
    size_t idx=0;
    for(size_t i=0;i<this->numIndSpecies();i++)
    {
      for(size_t j=0;j<=i;j++)
      {
        vec(idx)=mat(i,j);
        idx++;
      }
    }
}

//void
//LNAmodel::getDiffusionVec(Eigen::VectorXd &DiffusionVec)
//{

//    // make sure Diffusion vector is big enough
//    DiffusionVec.resize(this->numIndSpecies()*this->numIndSpecies());

//    // and evaluate:
//    for (size_t i=0; i<(this->numIndSpecies()*this->numIndSpecies()); i++)
//    {
//        DiffusionVec(i) = this->interpreter.evaluate(this->DiffusionVec(i));
//    }

//    // ... done.

//}

//void
//LNAmodel::getDiffusionVec(const Eigen::VectorXd &state, Eigen::VectorXd &DiffusionVec)
//{
//    // set state
//    this->setState(state);
//    // and get Diffusion vector
//    this->getDiffusionVec(DiffusionVec);
//}


//void
//LNAmodel::getDiffusionMatrix(Eigen::MatrixXd &DiffusionM)
//{

//    // make sure Diffusion vector is big enough
//    DiffusionM.resize(this->numIndSpecies(),this->numIndSpecies());

//    // just lower triangular matrix
//    Eigen::MatrixXd lowerTriangD(this->numIndSpecies(),this->numIndSpecies());

//    // and evaluate this:
//    for (size_t i=0; i<this->numIndSpecies(); i++)
//    {
//        for(size_t j=0;j<=i;j++)
//        {
//            lowerTriangD(i,j) = this->interpreter.evaluate(this->DiffusionMatrix(i,j));
//        }
//    }

//    // conversion to dense matrix
//    DiffusionM = lowerTriangD.selfadjointView<Eigen::Lower>();

//    // ... done.

//}

//void
//LNAmodel::getDiffusionMatrix(const Eigen::VectorXd &state, Eigen::MatrixXd &DiffusionM)
//{
//    // set state
//    this->setState(state);
//    // and get Diffusion matrix
//    this->getDiffusionMatrix(DiffusionM);
//}


//void
//LNAmodel::getHessian(Eigen::MatrixXd &HessianM)
//{

//    // make sure the matrix is big enough
//    HessianM.resize(this->numIndSpecies(),this->numIndSpecies()*(this->numIndSpecies()+1)/2);

//    // and evaluate this:
//    for (size_t i=0; i<this->numIndSpecies(); i++)
//    {
//        int idx=0;
//        for (size_t j=0; j<this->numIndSpecies(); j++)
//        {
//            for(size_t k=0;k<=j;k++)
//            {
//                HessianM(i,idx) = this->interpreter.evaluate(this->Hessian(i,idx));
//                idx++;
//            }
//        }
//    }

//    // ... done.

//}

//void
//LNAmodel::getHessian(const Eigen::VectorXd &state, Eigen::MatrixXd &HessianM)
//{
//    // set state
//    this->setState(state);
//    // and get the Hessian in matrix matrix form
//    this->getHessian(HessianM);
//}

