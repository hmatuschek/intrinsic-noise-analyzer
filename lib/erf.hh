#ifndef __FLUC_MATH_ERF_HH__
#define __FLUC_MATH_ERF_HH__

#include <cmath>

/**
 * @defgroup math Mathematical Utilities
 */


namespace Fluc
{
/**
 * Implements the error function defined as:
 *
 * \f[ erf(x) = \int_0^x{e^{-t^2}dt} \f]
 *
 * using the approximation as described in \cite abramowitz64 in formula 7.1.26 as:
 *
 * \f[ erf(x) = 1-\left(a_1t+a_2t^2+a_3t^3+a_4t^4+a_5t^5\right)e^{-x^2}+\epsilon(x)\f]
 *
 * where \f$t = \frac{1}{1+px}\f$ and \f$\left|\epsilon(x)\right| \le 1.5\cdot10^{-7}\f$ with
 * coefficients: \f$p=0.3275911\f$, \f$a_1=0.254829592\f$, \f$a_2=-0.284496736\f$,
 * \f$a_3=1.421413741\f$, \f$a_4=-1.453152027\f$ and \f$a_5=1.061405429\f$.
 *
 * @ingroup math
 */
inline double erf(double x)
{
  // Handle negative values:
  if (0 > x)
    return -erf(-x);

  double a1 =  0.254829592, \
      a2 = -0.284496736, \
      a3 =  1.421413741, \
      a4 = -1.453152027, \
      a5 =  1.061405429, \
      p  =  0.3275911;

  double t = 1./(1.+p*x);
  return 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*std::exp(-x*x);
}

}

#endif // ERF_HH
