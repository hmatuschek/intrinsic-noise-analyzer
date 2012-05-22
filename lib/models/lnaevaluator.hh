#ifndef __FLUC_MODELS_LNAEVALUATOR_HH
#define __FLUC_MODELS_LNAEVALUATOR_HH

#include "linearnoiseapproximation.hh"

namespace Fluc {
namespace Models {

/**
 * Wraps an instance of LinearNoiseApproximation and gives a basic interface for the ODE integration via Ginac.
 *
 * @deprecated This class is only here for test purposes USE Fluc::LNAinterpreter INSTEAD.
 *
 * @todo Remove this class, it is not needed anymore.
 *
 * @ingroup models
 */
class LNAevaluator
{
protected:
  /**
   * Holds a reference to an instance of LinearNoiseApproximation.
   */
   LinearNoiseApproximation &lnaModel;

public:
  /**
   * Constructor.
   *
   * @param model Specifies the LNA model to integrate.
   */
  LNAevaluator(LinearNoiseApproximation &model)
      : lnaModel(model)
  {

  }

  /**
   * Evaluates the joint ODE of the system size expansion.
   */
  inline void evaluate(const Eigen::VectorXd &state, double t, Eigen::VectorXd &dx)
  {

        int dimCOV = (lnaModel.numIndSpecies()*(lnaModel.numIndSpecies()+1))/2;

        // first split state vector

        // ... in concentrations,
        Eigen::VectorXd conc(lnaModel.numIndSpecies());
        conc=state.head(lnaModel.numIndSpecies());
        // ... covariances,
        Eigen::VectorXd tail(dimCOV);
        tail=state.segment(lnaModel.numIndSpecies(),dimCOV);
        // ... and EMRE
        Eigen::VectorXd emre(lnaModel.numIndSpecies());
        emre=state.segment(lnaModel.numIndSpecies()+dimCOV,lnaModel.numIndSpecies());

        Eigen::MatrixXd cov(lnaModel.numIndSpecies(),lnaModel.numIndSpecies());
        Eigen::MatrixXd cov_update(lnaModel.numIndSpecies(),lnaModel.numIndSpecies());


        ////////////////////////////
        // get old covariance matrix
        ////////////////////////////

        // fill symmetric covariance of independent species
        size_t idx=0;
        for(size_t i=0;i<lnaModel.numIndSpecies();i++)
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

        Eigen::VectorXd REs(lnaModel.numIndSpecies());
        Eigen::VectorXd REcorr(lnaModel.numIndSpecies());
        Eigen::MatrixXd JacobianM(lnaModel.numIndSpecies(),lnaModel.numIndSpecies());
        Eigen::MatrixXd DiffusionM(lnaModel.numIndSpecies(),lnaModel.numIndSpecies());

        // construct REs & set interpreter state
        lnaModel.getREs(conc,REs);

        // construct RE corrections
        lnaModel.getRateCorrections(REcorr);

        // construct Jacobian matrix
        lnaModel.getJacobianMatrix(JacobianM);

        // construct Diffusion matrix
        lnaModel.getDiffusionMatrix(DiffusionM);


        ////////////////////////////////////
        // compute update for covariances
        ////////////////////////////////////

        // update for covariances
        cov_update = (JacobianM*cov)+(cov*JacobianM.transpose())+DiffusionM;

        // take only lower triangular and stack up to vector
        idx=0;
        for(size_t i=0;i<lnaModel.numIndSpecies();i++)
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

        Eigen::VectorXd Delta(lnaModel.numIndSpecies());

        // get stacked up Hessian
        Eigen::MatrixXd HessianM;
        lnaModel.getHessian(HessianM);

        for (size_t i=0; i<lnaModel.numIndSpecies(); i++)
        {
          Delta(i)=0.;
          idx=0;
          for (size_t j=0; j<lnaModel.numIndSpecies(); j++)
          {
            for (size_t k=0; k<=j; k++)
            {
                if(k==j)
                {
                    Delta(i) += HessianM(i,idx)*cov(j,k);
                }
                else
                {
                    // fac 2 by symmetry, saves summing over strictly upper cov_full matrix
                    Delta(i) += 2.*HessianM(i,idx)*cov(j,k);
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

        Eigen::VectorXd emre_update(lnaModel.numIndSpecies());
        emre_update = ((JacobianM*emre)+Delta);

        dx << REs,tail,emre_update;

        // done.

  };


  /**
   * Constructs the "full" state from the internal, reduced state.
   */
  inline void full_state( const Eigen::VectorXd &state, Eigen::VectorXd &concentrations,
                   Eigen::MatrixXd &covariance, Eigen::VectorXd &emre)
  {
    this->lnaModel.fullState(state,concentrations,covariance,emre);
  }

  /**
   * Evaluates the initial state.
   */
  inline void getInitialState(Eigen::VectorXd &state)
  {
    this->lnaModel.getInitialState(state);
  }

  /**
   * Returns the dimension of the system.
   */
  inline size_t getDimension()
  {
    return this->lnaModel.getDimension();
  }

};

}
}

#endif // __FLUC_MODELS_LNAEVALUATOR_HH
