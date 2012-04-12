#ifndef __FLUC_SPECTRALANALYSIS_HH
#define __FLUC_SPECTRALANALYSIS_HH

#include "models/steadystateanalysis.hh"

namespace Fluc {
namespace Models {

/**
 *
 * Performs a spectral analysis based on a LinearNoiseApproximation model.
 *
 *  @ingroup unsupported
 *
 * The NLEsolve::NewtonRaphson solvers is used for the deterministic REs.
 */
class SpectralAnalysisBase :
        public SteadyStateAnalysis
{

public:
    SpectralAnalysisBase(LinearNoiseApproximation &model);
    Eigen::MatrixXcd getPropagatorMatrix(const double &freq);
    void calcSpectrum(const Eigen::VectorXd &freq, Eigen::VectorXd &spectrum, size_t first, size_t second);
    Eigen::VectorXd getSpectrum(const Eigen::VectorXd &freq, size_t first, size_t second);
    void dump(std::ostream &str);

private:

    /* the state */
    Eigen::VectorXd state;

    /* Some dummy vars to be compiled later */
    std::vector<GiNaC::symbol> pVars;

    /* the covariance matrix */
    Eigen::MatrixXd cov;

    Eigen::MatrixXex M5,connector;
    Eigen::MatrixXex loopSum;

    GiNaC::ex tadpoleSum;

    /* Some matrices containing only dummies */
    Eigen::MatrixXex FMatrix;
    Eigen::MatrixXex FMatrixAdj;
    Eigen::MatrixXex spectralMatrix;
    Eigen::MatrixXex covDummy;

    /* evaluate an expression to a complex value by using a substitution table */
    std::complex<double> eval(GiNaC::ex expression, const GiNaC::exmap &subs_table);

    void generateDiagrams(size_t first, size_t second);

    GiNaC::ex generateTadpoles(const Eigen::MatrixXd &cov, const Eigen::VectorXd &emre, size_t first, size_t second);

    void generateSubstitutionTable(GiNaC::exmap &state_substituations);

};

class SpectralAnalysis :
        public SpectralAnalysisBase

{

public:
    SpectralAnalysis(LinearNoiseApproximation &model);

    /* get the spectrum of a species */

    Eigen::VectorXd spectrum(const Eigen::VectorXd &freq,size_t specId);

    /* get all spectra */
    void spectrum(const Eigen::VectorXd &freq, Eigen::MatrixXd &spectrum);

};

}
}

#endif // ADVANCEDSTEADYSTATEANALYSIS_HH
