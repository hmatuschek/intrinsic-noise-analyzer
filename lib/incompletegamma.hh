#ifndef __FLUC_INCOMPLETEGAMMA_HH__
#define __FLUC_INCOMPLETEGAMMA_HH__

#include <cmath>

namespace Fluc {


/**
 * Incomplete gamma integral
 *
 * The function is defined by
 *
 * \f[incompletegama(a,x) = \frac{1}{\Gamma(a)}\int_0^x{e^{-t}t^{a-1}dt}\f]
 *
 * In this implementation both arguments must be positive.
 * The integral is evaluated by either a power series or
 * continued fraction expansion, depending on the relative
 * values of a and x.
 *
 * ACCURACY:
 *
 *                       Relative error:
 * arithmetic   domain     # trials      peak         rms
 *    IEEE      0,30       200000       3.6e-14     2.9e-15
 *    IEEE      0,100      300000       9.9e-14     1.5e-14
 *
 * Cephes Math Library Release 2.8:  June, 2000
 * Copyright 1985, 1987, 2000 by Stephen L. Moshier
 *
 * @ingroup math
 */
double incompletegamma(double a, double x);


/**
 * Complemented incomplete gamma integral
 *
 * The function is defined by
 *
 *  igamc(a,x)   =   1 - igam(a,x)
 *
 *  \f[= \frac{1}{\Gamma(a)}\int_x^{\inf}{e^{-t}t^{a-1}dt}\f]
 *
 * In this implementation both arguments must be positive.
 * The integral is evaluated by either a power series or
 * continued fraction expansion, depending on the relative
 * values of a and x.
 *
 * Tested at random a, x.
 *                a         x                      Relative error:
 * arithmetic   domain   domain     # trials      peak         rms
 *    IEEE     0.5,100   0,100      200000       1.9e-14     1.7e-15
 *    IEEE     0.01,0.5  0,100      200000       1.4e-13     1.6e-15
 *
 * Cephes Math Library Release 2.8:  June, 2000
 * Copyright 1985, 1987, 2000 by Stephen L. Moshier
 *
 * @ingroup math
 */
double incompletegammac(double a, double x);


/**
 * Natural logarithm of gamma function
 *
 * \f[\log{(\Gamma(x))}\f]
 *
 * Result:
 *     logarithm of the absolute value of the Gamma(X).
 *
 * Output parameters:
 *    SgnGam  -   sign(Gamma(X))
 *
 * Domain:
 *     0 < X < 2.55e305
 *     -2.55e305 < X < 0, X is not an integer.
 *
 * ACCURACY:
 * arithmetic      domain        # trials     peak         rms
 *    IEEE    0, 3                 28000     5.4e-16     1.1e-16
 *    IEEE    2.718, 2.556e305     40000     3.5e-16     8.3e-17
 * The error criterion was relative when the function magnitude
 * was greater than one but absolute when it was less than one.
 *
 * The following test used the relative error criterion, though
 * at certain points the relative error could be much higher than
 * indicated.
 *    IEEE    -200, -4             10000     4.8e-16     1.3e-16
 *
 * Cephes Math Library Release 2.8:  June, 2000
 * Copyright 1984, 1987, 1989, 1992, 2000 by Stephen L. Moshier
 * Translated to AlgoPascal by Bochkanov Sergey (2005, 2006, 2007).
 *
 * @ingroup math
 */
double lngamma(double x, double* sgngam);

}
#endif // INCOMPLETEGAMMA_HH
