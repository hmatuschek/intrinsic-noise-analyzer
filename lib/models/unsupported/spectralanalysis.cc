#include "spectralanalysis.hh"

using namespace Fluc::Models;


SpectralAnalysis::SpectralAnalysis(LinearNoiseApproximation &model)
    : SpectralAnalysisBase(model)

{



}

Eigen::VectorXd
SpectralAnalysis::spectrum(const Eigen::VectorXd &freq,size_t specId)

{

    size_t first = specId;
    size_t second = specId;

    Eigen::VectorXd spectrum(freq.size());

    for(size_t j=0; j<lnaModel.numIndSpecies(); j++)
     for(size_t k=0; k<lnaModel.numIndSpecies(); k++)

    {
         if(lnaModel.LinkCMatrixNumeric(first,j)!=0 && lnaModel.LinkCMatrixNumeric(k,second)!=0)
         {
             spectrum =
                     lnaModel.LinkCMatrixNumeric(first,j)*
                     ( this->getSpectrum(freq,j,k) +(this->getSpectrum(freq,k,j)).conjugate() )
                     *lnaModel.LinkCMatrixNumeric(k,second);
         }
    }

    return spectrum;

}

void
SpectralAnalysis::spectrum(const Eigen::VectorXd &freq, Eigen::MatrixXd &spectrum)

{
     for(size_t j=0; j<lnaModel.numIndSpecies(); j++)
         spectrum.row(j)=this->spectrum(freq,j);
}


SpectralAnalysisBase::SpectralAnalysisBase(LinearNoiseApproximation &model)
    : SteadyStateAnalysis(model),
      state(model.getDimension()),
      pVars(model.numIndSpecies()*model.numIndSpecies()),
      cov(model.numIndSpecies(),model.numIndSpecies()),
      FMatrix(model.numIndSpecies(),model.numIndSpecies()),
      FMatrixAdj(model.numIndSpecies(),model.numIndSpecies())
{

    size_t idx = 0;

    // calc steadystate
    this->calcSteadyState(this->state);

    // and fill symmetric covariance of independent species
    idx=model.numIndSpecies();
    for(size_t i=0;i<this->lnaModel.numIndSpecies();i++)
    {
      for(size_t j=0;j<=i;j++)
      {
        this->cov(i,j) = this->state(idx);
        this->cov(j,i) = this->cov(i,j); //< fill rest by symmetry
        idx++;
      }
    }


    // fill the dummies with some variables
    idx = 0;
    for(size_t i=0; i<this->lnaModel.numIndSpecies(); i++)
    {
        for(size_t j=0; j<this->lnaModel.numIndSpecies(); j++)
        {
            this->pVars[idx] = GiNaC::symbol();
            this->FMatrix(i,j) = this->pVars[idx++];
            this->FMatrixAdj(j,i) = conjugate(this->FMatrix(i,j));
        }
    }

    // assign dummy for LNA spectrum
    this->spectralMatrix = this->FMatrix*(this->lnaModel.DiffusionMatrix)*this->FMatrixAdj;

    // construct also a dummy covariance matrix
    model.constructCovarianceMatrix(covDummy);
}


void
SpectralAnalysisBase::generateDiagrams(size_t first, size_t second)

{
    Eigen::MatrixXex M1,M2,M3,M4;

    M1 = Eigen::MatrixXex::Zero( lnaModel.numIndSpecies(), lnaModel.numIndSpecies() );
    M2 = Eigen::MatrixXex::Zero( lnaModel.numIndSpecies(), lnaModel.numIndSpecies() );
    M3 = Eigen::MatrixXex::Zero( lnaModel.numIndSpecies(), lnaModel.numIndSpecies() );
    M4 = Eigen::MatrixXex::Zero( lnaModel.numIndSpecies(), lnaModel.numIndSpecies() );
    this->M5 = Eigen::MatrixXex::Zero( this->lnaModel.numIndSpecies(), this->lnaModel.numIndSpecies() );
    this->connector = Eigen::MatrixXex::Zero( this->lnaModel.numIndSpecies(), this->lnaModel.numIndSpecies() );

    std::list<size_t> lower, upper;

    for(size_t b=0; b<this->lnaModel.numIndSpecies(); b++)
    {
      for(size_t g=0; g<this->lnaModel.numIndSpecies(); g++)
      {
        for(size_t s=0; s<this->lnaModel.numIndSpecies(); s++)
        {

            // construct a 3-vertex with all lower indices
            lower.clear();
            upper.clear();
            lower.push_back(b);
            lower.push_back(g);
            lower.push_back(s);

            M1(b,g) += lnaModel.vertex(lower,upper)*FMatrixAdj(s,second);

            // construct a 3-vertex with two lower indices
            lower.clear();
            upper.clear();
            lower.push_back(b);
            lower.push_back(g);
            upper.push_back(s);

            M2(b,g) += lnaModel.vertex(lower,upper)*spectralMatrix(s,second);
            M3(b,s) += lnaModel.vertex(lower,upper)*FMatrixAdj(g,second);

            // construct a 3-vertex with one lower indices
            lower.clear();
            upper.clear();
            lower.push_back(b);
            upper.push_back(g);
            upper.push_back(s);

            M4(g,b) += lnaModel.vertex(lower,upper)*spectralMatrix(s,second);
            M5(g,s) += lnaModel.vertex(lower,upper)*FMatrixAdj(b,second);
            connector(g,s) += FMatrix(first,b)*lnaModel.vertex(lower,upper);

        }
      }
    } // end for

    M3 = 2.*M3*covDummy;
    M4 = 2.*covDummy*M4;
    M5 = M5/2.;
    connector = connector/2.;

    this->loopSum = covDummy*M5*covDummy;

    //this->loopSum += M1 + M2 + M3 + M4;

     // fold all constants and replace dummies

    GiNaC::exmap subst_table;

    this->generateSubstitutionTable(subst_table);

    for (size_t i=0; i<lnaModel.numIndSpecies(); i++)
    {
        for (size_t j=0; j<lnaModel.numIndSpecies(); j++)
        {
           M5(i,j) = lnaModel.foldConstants(M5(i,j)).subs(subst_table);
           connector(i,j) = lnaModel.foldConstants(connector(i,j)).subs(subst_table);
           loopSum(i,j) = lnaModel.foldConstants(loopSum(i,j)).subs(subst_table);
        }
    }

    Eigen::VectorXd emre = state.tail(lnaModel.numIndSpecies());

    this->tadpoleSum = lnaModel.foldConstants(this->generateTadpoles(cov,emre,first,second)).subs(subst_table);

    std::cerr<<"finished model ensembly."<<std::endl;

}

