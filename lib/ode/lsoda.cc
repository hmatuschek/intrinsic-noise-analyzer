#include "lsoda.hh"
#include "lsodahelper.hh"

using namespace Fluc;
using namespace Fluc::ODE;

const int    LsodaConstants::mord[3] = {0, 12, 5};
const double LsodaConstants::sm1[13] = {0., 0.5, 0.575, 0.55, 0.45, 0.35, 0.25, 0.2, 0.15, 0.1, 0.075, 0.05, 0.025};
const double LsodaConstants::ETA = 2.2204460492503131e-16;

LSODA::LSODA()
 : g_nyh(0), g_lenyh(0),
   illin(0), init(0), ntrep(0), ixpr(0), //< variables for lsoda()
   lsoda_warning(1)

{
    // some parameters
    mxstp0 = 5000; mxhnl0 = 10;
    // pass...

}


/* Terminate lsoda due to illegal input. */
void
LSODA::terminate(int *istate)
{
        if (illin == 5)
        {
                printf(
                        "[lsoda] repeated occurrence of illegal input. "
                        "run aborted.. apparent infinite loop\n");
        }
        else
        {
                illin++;
                *istate = -3;
        }
}


/* Terminate lsoda due to various error conditions. */
void
LSODA::terminate2(double *y, double *t)
{
        int             i;
        yp1 = yh[1];
        for (i = 1; i <= n; i++)
                y[i] = yp1[i];
        *t = tn;
        illin = 0;
        freevectors();
        return;

}


/*
   The following block handles all successful returns from lsoda.
   If itask != 1, y is loaded from yh and t is set accordingly.
   *Istate is set to 2, the illegal input counter is zeroed, and the
   optional outputs are loaded into the work arrays before returning.
*/



/*

The following is the original documentation. It has been adapted to Euler,
which uses interpreted functions for f etc.

The version I got, did already NOT use a jacobian routine, which is
a pitty, since Euler could provide one via Maxima. However, in this
case the routine seems to approximate the Jacobian.

Moreover, the version did not use the work arrays, since they are only
needed for the Jacobian functions, it seems.

c-----------------------------------------------------------------------
c this is the march 30, 1987 version of
c lsoda.. livermore solver for ordinary differential equations, with
c         automatic method switching for stiff and nonstiff problems.
c
c this version is in double precision.
c
c lsoda solves the initial value problem for stiff or nonstiff
c systems of first order ode-s,
c     dy/dt = f(t,y) ,  or, in component form,
c     dy(i)/dt = f(i) = f(i,t,y(1),y(2),...,y(neq)) (i = 1,...,neq).
c
c this a variant version of the lsode package.
c it switches automatically between stiff and nonstiff methods.
c this means that the user does not have to determine whether the
c problem is stiff or not, and the solver will automatically choose the
c appropriate method.  it always starts with the nonstiff method.
c
c authors..
c                linda r. petzold  and  alan c. hindmarsh,
c                computing and mathematics research division, l-316
c                lawrence livermore national laboratory
c                livermore, ca 94550.
c
c references..
c 1.  alan c. hindmarsh,  odepack, a systematized collection of ode
c     solvers, in scientific computing, r. s. stepleman et al. (eds.),
c     north-holland, amsterdam, 1983, pp. 55-64.
c 2.  linda r. petzold, automatic selection of methods for solving
c     stiff and nonstiff systems of ordinary differential equations,
c     siam j. sci. stat. comput. 4 (1983), pp. 136-148.
c-----------------------------------------------------------------------
c summary of usage.
c
c communication between the user and the lsoda package, for normal
c situations, is summarized here.  this summary describes only a subset
c of the full set of options available.  see the full description for
c details, including alternative treatment of the jacobian matrix,
c optional inputs and outputs, nonstandard options, and
c instructions for special situations.  see also the example
c problem (with program and output) following this summary.
c
c a. first provide a subroutine of the form..
c               subroutine f (neq, t, y, ydot)
c               dimension y(neq), ydot(neq)
c which supplies the vector function f by loading ydot(i) with f(i).
c
c b. write a main program which calls subroutine lsoda once for
c each point at which answers are desired.  this should also provide
c for possible use of logical unit 6 for output of error messages
c by lsoda.  on the first call to lsoda, supply arguments as follows..
c
c f      = name of subroutine for right-hand side vector f.
c          this name must be declared external in calling program.
c neq    = number of first order ode-s.
c y      = array of initial values, of length neq.
c t      = the initial value of the independent variable.
c tout   = first point where output is desired (.ne. t).
c itol   = 1 or 2 according as atol (below) is a scalar or array.
c rtol   = relative tolerance parameter (scalar).
c atol   = absolute tolerance parameter (scalar or array).
c          the estimated local error in y(i) will be controlled so as
c          to be less than
c             ewt(i) = rtol*abs(y(i)) + atol     if itol = 1, or
c             ewt(i) = rtol*abs(y(i)) + atol(i)  if itol = 2.
c          thus the local error test passes if, in each component,
c          either the absolute error is less than atol (or atol(i)),
c          or the relative error is less than rtol.
c          use rtol = 0.0 for pure absolute error control, and
c          use atol = 0.0 (or atol(i) = 0.0) for pure relative error
c          control.  caution.. actual (global) errors may exceed these
c          local tolerances, so choose them conservatively.
c itask  = 1 for normal computation of output values of y at t = tout.
c istate = integer flag (input and output).  set istate = 1.
c iopt   = 0 to indicate no optional inputs used.
c rwork  = real work array of length at least..
c             22 + neq * std::max(16, neq + 9).
c          see also paragraph e below.
c lrw    = declared length of rwork (in user-s dimension).
c iwork  = integer work array of length at least  20 + neq.
c liw    = declared length of iwork (in user-s dimension).
c jac    = name of subroutine for jacobian matrix.
c          use a dummy name.  see also paragraph e below.
c jt     = jacobian type indicator.  set jt = 2.
c          see also paragraph e below.
c note that the main program must declare arrays y, rwork, iwork,
c and possibly atol.
c
c c. the output from the first call (or any call) is..
c      y = array of computed values of y(t) vector.
c      t = corresponding value of independent variable (normally tout).
c istate = 2  if lsoda was successful, negative otherwise.
c          -1 means excess work done on this call (perhaps wrong jt).
c          -2 means excess accuracy requested (tolerances too small).
c          -3 means illegal input detected (see printed message).
c          -4 means repeated error test failures (check all inputs).
c          -5 means repeated convergence failures (perhaps bad jacobian
c             supplied or wrong choice of jt or tolerances).
c          -6 means error weight became zero during problem. (solution
c             component i vanished, and atol or atol(i) = 0.)
c          -7 means work space insufficient to finish (see messages).
c
c d. to continue the integration after a successful return, simply
c reset tout and call lsoda again.  no other parameters need be reset.
c
c e. note.. if and when lsoda regards the problem as stiff, and
c switches methods accordingly, it must make use of the neq by neq
c jacobian matrix, j = df/dy.  for the sake of simplicity, the
c inputs to lsoda recommended in paragraph b above cause lsoda to
c treat j as a full matrix, and to approximate it internally by
c difference quotients.  alternatively, j can be treated as a band
c matrix (with great potential reduction in the size of the rwork
c array).  also, in either the full or banded case, the user can supply
c j in closed form, with a routine whose name is passed as the jac
c argument.  these alternatives are described in the paragraphs on
c rwork, jac, and jt in the full description of the call sequence below.
c
c-----------------------------------------------------------------------
*/

void
LSODA::lsoda (int neq, double *y, double *t, double tout,
                   int itol, double *rtol, double *atol,
                   int itask, int *istate, int iopt, int jt
            )

