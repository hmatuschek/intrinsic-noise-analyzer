#ifndef __FLUC_MODELS_STEADYSTATEANALYSIS_HH
#define __FLUC_MODELS_STEADYSTATEANALYSIS_HH

#include "nlesolve/nlesolve.hh"
#include "linearnoiseapproximation.hh"
#include "REmodel.hh"
#include "math.hh"

namespace Fluc {
namespace Models {

template <typename M>
class SteadyStateAnalysis
{
    /**
     * Holds a reference to a SSE model.
     */

    M &sseModel;

    NLEsolve::NewtonRaphson<M> solver;

public:

    /**
    * Constructor
    */
    SteadyStateAnalysis(M &model)
        : sseModel(model), solver(model)

    {

    }

    /**
    * Constructor
    */
    SteadyStateAnalysis(M &model,const size_t &iter,const double &epsilon)
        : sseModel(model), solver(model)

    {
      this->setPrecision(epsilon);
      this->setMaxIterations(iter);
    }


    /**
    * Set the precision of the root finding algorithm
    */
    void setPrecision(const double &epsilon)

    {
        this->solver.parameters.epsilon = epsilon;
    }

    /**
    * Set the maximum number of iterations used by the root finding algorithm
    */
    void setMaxIterations(const size_t &maxiter)

    {
          this->solver.parameters.maxIterations = maxiter;
    }

    /**
     * Solves rate equations for steady state concentrations in @c conc and returns the number
     * of function evalutions.
     */
    int calcConcentrations(Eigen::VectorXd &conc)

    {

        // solve it
        switch(this->solver.solve(conc))
        {
            case NLEsolve::Success:
              break;
            case NLEsolve::Waiting:
              throw InternalError("Solver still waiting. This should not have happened."); break;
            case NLEsolve::MaxIterationsReached:
                throw NumericError("Maximum iterations reached. iNA is unable to find the roots of the system. You may try again with bigger number of iterations."); break;
            case NLEsolve::RoundOffProblem:
                throw NumericError("iNA has encountered an round-off problem for which line searched failed."); break;
            case NLEsolve::LineSearchFailed:
                throw NumericError("Line search failed. iNA is unable to find the roots of the system."); break;
            default:
                throw InternalError("Solver returned non-standard error. Status not resolved."); break;
        }

        // test for negative concentrations
        if((conc.array()<0).any())
        {
            throw NumericError("iNA encountered negative concentrations. The system may not have a positive root.");
        }

        // ... and test for stability
        if ((solver.getJacobianM().eigenvalues().real().array()>=0.).array().any())
        {
            throw NumericError("iNA has found a steady state which is unstable.");
        }

        // return number of iterations
        return solver.getIterations();

    }


    /**
     * Solves for steady state of the reduced state vector and returns number of function evaluations
     * used.
     *
     * @param x: Outputs the steady state concentrations, covariance and EMRE vector in reduced
     *        coordinates. Contents of x will be overwritten.
     */
    void calcLNA(Eigen::VectorXd &x, Eigen::VectorXex &sseUpdate)

