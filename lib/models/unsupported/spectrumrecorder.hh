#ifndef __FLUC_MODELS_SPECTRUMRECORDER_HH
#define __FLUC_MODELS_SPECTRUMRECORDER_HH

#include "openmp.hh"
#include <sbml/SBMLTypes.h>
#include <eigen3/unsupported/Eigen/FFT>

namespace Fluc{
namespace Models{

/**
* Container that performs and collects fft data from an ensemble.
* @ingroup unsupported
*/

class SpectrumContainer {

private:

    size_t dimension;
    size_t count;
    double timestep;

    Eigen::VectorXd freq;

    Eigen::VectorXd mean;
    //Eigen::MatrixXd spectrum;
    Eigen::MatrixXcd crossSpectrum;

    std::vector< Eigen::FFT<double> > fft;
    std::vector< Eigen::VectorXcd > transform;

    Eigen::MatrixXi indexMap;

public:

    SpectrumContainer(size_t dim, size_t windowSize, double dt, size_t numThreads=OpenMP::getMaxThreads())
        : dimension(dim), count(0), timestep(dt), freq(windowSize),
          mean(Eigen::VectorXd::Zero(dim)),
          //spectrum(Eigen::MatrixXd::Zero(dim,windowSize)),
          crossSpectrum(Eigen::MatrixXcd::Zero(dim*(dim+1)/2,windowSize)),
          fft(numThreads), transform(dim),
          indexMap(Eigen::MatrixXi::Zero(dim,dim))
    {
        for(size_t i=0;i<unsigned(freq.size());i++)
            freq(i)=i/dt/windowSize;

        // this could be somehow avoided...
        int idz=0;
        for(size_t sidx=0;sidx<dim;sidx++)
            for(size_t sidy=0;sidy<=sidx;sidy++)
                indexMap(sidx,sidy)=idz++;

    }

    /**
    * Accumulate spectral data from timeseries.
    */
    void accumulate(const std::vector<Eigen::MatrixXd> &timeseries)

    {

        //loop over all realizations
        for(size_t j=0; j<(unsigned)timeseries[0].rows(); j++)
        {

            //loop over species
            #pragma omp parallel for
            for(size_t sid=0;sid<timeseries.size();sid++)
            {

                fft[OpenMP::getThreadNum()].fwd( transform[sid], Eigen::VectorXd(timeseries[sid].row(j)) );

      //          spectrum.row(sid) += transform[sid].cwiseAbs2();
                mean(sid) += transform[sid](0).real();

            }

            #pragma omp parallel for
            for(size_t sidx=0;sidx<timeseries.size();sidx++)
                for(size_t sidy=0;sidy<=sidx;sidy++)
                    crossSpectrum.row(indexMap(sidx,sidy)) += Eigen::VectorXcd(transform[sidx].array()*transform[sidy].array().conjugate());

        }

        count += timeseries[0].rows();
    }

    Eigen::VectorXcd getCrossSpectrum(size_t idx, size_t idy)

    {
        Eigen::VectorXcd spec;
        if(idy<=idx)
                spec = crossSpectrum.row(indexMap(idx,idy));
        else
                spec = crossSpectrum.row(indexMap(idy,idx)).conjugate();

        // substract mean
        spec(0)-=mean(idx)*mean(idy)/count;
        return (spec*timestep/freq.size()/count);
    }

    Eigen::VectorXd getFrequencies()

    {
        return freq;
    }

    Eigen::VectorXd getAverage()

    {
        return mean/count*timestep/freq.size();
    }

    Eigen::VectorXd getSpectrum(size_t idx)

    {
        return getCrossSpectrum(idx, idx).real();
    }

    size_t getCount()

    {
        return count;
    }


};

/**
* Records spectra from a StochasticSimulator.
*
* Now: records all species of the simulator...soon: should record only a slice.
* @ingroup unsupported
*/

template <class T, class U>
class SpectrumRecorder {

protected:

    double timestep;
    size_t windowSize;

    /**
    * Holds an instance of the StochasticSimulator.
    */
    T Simulator;

    SpectrumContainer spectrum;

    std::vector<Eigen::MatrixXd> state;

public:

    /**
    * Constructor.
    * @param model model to use
    * @param realizations ensemble size to recorded
    * @param fMax maximum frequency
    * @param deltaf frequency resolution
    * @param numThreads number of OpenMP threads to use
    */
    SpectrumRecorder(U *model, size_t realizations, double fMax, double deltaf, size_t numThreads=OpenMP::getMaxThreads())
        : timestep(1./fMax), windowSize(ceil(fMax/deltaf-1)),
          Simulator(model,realizations,1024,2,numThreads),
          spectrum(Simulator.numSpecies(),windowSize,timestep),
          state(Simulator.numSpecies())
    {
       for(size_t i=0;i<state.size();i++)
          state[i].resize(realizations, windowSize);
    }

    /**
    * Advance the simulator by a transient time.
    */
    void advance(double transientTime)
    {
        Simulator.run(transientTime);
    }

    /**
    * Do the recording ...
    */
    void run()
    {
        Eigen::MatrixXd temp;

        //get time window
        for(size_t j=0.; j<windowSize; j++)
        {
            Simulator.run(timestep);
            Simulator.getState(temp);

            // collect all realizations of species sid
            for(size_t sid=0;sid<state.size(); sid++)
                state[sid].col(j) = temp.col(sid);
        }

        // accumulate spectrum
        spectrum.accumulate(state);

    }

    /**
    * Number of components that are recorded.
    */
    size_t getDimension()
    {
        return Simulator.numSpecies();
    }

    /**
    * Returns the spectrum of a component.
    */
    Eigen::VectorXd getSpectrum(size_t speciesId)
    {

        return spectrum.getSpectrum(speciesId);

    }

    Eigen::VectorXd getAverage()
    {

        return spectrum.getAverage();

    }

    /**
    * Returns the frequency vector of the record.
    */
    Eigen::VectorXd getFrequencies()
    {
        return spectrum.getFrequencies();
    }


};


}
}

#endif // SPECTRUMRECORDER_HH
