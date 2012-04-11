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
    for(size_t i=0;i<stateVariables.size();i++)
        stateVariables[i] = GiNaC::symbol();

    // and add them to index table
    for(size_t i = 0 ; i<this->stateVariables.size(); i++)
    {
        this->stateIndex.insert(std::make_pair(this->stateVariables[i],this->numIndSpecies()+i));
    }

    // form expressions with new symbols for remaining state variables
    size_t dimCOV = this->numIndSpecies()*(this->numIndSpecies()+1)/2;

    Eigen::VectorXex covVariables(dimCOV);
    Eigen::VectorXex emreVariables(this->numIndSpecies());

    for(size_t i = 0 ; i<dimCOV; i++)
        covVariables(i) = stateVariables[i];
    for(size_t i = 0 ; i<this->numIndSpecies(); i++)
        emreVariables(i) = stateVariables[dimCOV+i];

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

    ///////////////////////////////////////////
    // construct update vector for covariances
    ///////////////////////////////////////////

    // construct a covariance matrix

    Eigen::MatrixXex cov;
    constructCovarianceMatrix(cov);

    // determine update for covariances
    Eigen::MatrixXex cov_update;
    cov_update = (this->JacobianM*cov)+(cov*this->JacobianM.transpose())+this->DiffusionMatrix;

    // take only lower triangular and stack up to vector
    Eigen::VectorXex CovUpdate(dimCOV);

    size_t idx=0;
    for(size_t i=0;i<this->numIndSpecies();i++)
    {
      for(size_t j=0;j<=i;j++)
      {
        CovUpdate(idx)=cov_update(i,j);
        idx++;
      }
    }

    // done with CovUpdate

    ///////////////////////
    // now calculate EMRE
    ///////////////////////

    Eigen::VectorXex Delta(this->numIndSpecies());

    // get full covariance in permuted base
    Eigen::MatrixXex cov_full(this->numSpecies(),this->numSpecies());
    cov_full=this->LinkCMatrix*cov*(this->LinkCMatrix).transpose();

    // restore native permutation of covariance
    cov_full=this->PermutationM.transpose().cast<GiNaC::ex>()*cov_full*this->PermutationM.cast<GiNaC::ex>();

    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
      Delta(i)=0.;
      idx=0;
      for (size_t j=0; j<this->numSpecies(); j++)
      {
        for (size_t k=0; k<=j; k++)
        {
            if(k==j)
            {
                Delta(i) += this->Hessian(i,idx)*cov_full(j,k);
            }
            else
            {
                // fac 2 by symmetry, saves summing over strictly upper cov_full matrix
                Delta(i) += 2.*this->Hessian(i,idx)*cov_full(j,k);
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

    // and combine to update vector
    this->updateVector << this->REs,CovUpdate,EMREUpdate;

};



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
    HessianM.resize(this->numIndSpecies(),this->numSpecies()*(this->numSpecies()+1)/2);

    // and evaluate this:
    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
        int idx=0;
        for (size_t j=0; j<this->numSpecies(); j++)
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
LinearNoiseApproximation::getfHessianM(Eigen::MatrixXd &fHessianM)
{

    // make sure the matrix is big enough
    fHessianM.resize(this->numIndSpecies(),this->numIndSpecies());

    // and evaluate this:
    for (size_t i=0; i<this->numIndSpecies(); i++)
    {
        for (size_t j=0; j<this->numIndSpecies(); j++)
        {
           fHessianM(i,j) = this->interpreter.evaluate(this->fHessian(i,j));
        }
    }

    // ... done.

}

void
LinearNoiseApproximation::getfHessianM(const Eigen::VectorXd &state, Eigen::MatrixXd &fHessianM)
{
    // set state
    this->setState(state);
    // and get the Hessian of objective function in matrix matrix form
    this->getfHessianM(fHessianM);
}

void
LinearNoiseApproximation::getHessian(const Eigen::VectorXd &state, Eigen::MatrixXd &HessianM)
{
    // set state
    this->setState(state);
    // and get the Hessian in matrix matrix form
    this->getHessian(HessianM);
}

void
LinearNoiseApproximation::evaluate(const Eigen::VectorXd &state, double t, Eigen::VectorXd &dx)
{

  int dimCOV = (this->numIndSpecies()*(this->numIndSpecies()+1))/2;

  // first split state vector

  // ... in concentrations,
  Eigen::VectorXd conc(this->numIndSpecies());
  conc=state.head(this->numIndSpecies());
  // ... covariances,
  Eigen::VectorXd tail(dimCOV);
  tail=state.segment(this->numIndSpecies(),dimCOV);
  // ... and EMRE
  Eigen::VectorXd emre(this->numIndSpecies());
  emre=state.segment(this->numIndSpecies()+dimCOV,this->numIndSpecies());

  Eigen::MatrixXd cov(this->numIndSpecies(),this->numIndSpecies());
  Eigen::MatrixXd cov_update(this->numIndSpecies(),this->numIndSpecies());


  ////////////////////////////
  // get old covariance matrix
  ////////////////////////////

  // fill symmetric covariance of independent species
  size_t idx=0;
  for(size_t i=0;i<this->numIndSpecies();i++)
  {
    for(size_t j=0;j<=i;j++)
    {
      cov(i,j)=tail(idx);
      cov(j,i)=cov(i,j); //< fill rest by symmetry
      idx++;
    }
  }


  ////////////////////////
  // get all coefficients
  ////////////////////////

  Eigen::VectorXd REs(this->numIndSpecies());
  Eigen::VectorXd REcorr(this->numIndSpecies());
  Eigen::MatrixXd JacobianM(this->numIndSpecies(),this->numIndSpecies());
  Eigen::MatrixXd DiffusionM(this->numIndSpecies(),this->numIndSpecies());

  // construct REs & set interpreter state
  this->getREs(conc,REs);

  // construct RE corrections
  this->getRateCorrections(REcorr);

  // construct Jacobian matrix
  this->getJacobianMatrix(JacobianM);

  // construct Diffusion matrix
  this->getDiffusionMatrix(DiffusionM);


  ////////////////////////////////////
  // compute update for covariances
  ////////////////////////////////////

  // update for covariances
  cov_update = (JacobianM*cov)+(cov*JacobianM.transpose())+DiffusionM;

  // take only lower triangular and stack up to vector
  idx=0;
  for(size_t i=0;i<this->numIndSpecies();i++)
  {
    for(size_t j=0;j<=i;j++)
    {
      tail(idx)=cov_update(i,j);
      idx++;
    }
  }

  ///////////////////////
  // now calculate EMRE
  ///////////////////////

  Eigen::VectorXd Delta(this->numIndSpecies());

  // get full covariance in permuted base
  Eigen::MatrixXd cov_full(this->numSpecies(),this->numSpecies());
  cov_full=this->LinkCMatrixNumeric*cov*(this->LinkCMatrixNumeric).transpose();

  // restore native permutation of covariance
  cov_full = (this->PermutationM.transpose()*cov_full)*this->PermutationM;

  // get stacked up Hessian
  Eigen::MatrixXd HessianM;
  this->getHessian(HessianM);

  for (size_t i=0; i<this->numIndSpecies(); i++)
  {
    Delta(i)=0.;
    idx=0;
    for (size_t j=0; j<this->numSpecies(); j++)
    {
      for (size_t k=0; k<=j; k++)
      {
          if(k==j)
          {
              Delta(i) += HessianM(i,idx)*cov_full(j,k);
          }
          else
          {
              // fac 2 by symmetry, saves summing over strictly upper cov_full matrix
              Delta(i) += 2.*HessianM(i,idx)*cov_full(j,k);
          }

          idx++;

      }
    }
    // divide by 2
    Delta(i)/=2.;
    // now add rate corrections
    Delta(i)+=REcorr(i);
  }


  /////////////////////////
  // update state vector
  /////////////////////////

  Eigen::VectorXd emre_update(this->numIndSpecies());
  emre_update = ((JacobianM*emre)+Delta);

  dx << REs,tail,emre_update;

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

  return dim;
}


void
LinearNoiseApproximation::getInitialState(Eigen::VectorXd &x)
{
  int dimCov=(this->numIndSpecies()*(this->numIndSpecies()+1))/2;

  // deterministic initial conditions for state
  x<<(this->ICsPermuted).head(this->numIndSpecies()),
     // zero covariance
     Eigen::VectorXd::Zero(dimCov),
     // zero EMRE
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

    cLaw=cand;
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