    {
        size_t offset = sseModel.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;

        Eigen::VectorXd A(lnaLength);
        Eigen::MatrixXd B(lnaLength,lnaLength);

        // calc coeff-matrices
        GiNaC::exmap subs_table;
        for (size_t i=0; i<lnaLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( sseModel.getSSEvar(i), 0 ) );
        for(size_t i=0; i<lnaLength; i++)
        {
            A(i) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).subs(subs_table) ).to_double();
            for(size_t j=0; j<lnaLength; j++)
            {
               B(i,j) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).diff(sseModel.getSSEvar(j)) ).to_double();
            }
        }

        x.segment(offset,lnaLength) = B.lu().solve(-A);

        // substitute LNA
        subs_table.clear();
        for (size_t i=0; i<lnaLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( sseModel.getSSEvar(i), x(offset+i) ) );
        for(int i=0; i<sseUpdate.size(); i++)
            sseUpdate(i)=sseUpdate(i).subs(subs_table);

    }

    /**
     * Solves for steady state of the reduced state vector and returns number of function evaluations
     * used.
     *
     * @param x: Outputs the steady state concentrations, covariance and EMRE vector in reduced
     *        coordinates. Contents of x will be overwritten.
     */
    int calcSteadyState(Eigen::VectorXd &x)

    {

        // initialize with initial concentrations
        Eigen::VectorXd conc(sseModel.getDimension());
        sseModel.getInitialState(x);
        conc=x.head(sseModel.numIndSpecies());


        size_t offset = sseModel.numIndSpecies();
        size_t lnaLength = offset*(offset+1)/2;
        size_t sseLength = sseModel.getUpdateVector().size()-sseModel.numIndSpecies();

        Eigen::VectorXex sseUpdate = sseModel.getUpdateVector().segment(offset,sseLength);

        // substitute RE concentrations
        int iter = this->calcConcentrations(conc);
        GiNaC::exmap subs_table;
        for (size_t s=0; s<sseModel.numIndSpecies(); s++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( sseModel.getREvar(s), conc(s) ) );
        for (size_t i=0; i<sseLength; i++)
            sseUpdate(i) = sseUpdate(i).subs(subs_table);

        // calc LNA
        calcLNA(x,sseUpdate);

        // calc coeff-matrices
        Eigen::VectorXd A(sseLength-lnaLength);
        Eigen::MatrixXd B(sseLength-lnaLength,sseLength-lnaLength);
        subs_table.clear();
        for (size_t i=lnaLength; i<sseLength; i++)
            subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( sseModel.getSSEvar(i), 0 ) );
        for(size_t i=lnaLength; i<sseLength; i++)
        {
            A(i-lnaLength) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).subs(subs_table) ).to_double();
            for(size_t j=lnaLength; j<sseLength; j++)
            {
               B(i-lnaLength,j-lnaLength) = GiNaC::ex_to<GiNaC::numeric>( sseUpdate(i).diff(sseModel.getSSEvar(j)) ).to_double();
            }
        }

        x.head(offset) = conc;
        x.tail(sseLength-lnaLength) = B.lu().solve(-A);

        return iter;
    }

    int
    calcSteadyStateOld(Eigen::VectorXd &x)

    {



        // initialize with initial concentrations
        Eigen::VectorXd conc(sseModel.getDimension());
        sseModel.getInitialState(x);
        conc=x.head(sseModel.numIndSpecies());

        int iter = this->calcConcentrations(conc);

        // construct Diffusion vector
        Eigen::VectorXd DiffusionVec(sseModel.numIndSpecies()*sseModel.numIndSpecies());
        sseModel.getDiffusionVec(DiffusionVec);

        // solve for covariances
        Eigen::VectorXd sol(sseModel.numIndSpecies()*sseModel.numIndSpecies());
        sol = (-KroneckerSum(solver.getJacobianM(),solver.getJacobianM())).lu().solve(DiffusionVec);

        // should be check here if this is a good solution (requires a new epsilon!!!)
        //bool a_solution_exists = (JMsum*sol).isApprox(-DiffusionVec, 1.E-10);

        Eigen::VectorXd covVec((sseModel.numIndSpecies()*(sseModel.numIndSpecies()+1))/2);
        Eigen::MatrixXd cov(sseModel.numIndSpecies(),sseModel.numIndSpecies());

        // take only important elements into reduced state vector
        size_t idx=0;
        for(size_t i=0;i<sseModel.numIndSpecies();i++)
        {
          for(size_t j=0;j<=i;j++)
          {

            size_t idy=i+j*sseModel.numIndSpecies();
            covVec(idx)=sol(idy);
            cov(i,j) =sol(idy);//=covarianceMatrix(i,j);
            idx++;
            cov(j,i)=cov(i,j);

          }
        }
        // covariance finished

        // calculate EMRE

        Eigen::VectorXd Delta(sseModel.numIndSpecies());

        // get Rate corrections
        Eigen::VectorXd REcorr;
        sseModel.getRateCorrections(REcorr);

        // get stacked up Hessian
        Eigen::MatrixXd HessianM;
        sseModel.getHessian(HessianM);

        for (size_t i=0; i<sseModel.numIndSpecies(); i++)
        {
          Delta(i)=0.;
          idx=0;
          for (size_t j=0; j<sseModel.numIndSpecies(); j++)
          {
            for (size_t k=0; k<=j; k++)
            {

                if(k==j)
                {
                    Delta(i) += HessianM(i,idx)*cov(j,k);
                }
                else
                {
                    Delta(i) += 2.*HessianM(i,idx)*cov(j,k);
                }
                idx++;

            }
          }
          Delta(i)/=2.;
          // add RE corrections
          Delta(i)+=REcorr(i);
        }

        // solve EMRE
        Eigen::VectorXd emre(sseModel.numIndSpecies());
        emre=solver.getJacobianM().lu().solve(-Delta);

        // update reduced state vector
        // for concentrations
        // and covariances
        // and emre

        x.resize(sseModel.getDimension());
        x << conc, covVec, emre;

        return iter;

    }

};