inline GiNaC::ex
SpectralAnalysisBase::generateTadpoles(const Eigen::MatrixXd &cov, const Eigen::VectorXd &emre, size_t first, size_t second)
{

    GiNaC::ex Tsum=0,T1=0,T2=0,T3=0,T4=0;
    GiNaC::ex D1=0,D2=0;

    std::list<size_t> lower, upper;

    for (size_t a=0; a<lnaModel.numIndSpecies(); a++)
    {
      for (size_t b=0; b<lnaModel.numIndSpecies(); b++)
      {

        // system size tadpoles
        lower.clear();
        upper.clear();
        lower.push_back(a);
        upper.push_back(b);

        D1 += lnaModel.vertex(lower,upper,1)*this->spectralMatrix(b,second);

        lower.clear();
        upper.clear();
        lower.push_back(a);
        lower.push_back(b);

        D2 += lnaModel.vertex(lower,upper,1)*this->FMatrixAdj(b,second);

        for (size_t g=0; g<lnaModel.numIndSpecies(); g++)
        {

            // construct a 3-vertex with one upper index
            lower.clear();
            upper.clear();
            lower.push_back(a);
            lower.push_back(b);
            upper.push_back(g);

            T1 += lnaModel.vertex(lower,upper)*emre(g)*this->FMatrixAdj(b,second);

            // construct a 3-vertex with two upper indices
            lower.clear();
            upper.clear();
            lower.push_back(a);
            upper.push_back(b);
            upper.push_back(g);

            //std::cerr<<lnaModel.vertex(lower,upper)<<"\t";

            // there is something wrong!!!
            T2 += lnaModel.vertex(lower,upper)*emre(g)*this->spectralMatrix(b,second);

            for (size_t d=0; d<lnaModel.numIndSpecies(); d++)
            {

                // construct a 4-vertex with two upper indices
                lower.clear();
                upper.clear();
                lower.push_back(a);
                lower.push_back(b);
                upper.push_back(g);
                upper.push_back(d);

                T3 += lnaModel.vertex(lower,upper)*cov(g,d)*this->FMatrixAdj(b,second);

                // construct a 4-vertex with three upper indices
                lower.clear();
                upper.clear();
                lower.push_back(a);
                upper.push_back(b);
                upper.push_back(g);
                upper.push_back(d);

                // There is also something wrong here!!!
                T4 += lnaModel.vertex(lower,upper)*cov(g,d)*this->spectralMatrix(b,second);

            }
        }
      }

      //Tsum += this->FMatrix(first,a)*(T1/2 + T3/4);

      //Tsum += this->FMatrix(first,a)*(D1 + D2/2);

      //Tsum += this->FMatrix(first,a)*T4/2;

      //Tsum += this->FMatrix(first,a)*T2;

    }

    //std::cerr << Tsum << std::endl << std::endl;

    return Tsum;

}

