#ifndef __INA_HISTOGRAM_HH
#define __INA_HISTOGRAM_HH

#include <map>
#include <eigen3/Eigen/Eigen>

namespace iNA {
namespace Models {

template<class T>
struct less_second
: std::binary_function<T,T,bool>
{
   inline bool operator()(const T& lhs, const T& rhs)
   {
      return lhs.second < rhs.second;
   }
};

/**
 * Implements a 1D histogram.
 *
 * @ingroup ssa
 */

template <typename T, typename U = T>
class Histogram
{

public:


    /**
    * The histogram data type.
    **/
    typedef std::map<T,U> histType;

protected:

    /**
    * Raw histogram data.
    **/
    histType histogram;

public:

    /**
    * Inserts a vector of observations.
    **/
    Histogram()
    {
        // pass...
    };


    /**
    * Inserts a vector of observations.
    **/
    void insert(const Eigen::Matrix<T, Eigen::Dynamic, 1> &observation)

    {
        for(int sid=0; sid < observation.rows(); sid++)
        {
            T val = observation(sid,0);
            typename histType::iterator it = histogram.find(val);
            if( it == histogram.end() )
                histogram.insert( std::make_pair<T,U>(val,1.) );
            else
                it->second+=1.;
        }
    }


    /**
    * Returns the normalization of the discrete pdf.
    **/
    U getNorm()

    {
        U norm = 0.;
        for(typename histType::iterator it=histogram.begin(); it!=histogram.end(); it++)
            norm += it->second;
        return norm;
    }

    /**
    * Returns the normalization of the continous pdf.
    **/
    U getContinousNorm()
    {

        U norm = 0;

        typename histType::iterator it = histogram.begin();
        typename histType::iterator last = --histogram.end();
        while(it!=last)
        {
            //typename histType::iterator previous=it; previous--;
            typename histType::iterator next=it; next++;

            norm += it->second*(next->first-it->first);
            it++;
        }

        return norm;

    }


    /**
    * Returns the raw (un-normalized) histogram data.
    **/
    histType getRawHistogram()

    {
        return histogram;
    }


    /**
    * Returns the histogram normalized as a discrete pdf.
    **/
    histType getNormalized()

    {

        U norm = getNorm();

        histType nHist(histogram);
        for(typename histType::iterator it=nHist.begin(); it != nHist.end(); it++)
            it->second/=norm;

        return nHist;

    }


    /**
    * Returns the support of the histogram into as a two-element fixed size vector.
    **/
    Eigen::Vector2d getSupport()

    {
        typename histType::iterator begin=histogram.begin();
        typename histType::iterator end=histogram.end();
        return (Eigen::Vector2d() << histogram.begin(),--histogram.end()).finished();
    }

    /**
    * Returns the histogram as normalized continous pdf.
    **/
    histType getDensity()

    {

        U norm = getContinousNorm();

        histType density(histogram);

        // Delete last element
        density.erase(--density.end());

        // Apply norm
        for(typename histType::iterator it=density.begin(); it!=density.end(); it++)
            it->second/=norm;

        return density;

    }




};

}}

#endif // HISTOGRAM_HH