{
        int             i, iflag, lenyh, ihit=0;
        double          atoli, ayi, big, h0, hmax, hmx, rh, rtoli, tcrit, tdist, tnext, tol,
                tolsf, tp, size, sum, w0;

        if (*istate == 1) _freevectors();

        /*
        Block a.
        This code block is executed on every call.
        It tests *istate and itask for legality and branches appropriately.
        If *istate > 1 but the flag init shows that initialization has not
        yet been done, an error return occurs.
        If *istate = 1 and tout = t, return immediately.
        */

        if (*istate < 1 || *istate > 3) {
                printf("[lsoda] illegal istate = %d\n", *istate);
                terminate(istate);
                return;
        }
        if (itask < 1 || itask > 5) {
                printf("[lsoda] illegal itask = %d\n", itask);
                terminate(istate);
                return;
        }
        if (init == 0 && (*istate == 2 || *istate == 3)) {
                printf("[lsoda] istate > 1 but lsoda not initialized\n");
                terminate(istate);
                return;
        }
        if (*istate == 1) {
                init = 0;
                if (tout == *t) {
                        ntrep++;
                        if (ntrep < 5) return;
                        printf("[lsoda] repeated calls with istate = 1 and tout = t. run aborted\n"
                                "apparent infinite loop\n");
                        return;
                }
        }
        /*
        Block b.
        The next code block is executed for the initial call ( *istate = 1 ),
        or for a continuation call with parameter changes ( *istate = 3 ).
        It contains checking of all inputs and various initializations.

        First check legality of the non-optional inputs neq, itol, iopt,
        jt, ml, and mu.
        */

        if (*istate == 1 || *istate == 3) {
                ntrep = 0;
                if (neq <= 0) {
                        printf("[lsoda] neq = %d is less than 1\n",neq);
                        terminate(istate);
                        return;
                }
                if (*istate == 3 && neq > n) {
                        printf("[lsoda] istate = 3 and neq increased\n");
                        terminate(istate);
                        return;
                }
                n = neq;
                if (itol < 1 || itol > 4) {
                        printf("[lsoda] itol = %d illegal\n", itol);
                        terminate(istate);
                        return;
                }
                if (iopt < 0 || iopt > 1) {
                        printf("[lsoda] iopt = %d illegal\n", iopt);
                        terminate(istate);
                        return;
                }
                if (jt == 3 || jt < 1 || jt > 5) {
                        printf("[lsoda] jt = %d illegal\n", jt);
                        terminate(istate);
                        return;
                }
                jtyp = jt;
                ml = 0;
                mu = 0;

                /* Next process and check the optional inputs.   */

                /* Default options.   */

                if (iopt == 0)
                {
                        ixpr = 0;
                        mxstep = mxstp0;
                        mxhnil = mxhnl0;
                        hmxi = 0.;
                        hmin = 0.;
                        if (*istate == 1) {
                                h0 = 0.;
                                mxordn = mord[1];
                                mxords = mord[2];
                        }
                }
                /* end if ( iopt == 0 )   */
                /* Optional inputs.   */
                else
                {	/* if ( iopt = 1 )  */
                        ixpr = 0;
                        if (ixpr < 0 || ixpr > 1) {
                                printf("[lsoda] ixpr = %d is illegal\n", ixpr);
                                terminate(istate);
                                return;
                        }
                        mxstep = 0;
                        if (mxstep < 0) {
                                printf("[lsoda] mxstep < 0\n");
                                terminate(istate);
                                return;
                        }
                        if (mxstep == 0) mxstep = mxstp0;
                        mxhnil = 0;
                        if (mxhnil < 0) {
                                printf("[lsoda] mxhnil < 0\n");
                                terminate(istate);
                                return;
                        }
                        if (*istate == 1) {
                                h0 = 0;
                                mxordn = 0;
                                if (mxordn < 0) {
                                        printf("[lsoda] mxordn = %d is less than 0\n", mxordn);
                                        terminate(istate);
                                        return;
                                }
                                if (mxordn == 0) mxordn = 100;
                                mxordn = std::min(mxordn, mord[1]);
                                mxords = 0;
                                if (mxords < 0) {
                                        printf("[lsoda] mxords = %d is less than 0\n", mxords);
                                        terminate(istate);
                                        return;
                                }
                                if (mxords == 0) mxords = 100;
                                mxords = std::min(mxords, mord[2]);
                                if ((tout - *t) * h0 < 0.) {
                                        printf("[lsoda] tout = %g behind t = %g.\n"
                                                "integration direction is given by %g\n",
                                                tout, *t, h0);
                                        terminate(istate);
                                        return;
                                }
                        }	/* end if ( *istate == 1 )  */
                        hmax = 1;
                        if (hmax < 0.)
                        {
                                printf("[lsoda] hmax < 0.\n");
                                terminate(istate);
                                return;
                        }
                        hmxi = 0.;
                        if (hmax > 0)
                                hmxi = 1. / hmax;
                        hmin = 0;
                        if (hmin < 0.) {
                                printf("[lsoda] hmin < 0.\n");
                                terminate(istate);
                                return;
                        }
                }		/* end else   *//* end iopt = 1   */
        }			/* end if ( *istate == 1 || *istate == 3 )   */

        /*
        If *istate = 1, meth is initialized to 1.

        Also allocate memory for yh, wm, ewt, savf, acor, ipvt.
        */
        if (*istate == 1)
        {
                /*
                If memory were not freed, *istate = 3 need not reallocate memory.
                Hence this section is not executed by *istate = 3.
                */
                sqrteta = sqrt(ETA);
                meth = 1;
                g_nyh = nyh = n;
                g_lenyh = lenyh = 1 + std::max(mxordn, mxords);

                yh = new double * [1 + lenyh];

                for (i = 1; i <= lenyh; i++)
                {
                        yh[i] = new double [1 + nyh];
                        //if (error) return;
                }

                wm = new double * [1 + nyh];

                for (i = 1; i <= nyh; i++)
                {
                        wm[i] = new double [1 + nyh];
                        //if (error) return;
                }

                ewt = new double [1 + nyh];

                savf = new double [1 + nyh];

                acor = new double [1 + nyh];

                ipvt = new int [1 + nyh];
        }
        /*
        Check rtol and atol for legality.
        */
        if (*istate == 1 || *istate == 3) {
                rtoli = rtol[1];
                atoli = atol[1];
                for (i = 1; i <= n; i++) {
                        if (itol >= 3)
                                rtoli = rtol[i];
                        if (itol == 2 || itol == 4)
                                atoli = atol[i];
                        if (rtoli < 0.) {
                                printf("[lsoda] rtol = %g is less than 0.\n", rtoli);
                                terminate(istate);
                                freevectors();
                                return;
                        }
                        if (atoli < 0.) {
                                printf("[lsoda] atol = %g is less than 0.\n", atoli);
                                terminate(istate);
                                freevectors();
                                return;
                        }
                }		/* end for   */
        }			/* end if ( *istate == 1 || *istate == 3 )   */
        /*
        If *istate = 3, set flag to signal parameter changes to stoda.
        */
        if (*istate == 3)
        {
                jstart = -1;
        }
        /*
        Block c.
        The next block is for the initial call only ( *istate = 1 ).
        It contains all remaining initializations, the initial call to f,
        and the calculation of the initial step size.
        The error weights in ewt are inverted after being loaded.
        */
        if (*istate == 1) {
                tn = *t;
                tsw = *t;
                maxord = mxordn;
                if (itask == 4 || itask == 5) {
                        tcrit = 0;
                        if ((tcrit - tout) * (tout - *t) < 0.) {
                                printf("[lsoda] itask = 4 or 5 and tcrit behind tout\n");
                                terminate(istate);
                                freevectors();
                                return;
                        }
                        if (h0 != 0. && (*t + h0 - tcrit) * h0 > 0.)
                                h0 = tcrit - *t;
                }
                jstart = 0;
                nhnil = 0;
                nst = 0;
                nje = 0;
                nslast = 0;
                hu = 0.;
                nqu = 0;
                mused = 0;
                miter = 0;
                ccmax = 0.3;
                maxcor = 3;
                msbp = 20;
                mxncf = 10;
                /*
                Initial call to f.
                */
                evalODE(*t, y+1, yh[2]+1, neq);
                // (*f) (*t, y + 1, yh[2] + 1, _data);
                nfe = 1;
                /*
                Load the initial value vector in yh.
                */
                yp1 = yh[1];
                for (i = 1; i <= n; i++)
                        yp1[i] = y[i];
                /*
                Load and invert the ewt array.  ( h is temporarily set to 1. )
                */
                nq = 1;
                h = 1.;
                ewset(itol, rtol, atol, y);
                for (i = 1; i <= n; i++)
                {
                        if (ewt[i] <= 0.)
                        {
                                printf("[lsoda] ewt[%d] = %g <= 0.\n", i, ewt[i]);
                                terminate2(y, t);
                                return;
                        }
                        ewt[i] = 1. / ewt[i];
                }

                /*
                The coding below computes the step size, h0, to be attempted on the
                first step, unless the user has supplied a value for this.
                First check that tout - *t differs significantly from zero.
                A scalar tolerance quantity tol is computed, as std::max(rtol[i])
                if this is positive, or std::max(atol[i]/fabs(y[i])) otherwise, adjusted
                so as to be between 100*ETA and 0.001.
                Then the computed value h0 is given by

                h0^(-2) = 1. / ( tol * w0^2 ) + tol * ( norm(f) )^2

                where   w0     = std::max( fabs(*t), fabs(tout) ),
                f      = the initial value of the vector f(t,y), and
                norm() = the weighted vector norm used throughout, given by
                the vmnorm function routine, and weighted by the
                tolerances initially loaded into the ewt array.

                The sign of h0 is inferred from the initial values of tout and *t.
                fabs(h0) is made < fabs(tout-*t) in any case.
                */
                if (h0 == 0.)
                {
                        tdist = fabs(tout - *t);
                        w0 = std::max(fabs(*t), fabs(tout));
                        if (tdist < 2. * ETA * w0) {
                                printf("[lsoda] tout too close to t to start integration\n ");
                                terminate(istate);
                                freevectors();
                                return;
                        }
                        tol = rtol[1];
                        if (itol > 2)
                        {
                                for (i = 2; i <= n; i++)
                                        tol = std::max(tol, rtol[i]);
                        }
                        if (tol <= 0.)
                        {
                                atoli = atol[1];
                                for (i = 1; i <= n; i++)
                                {
                                        if (itol == 2 || itol == 4)
                                                atoli = atol[i];
                                        ayi = fabs(y[i]);
                                        if (ayi != 0.)
                                                tol = std::max(tol, atoli / ayi);
                                }
                        }
                        tol = std::max(tol, 100. * ETA);
                        tol = std::min(tol, 0.001);
                        sum = vmnorm(n, yh[2], ewt);
                        sum = 1. / (tol * w0 * w0) + tol * sum * sum;
                        h0 = 1. / sqrt(sum);
                        h0 = std::min(h0, tdist);
                        h0 = h0 * ((tout - *t >= 0.) ? 1. : -1.);
                }		/* end if ( h0 == 0. )   */
                /*
                Adjust h0 if necessary to meet hmax bound.
                */
                rh = fabs(h0) * hmxi;
                if (rh > 1.)
                        h0 /= rh;
                /*
                Load h with h0 and scale yh[2] by h0.
                */
                h = h0;
                yp1 = yh[2];
                for (i = 1; i <= n; i++)
                        yp1[i] *= h0;
        }			/* if ( *istate == 1 )   */

        /*
        Block d.
        The next code block is for continuation calls only ( *istate = 2 or 3 )
        and is to check stop conditions before taking a step.
        */
        if (*istate == 2 || *istate == 3)
        {
                nslast = nst;
                switch (itask)
                {
                        case 1:
                                if ((tn - tout) * h >= 0.)
                                {
                                        intdy(tout, 0, y, &iflag);
                                        if (iflag != 0)
                                        {
                                                printf("[lsoda] trouble from intdy, itask = %d, tout = %g\n",
                                                        itask, tout);
                                                terminate(istate);
                                                freevectors();
                                                return;
                                        }
                                        *t = tout;
                                        *istate = 2;
                                        illin = 0;
                                        freevectors();
                                        return;
                                }
                                break;
                        case 2:
                                break;
                        case 3:
                                tp = tn - hu * (1. + 100. * ETA);
                                if ((tp - tout) * h > 0.)
                                {
                                        printf("[lsoda] itask = %d and tout behind tcur - hu\n", itask);
                                        terminate(istate);
                                        freevectors();
                                        return;
                                }
                                if ((tn - tout) * h < 0.) break;
                                successreturn(y, t, itask, ihit, tcrit, istate);
                                return;
                        case 4:
                                tcrit = 0;
                                if ((tn - tcrit) * h > 0.)
                                {
                                        printf("[lsoda] itask = 4 or 5 and tcrit behind tcur\n");
                                        terminate(istate);
                                        freevectors();
                                        return;
                                }
                                if ((tcrit - tout) * h < 0.)
                                {
                                        printf("[lsoda] itask = 4 or 5 and tcrit behind tout\n");
                                        terminate(istate);
                                        freevectors();
                                        return;
                                }
                                if ((tn - tout) * h >= 0.)
                                {
                                        intdy(tout, 0, y, &iflag);
                                        if (iflag != 0)
                                        {
                                                printf("[lsoda] trouble from intdy, itask = %d, tout = %g\n",
                                                        itask, tout);
                                                terminate(istate);
                                                freevectors();
                                                return;
                                        }
                                        *t = tout;
                                        *istate = 2;
                                        illin = 0;
                                        freevectors();
                                        return;
                                }
                        case 5:
                                if (itask == 5)
                                {
                                        tcrit = 0;
                                        if ((tn - tcrit) * h > 0.)
                                        {
                                                printf("[lsoda] itask = 4 or 5 and tcrit behind tcur\n");
                                                terminate(istate);
                                                freevectors();
                                                return;
                                        }
                                }
                                hmx = fabs(tn) + fabs(h);
                                ihit = fabs(tn - tcrit) <= (100. * ETA * hmx);
                                if (ihit)
                                {
                                        *t = tcrit;
                                        successreturn(y, t, itask, ihit, tcrit, istate);
                                        return;
                                }
                                tnext = tn + h * (1. + 4. * ETA);
                                if ((tnext - tcrit) * h <= 0.)
                                        break;
                                h = (tcrit - tn) * (1. - 4. * ETA);
                                if (*istate == 2)
                                        jstart = -2;
                                break;
                }		/* end switch   */
        }			/* end if ( *istate == 2 || *istate == 3 )   */

        /*
        Block e.
        The next block is normally executed for all calls and contains
        the call to the one-step core integrator stoda.

        This is a looping point for the integration steps.

        First check for too many steps being taken, update ewt ( if not at
        start of problem).  Check for too much accuracy being requested, and
        check for h below the roundoff level in *t.
        */
        while (1)
        {
                if (*istate != 1 || nst != 0)
                {
                        if ((nst - nslast) >= mxstep)
                        {
                                printf("[lsoda] %d steps taken before reaching tout\n", mxstep);
                                *istate = -1;
                                terminate2(y, t);
                                return;
                        }
                        ewset(itol, rtol, atol, yh[1]);
                        for (i = 1; i <= n; i++)
                        {
                                if (ewt[i] <= 0.)
                                {
                                        printf("[lsoda] ewt[%d] = %g <= 0.\n", i, ewt[i]);
                                        *istate = -6;
                                        terminate2(y, t);
                                        return;
                                }
                                ewt[i] = 1. / ewt[i];
                        }
                }
                tolsf = ETA * vmnorm(n, yh[1], ewt);
                if (tolsf > 0.01)
                {
                        tolsf = tolsf * 200.;
                        if (nst == 0) {
                                printf("lsoda -- at start of problem, too much accuracy\n");
                                printf("         requested for precision of machine,\n");
                                printf("         suggested scaling factor = %g\n", tolsf);
                                terminate(istate);
                                freevectors();
                                return;
                        }
                        printf("lsoda -- at t = %g, too much accuracy requested\n", *t);
                        printf("         for precision of machine, suggested\n");
                        printf("         scaling factor = %g\n", tolsf);
                        *istate = -2;
                        terminate2(y, t);
                        return;
                }
                if ((tn + h) == tn)
                {
                        nhnil++;
                        if (nhnil <= mxhnil && lsoda_warning)
                        {
                                printf("lsoda -- warning..internal t = %g and h = %g are\n", tn, h);
                                printf("         such that in the machine, t + h = t on the next step\n");
                                printf("         solver will continue anyway.\n");
                                if (nhnil == mxhnil) {
                                        printf("lsoda -- above warning has been issued %d times,\n", nhnil);
                                        printf("         it will not be issued again for this problem\n");
                                }
                        }
                }
                /*
                Call stoda
                */
                stoda(neq, y);

                /*
                printf( "meth= %d,   order= %d,   nfe= %d,   nje= %d\n",
                meth, nq, nfe, nje );
                printf( "t= %20.15e,   h= %20.15e,   nst=%d\n", tn, h, nst );
                printf( "y= %20.15e,   %20.15e,   %20.15e\n\n\n",
                yh[1][1], yh[1][2], yh[1][3] );
                */

                if (kflag == 0)
                {
                        /*
                        Block f.
                        The following block handles the case of a successful return from the
                        core integrator ( kflag = 0 ).
                        If a method switch was just made, record tsw, reset maxord,
                        set jstart to -1 to signal stoda to complete the switch,
                        and do extra printing of data if ixpr = 1.
                        Then, in any case, check for stop conditions.
                        */
                        init = 1;
                        if (meth != mused)
                        {
                                tsw = tn;
                                maxord = mxordn;
                                if (meth == 2)
                                        maxord = mxords;
                                jstart = -1;
                                if (ixpr)
                                {
                                        if (meth == 2)
                                                printf("[lsoda] a switch to the stiff method has occurred");
                                        if (meth == 1)
                                                printf("[lsoda] a switch to the nonstiff method has occurred");
                                        printf("at t = %g, tentative step size h = %g, step nst = %d\n", tn, h, nst);
                                }
                        }	/* end if ( meth != mused )   */
                        /*
                        itask = 1.
                        If tout has been reached, interpolate.
                        */
                        if (itask == 1)
                        {
                                if ((tn - tout) * h < 0.)
                                        continue;
                                intdy(tout, 0, y, &iflag);
                                *t = tout;
                                *istate = 2;
                                illin = 0;
                                freevectors();
                                return;
                        }
                        /*
                        itask = 2.
                        */
                        if (itask == 2)
                        {
                                successreturn(y, t, itask, ihit, tcrit, istate);
                                return;
                        }
                        /*
                        itask = 3.
                        Jump to exit if tout was reached.
                        */
                        if (itask == 3)
                        {
                                if ((tn - tout) * h >= 0.)
                                {
                                        successreturn(y, t, itask, ihit, tcrit, istate);
                                        return;
                                }
                                continue;
                        }
                        /*
                        itask = 4.
                        See if tout or tcrit was reached.  Adjust h if necessary.
                        */
                        if (itask == 4)
                        {
                                if ((tn - tout) * h >= 0.)
                                {
                                        intdy(tout, 0, y, &iflag);
                                        *t = tout;
                                        *istate = 2;
                                        illin = 0;
                                        freevectors();
                                        return;
                                }
                                else
                                {
                                        hmx = fabs(tn) + fabs(h);
                                        ihit = fabs(tn - tcrit) <= (100. * ETA * hmx);
                                        if (ihit)
                                        {
                                                successreturn(y, t, itask, ihit, tcrit, istate);
                                                return;
                                        }
                                        tnext = tn + h * (1. + 4. * ETA);
                                        if ((tnext - tcrit) * h <= 0.)
                                                continue;
                                        h = (tcrit - tn) * (1. - 4. * ETA);
                                        jstart = -2;
                                        continue;
                                }
                        }	/* end if ( itask == 4 )   */
                        /*
                        itask = 5.
                        See if tcrit was reached and jump to exit.
                        */
                        if (itask == 5)
                        {
                                hmx = fabs(tn) + fabs(h);
                                ihit = fabs(tn - tcrit) <= (100. * ETA * hmx);
                                successreturn(y, t, itask, ihit, tcrit, istate);
                                return;
                        }
                }		/* end if ( kflag == 0 )   */
                /*
                kflag = -1, error test failed repeatedly or with fabs(h) = hmin.
                kflag = -2, convergence failed repeatedly or with fabs(h) = hmin.
                */
                if (kflag == -1 || kflag == -2)
                {
                        printf("lsoda -- at t = %g and step size h = %g, the\n", tn, h);
                        if (kflag == -1) {
                                printf("         error test failed repeatedly or\n");
                                printf("         with fabs(h) = hmin\n");
                                *istate = -4;
                        }
                        if (kflag == -2) {
                                printf("         corrector convergence failed repeatedly or\n");
                                printf("         with fabs(h) = hmin\n");
                                *istate = -5;
                        }
                        big = 0.;
                        imxer = 1;
                        for (i = 1; i <= n; i++) {
                                size = fabs(acor[i]) * ewt[i];
                                if (big < size) {
                                        big = size;
                                        imxer = i;
                                }
                        }
                        terminate2(y, t);
                        return;
                }		/* end if ( kflag == -1 || kflag == -2 )   */
        }			/* end while   */

}				/* end lsoda   */