/**
 * @ingroup models
 *
 * Performs steady state statistics on a LinearNoiseApproximation model.
 *
 * The NLEsolve::NewtonRaphson solvers is used for the deterministic REs.
 */
class SteadyStateAnalysisOld
{

protected:
  /**
   * Holds a reference to a LNA instance.
   */
   LinearNoiseApproximation &lnaModel;

   /**
    * Holds an instance of NLESolver::NewtonRaphson solver.
    */
   NLEsolve::NewtonRaphson<LinearNoiseApproximation> solver;

public:

   /**
   * Constructor
   */
  SteadyStateAnalysisOld(LinearNoiseApproximation &model)
  : lnaModel(model), solver(model)
  {

  }

  /**
  * Constructor
  */
  SteadyStateAnalysisOld(LinearNoiseApproximation &model,const size_t &iter,const double &epsilon)
  : lnaModel(model), solver(model)
  {
    this->setPrecision(epsilon);
    this->setMaxIterations(iter);
  }

  /**
  * Set the precision of the root finding algorithm
  */
  void setPrecision(const double &epsilon)
  {
      this->solver.parameters.epsilon = epsilon;
  }


  /**
  * Set the maximum number of iterations used by the root finding algorithm
  */
  void setMaxIterations(const size_t &maxiter)
  {
        this->solver.parameters.maxIterations = maxiter;
  }


  /**
   * Solves rate equations for steady state concentrations in @c conc and returns the number
   * of function evalutions.
   */
  int calcConcentrations(Eigen::VectorXd &conc)
  {

      // initialize with concentrations in permutated basis
      conc = (lnaModel.ICsPermuted).head(lnaModel.numIndSpecies());

      // solve it
      switch(this->solver.solve(conc))
      {
          case NLEsolve::Success:
            break;
          case NLEsolve::Waiting:
            throw InternalError("Solver still waiting. This should not have happened."); break;
          case NLEsolve::MaxIterationsReached:
              throw NumericError("Maximum iterations reached. iNA is unable to find the roots of the system. You may try again with bigger number of iterations."); break;
          case NLEsolve::RoundOffProblem:
              throw NumericError("iNA has encountered an round-off problem for which line searched failed."); break;
          case NLEsolve::LineSearchFailed:
              throw NumericError("Line search failed. iNA is unable to find the roots of the system."); break;
          default:
              throw InternalError("Solver returned non-standard error. Status not resolved."); break;
      }

      // test for negative concentrations
      if((conc.array()<0).any())
      {
          throw NumericError("iNA encountered negative concentrations. The system may not have a positive root.");
      }

      // ... and test for stability
      if ((solver.getJacobianM().eigenvalues().real().array()>=0.).array().any())
      {
          throw NumericError("iNA has found a steady state which is unstable.");
      }

      // return number of iterations
      return solver.getIterations();

  }


