#ifndef __FLUC_NLESOLVE_MINPACK_HH
#define __FLUC_NLESOLVE_MINPACK_HH

#include <iostream>
#include <string>

namespace Fluc {

namespace NLEsolve {

/**
 * MinPack++ Solver to find roots of nonlinear algebraic equations.
 *
 * Original FORTRAN77 version by Jorge More, Burt Garbow, Ken Hillstrom.
 * The C++ version by John Burkardt has been obtained from
 * http://people.sc.fsu.edu/~jburkardt/cpp_src/minpack/minpack.html
 *
 * @ingroup nlesolve
 */
class MinPack

{
public:

    MinPack();

    /**
    //       fcn is a user-supplied subroutine which
    //         calculates the functions and should be written as follows.
    //
    //         subroutine fcn(n,x,fvec,iflag)
    //         integer n,iflag
    //         double precision x(n),fvec(n)
    //         ----------
    //         calculate the functions at x and
    //         return this vector in fvec.
    //         ---------
    //         return
    //         end
    //
    //         the value of iflag should not be changed by fcn unless
    //         the user wants to terminate execution of hybrd1.
    //         in this case set iflag to a negative integer.
    //
    //@param n is a positive integer input variable set to the number
    //         of functions and variables.
    //
    //@param x is an array of length n. on input x must contain
    //         an initial estimate of the solution vector. on output x
    //         contains the final estimate of the solution vector.
    //
    //@param fvec is an output array of length n which contains
    //         the functions evaluated at the output x.
    **/

    virtual void fcn ( int n, double x[], double fvec[], int *iflag )=0;

protected:


    /**
    *  HYBRD finds a zero of a system of N nonlinear equations.
    *
    *  Discussion:
    *
    *    The purpose of hybrd is to find a zero of a system of
    *    n nonlinear functions in n variables by a modification
    *    of the powell hybrid method. The jacobian is
    *    then calculated by a forward-difference approximation.
    **/

    int hybrd ( int n, double x[], double fvec[], double xtol, int maxfev, int ml,
      int mu, double epsfcn, double diag[], int mode, double factor, int nprint,
      int nfev, double fjac[], int ldfjac, double r[], int lr, double qtf[],
      double wa1[], double wa2[], double wa3[], double wa4[] );


    /**
    //    HYBRD1 is a simplified interface to HYBRD.
    //
    //  Discussion:
    //
    //    The purpose of HYBRD1 is to find a zero of a system of
    //    N nonlinear functions in N variables by a modification
    //    of the Powell hybrid method.  This is done by using the
    //    more general nonlinear equation solver HYBRD.
    //    The jacobian is then calculated by a forward-difference
    //    approximation.
    //
    //  Parameters:
    //
    //@param n is a positive integer input variable set to the number
    //         of functions and variables.
    //
    //@param x is an array of length n. on input x must contain
    //         an initial estimate of the solution vector. on output x
    //         contains the final estimate of the solution vector.
    //
    //@param fvec is an output array of length n which contains
    //         the functions evaluated at the output x.
    //
    //@param tol is a nonnegative input variable. termination occurs
    //         when the algorithm estimates that the relative error
    //         between x and the solution is at most tol.
    //
    //@param info is an integer output variable. if the user has
    //         terminated execution, info is set to the (negative)
    //         value of iflag. see description of fcn. otherwise,
    //         info is set as follows.
    //
    //         info = 0   improper input parameters.
    //
    //         info = 1   algorithm estimates that the relative error
    //                    between x and the solution is at most tol.
    //
    //         info = 2   number of calls to fcn has reached or exceeded
    //                    200*(n+1).
    //
    //         info = 3   tol is too small. no further improvement in
    //                    the approximate solution x is possible.
    //
    //         info = 4   iteration is not making good progress.
    //
    //@param wa is a work array of length lwa.
    //
    //@param lwa is a positive integer input variable not less than
    //         (n*(3*n+13))/2.
    **/

    int hybrd1 ( int n,
      double x[], double fvec[], double tol, double wa[], int lwa );

private:

void chkder ( int m, int n, double x[], double fvec[], double fjac[], 
  int ldfjac, double xp[], double fvecp[], int mode, double err[] );
void dogleg ( int n, double r[], int lr, double diag[], double qtb[],
  double delta, double x[], double wa1[], double wa2[] );
double enorm ( int n, double x[] );
void fdjac1 ( int n, double x[], double fvec[], double fjac[], int ldfjac, int *iflag,
  int ml, int mu, double epsfcn, double wa1[], double wa2[] );
//void fdjac2 ( void fcn ( int m, int n, double x[], double fvec[], int *iflag ),
//  int m, int n, double x[], double fvec[], double fjac[], int ldfjac,
//  int *iflag, double epsfcn, double wa[] );
int i4_max ( int i1, int i2 );
int i4_min ( int i1, int i2 );
void qform ( int m, int n, double q[], int ldq, double wa[] );
void qrfac ( int m, int n, double a[], int lda, bool pivot, int ipvt[],
  int lipvt, double rdiag[], double acnorm[], double wa[] );
void r1mpyq ( int m, int n, double a[], int lda, double v[], double w[] );
bool r1updt ( int m, int n, double s[], int ls, double u[], double v[], double w[] );
double r8_abs ( double x );
double r8_epsilon ( );
double r8_huge ( );
double r8_max ( double x, double y );
double r8_min ( double x, double y );
double r8_tiny ( );
double r8_uniform_01 ( int *seed );

};

}}

#endif