void
LSODA::stoda(int neq, double *y)
{
        int             corflag, orderflag;
        int             i, i1, j, m, ncf;
        double          del, delp, dsm, dup, exup, r, rh, rhup, told;
        double          pdh, pnorm;

        /*
        stoda performs one step of the integration of an initial value
        problem for a system of ordinary differential equations.
        Note.. stoda is independent of the value of the iteration method
        indicator miter, when this is != 0, and hence is independent
        of the type of chord method used, or the Jacobian structure.
        Communication with stoda is done with the following variables:

        jstart = an integer used for input only, with the following
        values and meanings:

        0  perform the first step,
        > 0  take a new step continuing from the last,
        -1  take the next step with a new value of h,
        n, meth, miter, and/or matrix parameters.
        -2  take the next step with a new value of h,
        but with other inputs unchanged.

        kflag = a completion code with the following meanings:

        0  the step was successful,
        -1  the requested error could not be achieved,
        -2  corrector convergence could not be achieved,
        -3  fatal error in prja or solsy.

        miter = corrector iteration method:

        0  functional iteration,
        >0  a chord method corresponding to jacobian type jt.

        */
        kflag = 0;
        told = tn;
        ncf = 0;
        ierpj = 0;
        iersl = 0;
        jcur = 0;
        delp = 0.;

        /*
        On the first call, the order is set to 1, and other variables are
        initialized.  rmax is the maximum ratio by which h can be increased
        in a single step.  It is initially 1.e4 to compensate for the small
        initial h, but then is normally equal to 10.  If a filure occurs
        (in corrector convergence or error test), rmax is set at 2 for
        the next increase.
        cfode is called to get the needed coefficients for both methods.
        */
        if (jstart == 0) {
                lmax = maxord + 1;
                nq = 1;
                l = 2;
                ialth = 2;
                rmax = 10000.;
                rc = 0.;
                el0 = 1.;
                crate = 0.7;
                hold = h;
                nslp = 0;
                ipup = miter;
                /*
                Initialize switching parameters.  meth = 1 is assumed initially.
                */
                icount = 20;
                irflag = 0;
                pdest = 0.;
                pdlast = 0.;
                ratio = 5.;
                cfode(2);
                for (i = 1; i <= 5; i++)
                        cm2[i] = tesco[i][2] * elco[i][i + 1];
                cfode(1);
                for (i = 1; i <= 12; i++)
                        cm1[i] = tesco[i][2] * elco[i][i + 1];
                resetcoeff();
        }			/* end if ( jstart == 0 )   */
        /*
        The following block handles preliminaries needed when jstart = -1.
        ipup is set to miter to force a matrix update.
        If an order increase is about to be considered ( ialth = 1 ),
        ialth is reset to 2 to postpone consideration one more step.
        If the caller has changed meth, cfode is called to reset
        the coefficients of the method.
        If h is to be changed, yh must be rescaled.
        If h or meth is being changed, ialth is reset to l = nq + 1
        to prevent further changes in h for that many steps.
        */
        if (jstart == -1) {
                ipup = miter;
                lmax = maxord + 1;
                if (ialth == 1)
                        ialth = 2;
                if (meth != mused) {
                        cfode(meth);
                        ialth = l;
                        resetcoeff();
                }
                if (h != hold) {
                        rh = h / hold;
                        h = hold;
                        scaleh(&rh, &pdh);
                }
        }			/* if ( jstart == -1 )   */
        if (jstart == -2) {
                if (h != hold) {
                        rh = h / hold;
                        h = hold;
                        scaleh(&rh, &pdh);
                }
        }			/* if ( jstart == -2 )   */
        /*
        Prediction.
        This section computes the predicted values by effectively
        multiplying the yh array by the pascal triangle matrix.
        rc is the ratio of new to old values of the coefficient h * el[1].
        When rc differs from 1 by more than ccmax, ipup is set to miter
        to force pjac to be called, if a jacobian is involved.
        In any case, prja is called at least every msbp steps.
        */
        while (1) {
                while (1) {
                        if (fabs(rc - 1.) > ccmax)
                                ipup = miter;
                        if (nst >= nslp + msbp)
                                ipup = miter;
                        tn += h;
                        for (j = nq; j >= 1; j--)
                                for (i1 = j; i1 <= nq; i1++) {
                                yp1 = yh[i1];
                                yp2 = yh[i1 + 1];
                                for (i = 1; i <= n; i++)
                                        yp1[i] += yp2[i];
                        }
                        pnorm = vmnorm(n, yh[1], ewt);

                        correction(neq, y, &corflag, pnorm, &del, &delp, &told, &ncf, &rh, &m);
                        if (corflag == 0)
                                break;
                        if (corflag == 1) {
                                rh = std::max(rh, hmin / fabs(h));
                                scaleh(&rh, &pdh);
                                continue;
                        }
                        if (corflag == 2) {
                                kflag = -2;
                                hold = h;
                                jstart = 1;
                                return;
                        }
                }		/* end inner while ( corrector loop )   */
                /*
                The corrector has converged.  jcur is set to 0
                to signal that the Jacobian involved may need updating later.
                The local error test is done now.
                */
                jcur = 0;
                if (m == 0)
                        dsm = del / tesco[nq][2];
                if (m > 0)
                        dsm = vmnorm(n, acor, ewt) / tesco[nq][2];
                if (dsm <= 1.) {
                        /*
                        After a successful step, update the yh array.
                        Decrease icount by 1, and if it is -1, consider switching methods.
                        If a method switch is made, reset various parameters,
                        rescale the yh array, and exit.  If there is no switch,
                        consider changing h if ialth = 1.  Otherwise decrease ialth by 1.
                        If ialth is then 1 and nq < maxord, then acor is saved for
                        use in a possible order increase on the next step.
                        If a change in h is considered, an increase or decrease in order
                        by one is considered also.  A change in h is made only if it is by
                        a factor of at least 1.1.  If not, ialth is set to 3 to prevent
                        testing for that many steps.
                        */
                        kflag = 0;
                        nst++;
                        hu = h;
                        nqu = nq;
                        mused = meth;
                        for (j = 1; j <= l; j++) {
                                yp1 = yh[j];
                                r = el[j];
                                for (i = 1; i <= n; i++)
                                        yp1[i] += r * acor[i];
                        }
                        icount--;
                        if (icount < 0) {
                                methodswitch(dsm, pnorm, &pdh, &rh);
                                if (meth != mused) {
                                        rh = std::max(rh, hmin / fabs(h));
                                        scaleh(&rh, &pdh);
                                        rmax = 10.;
                                        endstoda();
                                        break;
                                }
                        }
                        /*
                        No method switch is being made.  Do the usual step/order selection.
                        */
                        ialth--;
                        if (ialth == 0) {
                                rhup = 0.;
                                if (l != lmax) {
                                        yp1 = yh[lmax];
                                        for (i = 1; i <= n; i++)
                                                savf[i] = acor[i] - yp1[i];
                                        dup = vmnorm(n, savf, ewt) / tesco[nq][3];
                                        exup = 1. / (double) (l + 1);
                                        rhup = 1. / (1.4 * pow(dup, exup) + 0.0000014);
                                }
                                orderswitch(&rhup, dsm, &pdh, &rh, &orderflag);
                                /*
                                No change in h or nq.
                                */
                                if (orderflag == 0) {
                                        endstoda();
                                        break;
                                }
                                /*
                                h is changed, but not nq.
                                */
                                if (orderflag == 1) {
                                        rh = std::max(rh, hmin / fabs(h));
                                        scaleh(&rh, &pdh);
                                        rmax = 10.;
                                        endstoda();
                                        break;
                                }
                                /*
                                both nq and h are changed.
                                */
                                if (orderflag == 2) {
                                        resetcoeff();
                                        rh = std::max(rh, hmin / fabs(h));
                                        scaleh(&rh, &pdh);
                                        rmax = 10.;
                                        endstoda();
                                        break;
                                }
                        }	/* end if ( ialth == 0 )   */
                        if (ialth > 1 || l == lmax) {
                                endstoda();
                                break;
                        }
                        yp1 = yh[lmax];
                        for (i = 1; i <= n; i++)
                                yp1[i] = acor[i];
                        endstoda();
                        break;
                }
                /* end if ( dsm <= 1. )   */
                /*
                The error test failed.  kflag keeps track of multiple failures.
                Restore tn and the yh array to their previous values, and prepare
                to try the step again.  Compute the optimum step size for this or
                one lower.  After 2 or more failures, h is forced to decrease
                by a factor of 0.2 or less.
                */
                else {
                        kflag--;
                        tn = told;
                        for (j = nq; j >= 1; j--)
                                for (i1 = j; i1 <= nq; i1++) {
                                yp1 = yh[i1];
                                yp2 = yh[i1 + 1];
                                for (i = 1; i <= n; i++)
                                        yp1[i] -= yp2[i];
                        }
                        rmax = 2.;
                        if (fabs(h) <= hmin * 1.00001) {
                                kflag = -1;
                                hold = h;
                                jstart = 1;
                                break;
                        }
                        if (kflag > -3) {
                                rhup = 0.;
                                orderswitch(&rhup, dsm, &pdh, &rh, &orderflag);
                                if (orderflag == 1 || orderflag == 0) {
                                        if (orderflag == 0)
                                                rh = std::min(rh, 0.2);
                                        rh = std::max(rh, hmin / fabs(h));
                                        scaleh(&rh, &pdh);
                                }
                                if (orderflag == 2) {
                                        resetcoeff();
                                        rh = std::max(rh, hmin / fabs(h));
                                        scaleh(&rh, &pdh);
                                }
                                continue;
                        }
                        /* if ( kflag > -3 )   */
                        /*
                        Control reaches this section if 3 or more failures have occurred.
                        If 10 failures have occurred, exit with kflag = -1.
                        It is assumed that the derivatives that have accumulated in the
                        yh array have errors of the wrong order.  Hence the first
                        derivative is recomputed, and the order is set to 1.  Then
                        h is reduced by a factor of 10, and the step is retried,
                        until it succeeds or h reaches hmin.
                        */
                        else {
                                if (kflag == -10) {
                                        kflag = -1;
                                        hold = h;
                                        jstart = 1;
                                        break;
                                } else {
                                        rh = 0.1;
                                        rh = std::max(hmin / fabs(h), rh);
                                        h *= rh;
                                        yp1 = yh[1];
                                        for (i = 1; i <= n; i++)
                                                y[i] = yp1[i];
                                        evalODE(tn, y+1, savf+1, neq);
                                        // (*f) (tn, y + 1, savf + 1, _data);
                                        nfe++;
                                        yp1 = yh[2];
                                        for (i = 1; i <= n; i++)
                                                yp1[i] = h * savf[i];
                                        ipup = miter;
                                        ialth = 5;
                                        if (nq == 1)
                                                continue;
                                        nq = 1;
                                        l = 2;
                                        resetcoeff();
                                        continue;
                                }
                        }	/* end else -- kflag <= -3 */
                }		/* end error failure handling   */
        }			/* end outer while   */

}				/* end stoda   */