  /**
   * Get the LNA steady state spectrum.
   *
   * @param freq Specifies the vector of frequencies to evaluate the spectrum on. If @c automatic
   *        is true, the vector will be ignored and the frequencies are selected automatically. On
   *        exit the vector contains the selected frequencies.
   * @param spectrum On exit, contains the spectrum for each species.
   * @param automatic Specifies if the frequencies of the spectrum are selected automatically.
   */
  void calcSpectrum(Eigen::VectorXd &freq, Eigen::MatrixXd &spectrum, bool automatic=true)
  {

      // make sure there is enough space
      spectrum.resize(freq.size(),lnaModel.numSpecies());

      Eigen::MatrixXd DiffusionM(lnaModel.numIndSpecies(),lnaModel.numIndSpecies());
      Eigen::VectorXd conc(lnaModel.numIndSpecies());

      // construct Diffusion matrix
      lnaModel.getDiffusionMatrix(DiffusionM);

      // adjust plot range automatically
      if(automatic)
         this->getFrequencyRange(freq);

      // Matrix of inverse propagator full state -> reduced state
      Eigen::MatrixXcd FMatrix(lnaModel.numSpecies(), lnaModel.numIndSpecies());

      for(int i=0;i<freq.size();i++)
      {
         std::complex<double> iomega(0.,freq(i)*(2.*constants::pi));
         FMatrix.noalias()=( lnaModel.PermutationM*lnaModel.LinkCMatrixNumeric ).cast< std::complex<double> >()*
                 ( ( solver.getJacobianM().cast< std::complex<double> >() )
                  - (iomega*Eigen::MatrixXcd::Identity(lnaModel.numIndSpecies(),lnaModel.numIndSpecies())) ).inverse();

         Eigen::VectorXcd spec(lnaModel.numSpecies());
         spec = (FMatrix*DiffusionM*FMatrix.adjoint()).diagonal();

         // take diagonal elements
         Eigen::VectorXd specReal(lnaModel.numSpecies());
         for(int j=0; j<spec.size(); j++)
         {
             specReal(j) = std::real(spec(j));
         }
         spectrum.row(i)=specReal;
      }

      // done

  }