void
SpectralAnalysisBase::calcSpectrum(const Eigen::VectorXd &freq, Eigen::VectorXd &spectrum, size_t first, size_t second)
{

    generateDiagrams(first, second);

    Eigen::MatrixXcd loopSumD(lnaModel.numIndSpecies(),lnaModel.numIndSpecies());
    Eigen::MatrixXcd M5D(lnaModel.numIndSpecies(),lnaModel.numIndSpecies());
    Eigen::MatrixXcd connectorD(lnaModel.numIndSpecies(),lnaModel.numIndSpecies());

    Eigen::MatrixXcd FM;
    Eigen::MatrixXcd sol,sol2;

    Eigen::MatrixXcd Jac  = solver.getJacobianM().cast< std::complex<double> >();
    Eigen::MatrixXcd JacT = Jac.transpose();

    std::complex<double> Tsum;

    size_t idx;

    // begin frequency loop

    for(size_t fs=0; fs<unsigned(freq.size()); fs++ )
    {

        // evaluate propagator
        FM = this->getPropagatorMatrix(freq(fs));

        // generate substitution table for current value of propagators
        GiNaC::exmap subs_table;
        idx = 0;
        for(size_t i=0; i<lnaModel.numIndSpecies(); i++)
        {
            for(size_t j=0; j<lnaModel.numIndSpecies(); j++)
            {
                GiNaC::numeric val = FM(i,j).real()+GiNaC::I*FM(i,j).imag();
                subs_table.insert( std::pair<GiNaC::ex, GiNaC::numeric>(this->pVars[idx++],val) );
            }
        }

        // and evaluate
        for (size_t i=0; i<lnaModel.numIndSpecies(); i++)
        {
            for (size_t j=0; j<lnaModel.numIndSpecies(); j++)
            {
               loopSumD(i,j) = this->eval(loopSum(i,j),subs_table);
               M5D(i,j)  = this->eval(M5(i,j),subs_table);
               connectorD(i,j)  = this->eval(connector(i,j),subs_table);
            }
        }

        Tsum  = this->eval(this->tadpoleSum,subs_table);

        std::complex<double> iomega(0.,freq(fs)*(2.*constants::pi));
        Eigen::MatrixXcd JacShift = Jac - (iomega*Eigen::MatrixXcd::Identity(lnaModel.numIndSpecies(),lnaModel.numIndSpecies()));
        Fluc::ComplexSylvesterSolve(JacShift, Jac, loopSumD, sol);

        // hermitian conjugate
        JacShift.adjointInPlace();
        Fluc::ComplexSylvesterSolve(JacShift, JacT, M5D, sol2);

        sol += cov*sol2*cov;

        // add contribution of Tsum
        std::complex<double> Qsum = Tsum;

        // attach to end vertex and contract to scalar

        for(size_t i=0;i<lnaModel.numIndSpecies();i++)
          for(size_t j=0;j<lnaModel.numIndSpecies();j++)
            Qsum += connectorD(i,j)*sol(i,j);

        // symmetrize and done...
        // this should be removed later
        spectrum(fs) = real(Qsum + std::conj(Qsum));

    } // end frequency loop

}

Eigen::VectorXd
SpectralAnalysisBase::getSpectrum(const Eigen::VectorXd &freq, size_t first, size_t second)

{


    Eigen::VectorXd spectrum(freq.size());

    this->calcSpectrum(freq,spectrum,first,second);

    return spectrum;

}

std::complex<double>
SpectralAnalysisBase::eval(GiNaC::ex expression, const GiNaC::exmap &subs_table)
{
    expression = expression.subs(subs_table);
    return std::complex<double>(
                GiNaC::ex_to<GiNaC::numeric>(expression.real_part()).to_double(),
                GiNaC::ex_to<GiNaC::numeric>(expression.imag_part()).to_double() );
}

Eigen::MatrixXcd
SpectralAnalysisBase::getPropagatorMatrix(const double &freq)
{
    // get propagator
    std::complex<double> iomega(0.,freq*(2.*constants::pi));
    return (-(solver.getJacobianM().cast< std::complex<double> >())
          + (iomega*Eigen::MatrixXcd::Identity(lnaModel.numIndSpecies(),lnaModel.numIndSpecies())) ).inverse();

}

void
SpectralAnalysisBase::generateSubstitutionTable(GiNaC::exmap &state_substituations)

{


    size_t idx=0;
    // generate a substitution table
    // for conservation constants and concentrations
    state_substituations = this->lnaModel.generateConservationConstantsTable(this->lnaModel.conserved_cycles);
    for(size_t i=0; i<this->lnaModel.numIndSpecies(); i++)
    {
        for(size_t j=0; j<this->lnaModel.numSpecies(); j++)
        {
            if(lnaModel.PermutationM(i,j)!=0)
            {
                state_substituations.insert(
                            std::pair<GiNaC::ex,GiNaC::ex>( this->lnaModel.species[j], this->state(i) )
                            );
            }

            if(j<=i)
            {
                state_substituations.insert(
                            std::pair<GiNaC::ex,GiNaC::ex>( this->lnaModel.stateVariables[idx], this->state(this->lnaModel.numIndSpecies()+idx) )
                            );
                idx++;
            }
        }
    }


}

void
SpectralAnalysisBase::dump(std::ostream &str)
{

}