void
LSODA::ewset (int itol, double *rtol, double *atol, double *ycur)
{
        int             i;

        switch (itol) {
                case 1:
                        for (i = 1; i <= n; i++)
                        ewt[i] = rtol[1] * fabs(ycur[i]) + atol[1];
                break;
                case 2:
                        for (i = 1; i <= n; i++)
                        ewt[i] = rtol[1] * fabs(ycur[i]) + atol[i];
                break;
                case 3:
                        for (i = 1; i <= n; i++)
                        ewt[i] = rtol[i] * fabs(ycur[i]) + atol[1];
                break;
                case 4:
                        for (i = 1; i <= n; i++)
                        ewt[i] = rtol[i] * fabs(ycur[i]) + atol[i];
                break;
        }

}				/* end ewset   */

void
LSODA::intdy (double t, int k, double *dky, int *iflag)

/*
   Intdy computes interpolated values of the k-th derivative of the
   dependent variable vector y, and stores it in dky.  This routine
   is called within the package with k = 0 and *t = tout, but may
   also be called by the user for any k up to the current order.
   ( See detailed instructions in the usage documentation. )

   The computed values in dky are gotten by interpolation using the
   Nordsieck history array yh.  This array corresponds uniquely to a
   vector-valued polynomial of degree nqcur or less, and dky is set
   to the k-th derivative of this polynomial at t.
   The formula for dky is

             q
   dky[i] = sum c[k][j] * ( t - tn )^(j-k) * h^(-j) * yh[j+1][i]
            j=k

   where c[k][j] = j*(j-1)*...*(j-k+1), q = nqcur, tn = tcur, h = hcur.
   The quantities nq = nqcur, l = nq+1, n = neq, tn, and h are declared
   static globally.  The above sum is done in reverse order.
   *iflag is returned negative if either k or t is out of bounds.
*/

