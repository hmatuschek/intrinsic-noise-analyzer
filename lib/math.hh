/**
 * @defgroup math Mathematical Utilities
 *
 * This group collects some mathematical functions and constants.
 */

#ifndef __FLUC_MATH_HH__
#define __FLUC_MATH_HH__

#include "erf.hh"
#include "incompletegamma.hh"
#include "kroneckerproduct.hh"


namespace Fluc {

/**
 * Important constants.
 *
 * @ingroup math
 */
namespace constants {

/**
 * The second most important constant.
 */
const double pi = 4*std::atan(1);

/**
 * The most important constant.
 */
const double e  = std::exp(1);

/**
 * The least important constant.
 */
const double AVOGADRO  = 6.0221412927e23;

}


namespace Math {

/** Tests if the given value is true value or some special value like NaN, inf, etc.
 * @ingroup math */
template <typename T>
inline bool isValue(const T &value)
{
  return value == value && // if value != NaN
      (! std::numeric_limits<T>::has_infinity ||   // if value type has no inf:
       ( value != std::numeric_limits<T>::infinity() &&  // if value != inf
         value != -std::numeric_limits<T>::infinity())); // if value != -inf
}


/** Tests if the given value is true value or some special value like NaN, inf, etc.
 * @ingroup math */
template <>
inline bool isValue< std::complex<double> >(const std::complex<double> &value)
{
  return isValue<double>(value.real()) && isValue<double>(value.imag());
}


/** Tests if the given value is true value or some special value like NaN, inf, etc.
 * @ingroup math */
template <typename T>
inline bool isNotValue(const T &value)
{
  return ! isValue<T>(value);
}

}
}

#endif // MATH_HH