  /**
   * Solves for steady state of the reduced state vector and returns number of function evaluations
   * used.
   *
   * @param x: Outputs the steady state concentrations, covariance and EMRE vector in reduced
   *        coordinates. Contents of x will be overwritten.
   */
  int calcSteadyState(Eigen::VectorXd &x)
  {


      // calculate concentrations
      Eigen::VectorXd conc(lnaModel.numIndSpecies());
      int iter=this->calcConcentrations(conc);

      // construct Diffusion vector
      Eigen::VectorXd DiffusionVec(lnaModel.numIndSpecies()*lnaModel.numIndSpecies());
      lnaModel.getDiffusionVec(DiffusionVec);

      // solve for covariances
      Eigen::VectorXd sol(lnaModel.numIndSpecies()*lnaModel.numIndSpecies());
      sol = (-KroneckerSum(solver.getJacobianM(),solver.getJacobianM())).lu().solve(DiffusionVec);

      // should be check here if this is a good solution (requires a new epsilon!!!)
      //bool a_solution_exists = (JMsum*sol).isApprox(-DiffusionVec, 1.E-10);

      Eigen::VectorXd covVec((lnaModel.numIndSpecies()*(lnaModel.numIndSpecies()+1))/2);
      Eigen::MatrixXd cov(lnaModel.numIndSpecies(),lnaModel.numIndSpecies());

      // take only important elements into reduced state vector
      size_t idx=0;
      for(size_t i=0;i<lnaModel.numIndSpecies();i++)
      {
        for(size_t j=0;j<=i;j++)
        {

          size_t idy=i+j*lnaModel.numIndSpecies();
          covVec(idx)=sol(idy);
          cov(i,j) =sol(idy);//=covarianceMatrix(i,j);
          idx++;
          cov(j,i)=cov(i,j);

        }
      }
      // covariance finished

      // calculate EMRE

      Eigen::VectorXd Delta(lnaModel.numIndSpecies());

      // get Rate corrections
      Eigen::VectorXd REcorr;
      lnaModel.getRateCorrections(REcorr);

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
                  Delta(i) += 2.*HessianM(i,idx)*cov(j,k);
              }
              idx++;

          }
        }
        Delta(i)/=2.;
        // add RE corrections
        Delta(i)+=REcorr(i);
      }

      // solve EMRE
      Eigen::VectorXd emre(lnaModel.numIndSpecies());
      emre=solver.getJacobianM().lu().solve(-Delta);

      // update reduced state vector
      // for concentrations
      // and covariances
      // and emre

      x.resize(lnaModel.getDimension());
      x << conc, covVec, emre, Eigen::VectorXd::Zero(covVec.size()*(lnaModel.numIndSpecies()+2)/3),Eigen::VectorXd::Zero(covVec.size()),Eigen::VectorXd::Zero(lnaModel.numIndSpecies());

      return iter;

  }


  /**
   * Solves for IOS steady state of the reduced state vector and returns number of function evaluations
   * used.
   *
   * @param x: Outputs the steady state concentrations, covariance and EMRE vector, the IOS covariance corrections and the IOS emre corrections in reduced
   *        coordinates. Contents of x will be overwritten.
   */

  int calcIOS(Eigen::VectorXd &x)
  {

      size_t dimCov = lnaModel.numIndSpecies()*(lnaModel.numIndSpecies()+1)/2;
      size_t dimThree = dimCov*(lnaModel.numIndSpecies()+2)/3;

      int iter = this->calcSteadyState(x);

      size_t offset = 2*lnaModel.numIndSpecies()+dimCov;
      size_t iosLength = lnaModel.numIndSpecies()+dimCov+dimThree;

      Eigen::VectorXex iosUpdate = lnaModel.updateVector.segment(offset,iosLength);

      Eigen::VectorXd A(iosLength);
      Eigen::MatrixXd B(iosLength,iosLength);

      // substitution table for lna
      GiNaC::exmap subs_table;
      for (size_t s=0; s<(lnaModel.numIndSpecies()); s++)
          subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( lnaModel.species[lnaModel.PermutationVec(s)], x(s) ) );
      for (size_t s=0; s<(dimCov+lnaModel.numIndSpecies()); s++)
          subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( lnaModel.stateVariables[s], x(lnaModel.numIndSpecies()+s) ) );
      // substitute lna result
      for(int i=0; i<iosUpdate.size(); i++)
          iosUpdate(i)=iosUpdate(i).subs(subs_table);

      // generate zero
      subs_table.clear();
      for (int i=0; i<iosUpdate.size(); i++)
          subs_table.insert( std::pair<GiNaC::ex,GiNaC::ex>( lnaModel.stateVariables[lnaModel.numIndSpecies()+dimCov+i], 0 ) );
      // obtain result
      for(size_t i=0; i<iosLength; i++)
      {
          A(i)=GiNaC::ex_to<GiNaC::numeric>( iosUpdate(i).subs(subs_table) ).to_double();
          for(size_t j=0; j<iosLength; j++)
              B(i,j) = GiNaC::ex_to<GiNaC::numeric>( iosUpdate(i).diff(lnaModel.stateVariables[lnaModel.numIndSpecies()+dimCov+j]) ).to_double();
      }

      // solve IOS
      x.tail(iosLength)=B.lu().solve(-A);

      return iter;
  }

  /**
   * Samples the frequencies [0, f_max], where f_max is dertermined by the eigen values of the
   * Jacobian.
   */
  void getFrequencyRange(Eigen::VectorXd &freq)
  {

      // value for which spectrum fallen off to 5 percent
      double domega = getMaxFrequency(0.05)/freq.size();
      for(int i=0;i<freq.size();i++)
          freq(i)=i*domega;

  }


  /**
   * Get maximum frequency from Jacobian estimation.
   */
  double getMaxFrequency(const double &pr)
  {

      Eigen::VectorXcd eig = this->solver.getJacobianM().eigenvalues();
      std::cerr<<"Eigenvalues of the Jacobian..."<<std::endl;
      std::cerr<<eig<<std::endl<<std::endl;

      double imagMax = eig.imag().maxCoeff();
      double realMax = std::abs(eig.real().maxCoeff());
      double p = pr;

      p /= 1.+ imagMax*imagMax/realMax/realMax;

      // value for which spectrum fallen off to pr percent
      return ( imagMax + realMax * std::sqrt(1./p-1) )/(2.*constants::pi);

  }


  /**
   * Dump all the nice values of the steady state analysis.
   */
  void dump(std::ostream &str)
  {

      std::vector<GiNaC::ex> cLaw;
      lnaModel.getConservedCycles(cLaw);

      Eigen::VectorXd cLawValue;
      lnaModel.getConservedCycles(cLawValue);

      str << "Conserved cycles: "<<std::endl;

      for (size_t i=0; i<cLaw.size(); i++)
      {
        //str << cLaw[i] << "="<< cLawValue[i]<< std::endl;
      }
      str << std::endl << std::endl;

      size_t dimx = lnaModel.getDimension();

      Eigen::VectorXd conc(lnaModel.numSpecies());
      Eigen::VectorXd emre(lnaModel.numSpecies());
      Eigen::MatrixXd cov(lnaModel.numSpecies(), lnaModel.numSpecies());

      Eigen::VectorXd x(dimx);
      int iter = 500;

      iter=this->calcSteadyState(x);

      Eigen::VectorXd omega(10);

      Eigen::MatrixXd spectrum(omega.size(),lnaModel.numSpecies());

          this->calcSpectrum(omega,spectrum);

          lnaModel.fullState(x,conc,cov,emre);

          str << "steady state calculated in " << iter << " iterations:" << std::endl;
          for (size_t i=0; i<lnaModel.species.size(); i++)
          {
            str << lnaModel.species[i] << "\t";
          }
          str << std::endl;
          for (size_t i=0; i<lnaModel.species.size(); i++)
          {
            str << conc(i) << "\t";
          }
          str << std::endl << "Covariance:" << std::endl;

          for (size_t i=0; i<lnaModel.species.size(); i++)
          {
            str << cov(i,i) << "\t";
          }
          str << std::endl << "EMRE:" << std::endl;
          for (size_t i=0; i<lnaModel.species.size(); i++)
          {
            str << emre(i) << "\t";
          }
          str << std::endl << std::endl;

          str << std::endl << "Spectrum:" << std::endl;
          for (int i=0; i<omega.size(); i++)
          {
              str << omega(i) <<"\t"<< spectrum(i,0) << "\t";
          }
          str << std::endl << std::endl;

  }

};


/**
 * Provides a functor of the REs and the transposed Jacobian for HybridNonLinearSolver provided by
 * Eigen.
 *
 * All expressions are evaluated using Ginac.
 *
 * @deprecated We do not use the solver from Eigen, so this class can be removed.
 *
 * @ingroup models
 */
class REevaluator
{
protected:
  /**
   * Holds a reference to an instance of @c LinearNoiseApproximation.
   */
   LinearNoiseApproximation &lnaModel;

public:
   /**
    * Constructor
    */
    REevaluator(LinearNoiseApproximation &model)
        : lnaModel(model)
    {}

    /**
     * Evaluates the REs
     */
    int operator()(const Eigen::VectorXd &x, Eigen::VectorXd &fvec)
    {
        lnaModel.getREs(x,fvec);
        return 0;
    }

    /**
     * Evaluates the transposed Jacobian
     */
    int df(const Eigen::VectorXd &x, Eigen::MatrixXd &fjac)
    {
        lnaModel.getJacobianMatrix(x,fjac);
        fjac.transposeInPlace();
        return 0;
    }
};

}} // close namespaces

#endif // __FLUC_MODELS_STEADYSTATEANALYSIS_HH