{
        int             i, ic, j, jj, jp1;
        double          c, r, s, tp;

        *iflag = 0;
        if (k < 0 || k > nq)
        {
                printf("[intdy] k = %d illegal\n", k);
                *iflag = -1;
                return;
        }
        tp = tn - hu - 100. * ETA * (tn + hu);
        if ((t - tp) * (t - tn) > 0.)
        {
                if (lsoda_warning)
                        printf("intdy -- t = %g illegal. t not in interval tcur - hu to tcur\n", t);
                *iflag = -2;
                return;
        }
        s = (t - tn) / h;
        ic = 1;
        for (jj = l - k; jj <= nq; jj++)
                ic *= jj;
        c = (double) ic;
        yp1 = yh[l];
        for (i = 1; i <= n; i++)
                dky[i] = c * yp1[i];
        for (j = nq - 1; j >= k; j--) {
                jp1 = j + 1;
                ic = 1;
                for (jj = jp1 - k; jj <= j; jj++)
                        ic *= jj;
                c = (double) ic;
                yp1 = yh[jp1];
                for (i = 1; i <= n; i++)
                        dky[i] = c * yp1[i] + s * dky[i];
        }
        if (k == 0)
                return;
        r = pow(h, (double) (-k));
        for (i = 1; i <= n; i++)
                dky[i] *= r;

}				/* end intdy   */

void
LSODA::cfode(int meth)
{
        int             i, nq, nqm1, nqp1;
        double          agamq, fnq, fnqm1, pc[13], pint, ragq, rqfac, rq1fac, tsign, xpin;
        /*
        cfode is called by the integrator routine to set coefficients
        needed there.  The coefficients for the current method, as
        given by the value of meth, are set for all orders and saved.
        The maximum order assumed here is 12 if meth = 1 and 5 if meth = 2.
        ( A smaller value of the maximum order is also allowed. )
        cfode is called once at the beginning of the problem, and
        is not called again unless and until meth is changed.

        The elco array contains the basic method coefficients.
        The coefficients el[i], 1 < i < nq+1, for the method of
        order nq are stored in elco[nq][i].  They are given by a generating
        polynomial, i.e.,

        l(x) = el[1] + el[2]*x + ... + el[nq+1]*x^nq.

        For the implicit Adams method, l(x) is given by

        dl/dx = (x+1)*(x+2)*...*(x+nq-1)/factorial(nq-1),   l(-1) = 0.

        For the bdf methods, l(x) is given by

        l(x) = (x+1)*(x+2)*...*(x+nq)/k,

        where   k = factorial(nq)*(1+1/2+...+1/nq).

        The tesco array contains test constants used for the
        local error test and the selection of step size and/or order.
        At order nq, tesco[nq][k] is used for the selection of step
        size at order nq-1 if k = 1, at order nq if k = 2, and at order
        nq+1 if k = 3.
        */
        if (meth == 1) {
                elco[1][1] = 1.;
                elco[1][2] = 1.;
                tesco[1][1] = 0.;
                tesco[1][2] = 2.;
                tesco[2][1] = 1.;
                tesco[12][3] = 0.;
                pc[1] = 1.;
                rqfac = 1.;
                for (nq = 2; nq <= 12; nq++) {
                        /*
                        The pc array will contain the coefficients of the polynomial

                        p(x) = (x+1)*(x+2)*...*(x+nq-1).

                        Initially, p(x) = 1.
                        */
                        rq1fac = rqfac;
                        rqfac = rqfac / (double) nq;
                        nqm1 = nq - 1;
                        fnqm1 = (double) nqm1;
                        nqp1 = nq + 1;
                        /*
                        Form coefficients of p(x)*(x+nq-1).
                        */
                        pc[nq] = 0.;
                        for (i = nq; i >= 2; i--)
                                pc[i] = pc[i - 1] + fnqm1 * pc[i];
                        pc[1] = fnqm1 * pc[1];
                        /*
                        Compute integral, -1 to 0, of p(x) and x*p(x).
                        */
                        pint = pc[1];
                        xpin = pc[1] / 2.;
                        tsign = 1.;
                        for (i = 2; i <= nq; i++) {
                                tsign = -tsign;
                                pint += tsign * pc[i] / (double) i;
                                xpin += tsign * pc[i] / (double) (i + 1);
                        }
                        /*
                        Store coefficients in elco and tesco.
                        */
                        elco[nq][1] = pint * rq1fac;
                        elco[nq][2] = 1.;
                        for (i = 2; i <= nq; i++)
                                elco[nq][i + 1] = rq1fac * pc[i] / (double) i;
                        agamq = rqfac * xpin;
                        ragq = 1. / agamq;
                        tesco[nq][2] = ragq;
                        if (nq < 12)
                                tesco[nqp1][1] = ragq * rqfac / (double) nqp1;
                        tesco[nqm1][3] = ragq;
                }		/* end for   */
                return;
        }			/* end if ( meth == 1 )   */
        /*
        meth = 2.
        */
        pc[1] = 1.;
        rq1fac = 1.;
        /*
        The pc array will contain the coefficients of the polynomial

        p(x) = (x+1)*(x+2)*...*(x+nq).

        Initially, p(x) = 1.
        */
        for (nq = 1; nq <= 5; nq++) {
                fnq = (double) nq;
                nqp1 = nq + 1;
                /*
                Form coefficients of p(x)*(x+nq).
                */
                pc[nqp1] = 0.;
                for (i = nq + 1; i >= 2; i--)
                        pc[i] = pc[i - 1] + fnq * pc[i];
                pc[1] *= fnq;
                /*
                Store coefficients in elco and tesco.
                */
                for (i = 1; i <= nqp1; i++)
                        elco[nq][i] = pc[i] / pc[2];
                elco[nq][2] = 1.;
                tesco[nq][1] = rq1fac;
                tesco[nq][2] = ((double) nqp1) / elco[nq][1];
                tesco[nq][3] = ((double) (nq + 2)) / elco[nq][1];
                rq1fac /= fnq;
        }
        return;

}				/* end cfode   */

void
LSODA::scaleh (double *rh, double *pdh)
{
        double          r;
        int             j, i;
        /*
        If h is being changed, the h ratio rh is checked against rmax, hmin,
        and hmxi, and the yh array is rescaled.  ialth is set to l = nq + 1
        to prevent a change of h for that many steps, unless forced by a
        convergence or error test failure.
        */
        *rh = std::min(*rh, rmax);
        *rh = *rh / std::max(1., fabs(h) * hmxi * *rh);
        /*
        If meth = 1, also restrict the new step size by the stability region.
        If this reduces h, set irflag to 1 so that if there are roundoff
        problems later, we can assume that is the cause of the trouble.
        */
        if (meth == 1) {
                irflag = 0;
                *pdh = std::max(fabs(h) * pdlast, 0.000001);
                if ((*rh * *pdh * 1.00001) >= sm1[nq]) {
                        *rh = sm1[nq] / *pdh;
                        irflag = 1;
                }
        }
        r = 1.;
        for (j = 2; j <= l; j++) {
                r *= *rh;
                yp1 = yh[j];
                for (i = 1; i <= n; i++)
                        yp1[i] *= r;
        }
        h *= *rh;
        rc *= *rh;
        ialth = l;

}				/* end scaleh   */


void
LSODA::prja (int neq, double *y)
{
        int             i, ier, j;
        double          fac, hl0, r, r0, yj;
        /*
        prja is called by stoda to compute and process the matrix
        P = I - h * el[1] * J, where J is an approximation to the Jacobian.
        Here J is computed by finite differencing.
        J, scaled by -h * el[1], is stored in wm.  Then the norm of J ( the
        matrix norm consistent with the weighted max-norm on vectors given
        by vmnorm ) is computed, and J is overwritten by P.  P is then
        subjected to LU decomposition in preparation for later solution
        of linear systems with p as coefficient matrix.  This is done
        by dgefa if miter = 2, and by dgbfa if miter = 5.
        */
        nje++;
        ierpj = 0;
        jcur = 1;
        hl0 = h * el0;
        /*
        If miter = 2, make n calls to f to approximate J.
        */
        if (miter != 2) {
                printf("[prja] miter != 2\n");
                return;
        }
        if (miter == 2) {
                fac = vmnorm(n, savf, ewt);
                r0 = 1000. * fabs(h) * ETA * ((double) n) * fac;
                if (r0 == 0.)
                        r0 = 1.;
                for (j = 1; j <= n; j++) {
                        yj = y[j];
                        r = std::max(sqrteta * fabs(yj), r0 / ewt[j]);
                        y[j] += r;
                        fac = -hl0 / r;
                        evalODE(tn, y+1, acor+1, neq);
                        for (i = 1; i <= n; i++)
                                wm[i][j] = (acor[i] - savf[i]) * fac;
                        y[j] = yj;
                }
                nfe += n;
                /*
                Compute norm of Jacobian.
                */
                pdnorm = fnorm(n, wm, ewt) / fabs(hl0);
                /*
                Add identity matrix.
                */
                for (i = 1; i <= n; i++)
                        wm[i][i] += 1.;
                /*
                Do LU decomposition on P.
                */
                dgefa(wm, n, ipvt, &ier);
                if (ier != 0)
                        ierpj = 1;
                return;
        }
}				/* end prja   */


void
LSODA::correction(int neq, double *y, int *corflag,
                                           double pnorm, double *del, double *delp, double *told,
                                           int *ncf, double *rh, int *m)
/*
   *corflag = 0 : corrector converged,
              1 : step size to be reduced, redo prediction,
              2 : corrector cannot converge, failure flag.
*/

{
        int             i;
        double          rm, rate, dcon;

        /*
        Up to maxcor corrector iterations are taken.  A convergence test is
        made on the r.m.s. norm of each correction, weighted by the error
        weight vector ewt.  The sum of the corrections is accumulated in the
        vector acor[i].  The yh array is not altered in the corrector loop.
        */

        *m = 0;
        *corflag = 0;
        rate = 0.;
        *del = 0.;
        yp1 = yh[1];
        for (i = 1; i <= n; i++)
                y[i] = yp1[i];
        evalODE(tn, y+1, savf+1, neq);
        nfe++;
        /*
        If indicated, the matrix P = I - h * el[1] * J is reevaluated and
        preprocessed before starting the corrector iteration.  ipup is set
        to 0 as an indicator that this has been done.
        */
        while (1) {
                if (*m == 0) {
                        if (ipup > 0) {
                                prja(neq, y);
                                ipup = 0;
                                rc = 1.;
                                nslp = nst;
                                crate = 0.7;
                                if (ierpj != 0) {
                                        corfailure(told, rh, ncf, corflag);
                                        return;
                                }
                        }
                        for (i = 1; i <= n; i++)
                                acor[i] = 0.;
                }		/* end if ( *m == 0 )   */
                if (miter == 0) {
                        /*
                        In case of functional iteration, update y directly from
                        the result of the last function evaluation.
                        */
                        yp1 = yh[2];
                        for (i = 1; i <= n; i++) {
                                savf[i] = h * savf[i] - yp1[i];
                                y[i] = savf[i] - acor[i];
                        }
                        *del = vmnorm(n, y, ewt);
                        yp1 = yh[1];
                        for (i = 1; i <= n; i++) {
                                y[i] = yp1[i] + el[1] * savf[i];
                                acor[i] = savf[i];
                        }
                }
                /* end functional iteration   */
                /*
                In the case of the chord method, compute the corrector error,
                and solve the linear system with that as right-hand side and
                P as coefficient matrix.

                */
                else {
                        yp1 = yh[2];
                        for (i = 1; i <= n; i++)
                                y[i] = h * savf[i] - (yp1[i] + acor[i]);
                        solsy(y);
                        *del = vmnorm(n, y, ewt);
                        yp1 = yh[1];
                        for (i = 1; i <= n; i++) {
                                acor[i] += y[i];
                                y[i] = yp1[i] + el[1] * acor[i];
                        }
                }		/* end chord method   */
                /*
                Test for convergence.  If *m > 0, an estimate of the convergence
                rate constant is stored in crate, and this is used in the test.

                We first check for a change of iterates that is the size of
                roundoff error.  If this occurs, the iteration has converged, and a
                new rate estimate is not formed.
                In all other cases, force at least two iterations to estimate a
                local Lipschitz constant estimate for Adams method.
                On convergence, form pdest = local maximum Lipschitz constant
                estimate.  pdlast is the most recent nonzero estimate.
                */
                if (*del <= 100. * pnorm * ETA)
                        break;
                if (*m != 0 || meth != 1) {
                        if (*m != 0) {
                                rm = 1024.0;
                                if (*del <= (1024. * *delp))
                                        rm = *del / *delp;
                                rate = std::max(rate, rm);
                                crate = std::max(0.2 * crate, rm);
                        }
                        dcon = *del * std::min(1., 1.5 * crate) / (tesco[nq][2] * conit);
                        if (dcon <= 1.) {
                                pdest = std::max(pdest, rate / fabs(h * el[1]));
                                if (pdest != 0.)
                                        pdlast = pdest;
                                break;
                        }
                }
                /*
                The corrector iteration failed to converge.
                If miter != 0 and the Jacobian is out of date, prja is called for
                the next try.   Otherwise the yh array is retracted to its values
                before prediction, and h is reduced, if possible.  If h cannot be
                reduced or mxncf failures have occured, exit with corflag = 2.
                */
                (*m)++;
                if (*m == maxcor || (*m >= 2 && *del > 2. * *delp)) {
                        if (miter == 0 || jcur == 1) {
                                corfailure(told, rh, ncf, corflag);
                                return;
                        }
                        ipup = miter;
                        /*
                        Restart corrector if Jacobian is recomputed.
                        */
                        *m = 0;
                        rate = 0.;
                        *del = 0.;
                        yp1 = yh[1];
                        for (i = 1; i <= n; i++)
                                y[i] = yp1[i];
                        evalODE(tn, y+1, savf+1, neq);
                        nfe++;
                }
                /*
                Iterate corrector.
                */
                else {
                        *delp = *del;
                        evalODE(tn, y+1, savf+1, neq);
                        nfe++;
                }
        }			/* end while   */
}				/* end correction   */

void
LSODA::corfailure (double *told, double *rh, int *ncf, int *corflag)
{
        int             j, i1, i;

        ncf++;
        rmax = 2.;
        tn = *told;
        for (j = nq; j >= 1; j--)
                for (i1 = j; i1 <= nq; i1++) {
                yp1 = yh[i1];
                yp2 = yh[i1 + 1];
                for (i = 1; i <= n; i++)
                        yp1[i] -= yp2[i];
        }
        if (fabs(h) <= hmin * 1.00001 || *ncf == mxncf) {
                *corflag = 2;
                return;
        }
        *corflag = 1;
        *rh = 0.25;
        ipup = miter;

}

void
LSODA::solsy (double *y)

/*
   This routine manages the solution of the linear system arising from
   a chord iteration.  It is called if miter != 0.
   If miter is 2, it calls dgesl to accomplish this.
   If miter is 5, it calls dgbsl.

   y = the right-hand side vector on input, and the solution vector
       on output.
*/

{
        iersl = 0;
        if (miter != 2) {
                printf("solsy -- miter != 2\n");
                return;
        }
        if (miter == 2)
                dgesl(wm, n, ipvt, y, 0);
        return;

}

void
LSODA::methodswitch (double dsm, double pnorm, double *pdh, double *rh)
{
        int             lm1, lm1p1, lm2, lm2p1, nqm1, nqm2;
        double          rh1, rh2, rh1it, exm2, dm2, exm1, dm1, alpha, exsm;

        /*
        We are current using an Adams method.  Consider switching to bdf.
        If the current order is greater than 5, assume the problem is
        not stiff, and skip this section.
        If the Lipschitz constant and error estimate are not polluted
        by roundoff, perform the usual test.
        Otherwise, switch to the bdf methods if the last step was
        restricted to insure stability ( irflag = 1 ), and stay with Adams
        method if not.  When switching to bdf with polluted error estimates,
        in the absence of other information, double the step size.

        When the estimates are ok, we make the usual test by computing
        the step size we could have (ideally) used on this step,
        with the current (Adams) method, and also that for the bdf.
        If nq > mxords, we consider changing to order mxords on switching.
        Compare the two step sizes to decide whether to switch.
        The step size advantage must be at least ratio = 5 to switch.
        */
        if (meth == 1) {
                if (nq > 5)
                        return;
                if (dsm <= (100. * pnorm * ETA) || pdest == 0.) {
                        if (irflag == 0)
                                return;
                        rh2 = 2.;
                        nqm2 = std::min(nq, mxords);
                } else {
                        exsm = 1. / (double) l;
                        rh1 = 1. / (1.2 * pow(dsm, exsm) + 0.0000012);
                        rh1it = 2. * rh1;
                        *pdh = pdlast * fabs(h);
                        if ((*pdh * rh1) > 0.00001)
                                rh1it = sm1[nq] / *pdh;
                        rh1 = std::min(rh1, rh1it);
                        if (nq > mxords) {
                                nqm2 = mxords;
                                lm2 = mxords + 1;
                                exm2 = 1. / (double) lm2;
                                lm2p1 = lm2 + 1;
                                dm2 = vmnorm(n, yh[lm2p1], ewt) / cm2[mxords];
                                rh2 = 1. / (1.2 * pow(dm2, exm2) + 0.0000012);
                        } else {
                                dm2 = dsm * (cm1[nq] / cm2[nq]);
                                rh2 = 1. / (1.2 * pow(dm2, exsm) + 0.0000012);
                                nqm2 = nq;
                        }
                        if (rh2 < ratio * rh1)
                                return;
                }
                /*
                The method switch test passed.  Reset relevant quantities for bdf.
                */
                *rh = rh2;
                icount = 20;
                meth = 2;
                miter = jtyp;
                pdlast = 0.;
                nq = nqm2;
                l = nq + 1;
                return;
        }			/* end if ( meth == 1 )   */
        /*
        We are currently using a bdf method, considering switching to Adams.
        Compute the step size we could have (ideally) used on this step,
        with the current (bdf) method, and also that for the Adams.
        If nq > mxordn, we consider changing to order mxordn on switching.
        Compare the two step sizes to decide whether to switch.
        The step size advantage must be at least 5/ratio = 1 to switch.
        If the step size for Adams would be so small as to cause
        roundoff pollution, we stay with bdf.
        */
        exsm = 1. / (double) l;
        if (mxordn < nq) {
                nqm1 = mxordn;
                lm1 = mxordn + 1;
                exm1 = 1. / (double) lm1;
                lm1p1 = lm1 + 1;
                dm1 = vmnorm(n, yh[lm1p1], ewt) / cm1[mxordn];
                rh1 = 1. / (1.2 * pow(dm1, exm1) + 0.0000012);
        } else {
                dm1 = dsm * (cm2[nq] / cm1[nq]);
                rh1 = 1. / (1.2 * pow(dm1, exsm) + 0.0000012);
                nqm1 = nq;
                exm1 = exsm;
        }
        rh1it = 2. * rh1;
        *pdh = pdnorm * fabs(h);
        if ((*pdh * rh1) > 0.00001)
                rh1it = sm1[nqm1] / *pdh;
        rh1 = std::min(rh1, rh1it);
        rh2 = 1. / (1.2 * pow(dsm, exsm) + 0.0000012);
        if ((rh1 * ratio) < (5. * rh2))
                return;
        alpha = std::max(0.001, rh1);
        dm1 *= pow(alpha, exm1);
        if (dm1 <= 1000. * ETA * pnorm)
                return;
        /*
        The switch test passed.  Reset relevant quantities for Adams.
        */
        *rh = rh1;
        icount = 20;
        meth = 1;
        miter = 0;
        pdlast = 0.;
        nq = nqm1;
        l = nq + 1;

}				/* end methodswitch   */


/*
   This routine returns from stoda to lsoda.  Hence freevectors() is
   not executed.
*/
void
LSODA::endstoda()
{
        double          r;
        int             i;

        r = 1. / tesco[nqu][2];
        for (i = 1; i <= n; i++)
                acor[i] *= r;
        hold = h;
        jstart = 1;

}

void
LSODA::orderswitch (double *rhup, double dsm, double *pdh,
                                                 double *rh, int *orderflag)

/*
   Regardless of the success or failure of the step, factors
   rhdn, rhsm, and rhup are computed, by which h could be multiplied
   at order nq - 1, order nq, or order nq + 1, respectively.
   In the case of a failure, rhup = 0. to avoid an order increase.
   The largest of these is determined and the new order chosen
   accordingly.  If the order is to be increased, we compute one
   additional scaled derivative.

   orderflag = 0  : no change in h or nq,
               1  : change in h but not nq,
               2  : change in both h and nq.
*/

{
        int             newq, i;
        double          exsm, rhdn, rhsm, ddn, exdn, r;

        *orderflag = 0;

        exsm = 1. / (double) l;
        rhsm = 1. / (1.2 * pow(dsm, exsm) + 0.0000012);

        rhdn = 0.;
        if (nq != 1) {
                ddn = vmnorm(n, yh[l], ewt) / tesco[nq][1];
                exdn = 1. / (double) nq;
                rhdn = 1. / (1.3 * pow(ddn, exdn) + 0.0000013);
        }
        /*
        If meth = 1, limit rh accordinfg to the stability region also.
        */
        if (meth == 1) {
                *pdh = std::max(fabs(h) * pdlast, 0.000001);
                if (l < lmax)
                        *rhup = std::min(*rhup, sm1[l] / *pdh);
                rhsm = std::min(rhsm, sm1[nq] / *pdh);
                if (nq > 1)
                        rhdn = std::min(rhdn, sm1[nq - 1] / *pdh);
                pdest = 0.;
        }
        if (rhsm >= *rhup) {
                if (rhsm >= rhdn) {
                        newq = nq;
                        *rh = rhsm;
                } else {
                        newq = nq - 1;
                        *rh = rhdn;
                        if (kflag < 0 && *rh > 1.)
                                *rh = 1.;
                }
        } else {
                if (*rhup <= rhdn) {
                        newq = nq - 1;
                        *rh = rhdn;
                        if (kflag < 0 && *rh > 1.)
                                *rh = 1.;
                } else {
                        *rh = *rhup;
                        if (*rh >= 1.1) {
                                r = el[l] / (double) l;
                                nq = l;
                                l = nq + 1;
                                yp1 = yh[l];
                                for (i = 1; i <= n; i++)
                                        yp1[i] = acor[i] * r;
                                *orderflag = 2;
                                return;
                        } else {
                                ialth = 3;
                                return;
                        }
                }
        }
        /*
        If meth = 1 and h is restricted by stability, bypass 10 percent test.
        */
        if (meth == 1) {
                if ((*rh * *pdh * 1.00001) < sm1[newq])
                        if (kflag == 0 && *rh < 1.1) {
                        ialth = 3;
                        return;
                }
        } else {
                if (kflag == 0 && *rh < 1.1) {
                        ialth = 3;
                        return;
                }
        }
        if (kflag <= -2)
                *rh = std::min(*rh, 0.2);
        /*
        If there is a change of order, reset nq, l, and the coefficients.
        In any case h is reset according to rh and the yh array is rescaled.
        Then exit or redo the step.
        */
        if (newq == nq) {
                *orderflag = 1;
                return;
        }
        nq = newq;
        l = nq + 1;
        *orderflag = 2;

}				/* end orderswitch   */


void LSODA::resetcoeff()
/*
   The el vector and related constants are reset
   whenever the order nq is changed, or at the start of the problem.
*/
{
        int             i;
        double         *ep1;

        ep1 = elco[nq];
        for (i = 1; i <= l; i++)
                el[i] = ep1[i];
        rc = rc * el[1] / el0;
        el0 = el[1];
        conit = 0.5 / (double) (nq + 2);

}



void
LSODA::successreturn (double *y, double *t, int itask, int ihit, double tcrit, int *istate)
{
        int i;
        yp1 = yh[1];
        for (i = 1; i <= n; i++)
                y[i] = yp1[i];
        *t = tn;
        if (itask == 4 || itask == 5)
                if (ihit)
                *t = tcrit;
        *istate = 2;
        illin = 0;
        freevectors();
}

/* this function does nothing. */
void
LSODA::freevectors(void)
{
}

void
LSODA::_freevectors(void)
{
        g_nyh = g_lenyh = 0;
        yh = 0; wm = 0;
        ewt = 0; savf = 0; acor = 0; ipvt = 0;
}

