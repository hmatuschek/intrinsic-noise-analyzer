#include <math.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define ETA 2.2204460492503131e-16

using namespace std;

/*
  This is a C version of the LSODA library. I acquired the original
  source code from this web page:

    http://www.ccl.net/cca/software/SOURCES/C/kinetics2/index.shtml

  I merged several C files into one and added a simpler interface. I
  also made the array start from zero in functions called by lsoda(),
  and fixed two minor bugs: a) small memory leak in freevectors(); and
  b) misuse of lsoda() in the example.

  The original source code came with no license or copyright
  information. I now release this file under the MIT/X11 license. All
  authors' notes are kept in this file.

  - Heng Li <lh3lh3@gmail.com>
 */

/* The MIT License

   Copyright (c) 2009 Genome Research Ltd (GRL).

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

/* Contact: Heng Li <lh3@sanger.ac.uk> */

/***********
 * lsoda.c *
 ***********/

/*
From tam@dragonfly.wri.com Wed Apr 24 01:35:52 1991
Return-Path: <tam>
Date: Wed, 24 Apr 91 03:35:24 CDT
From: tam@dragonfly.wri.com
To: whitbeck@wheeler.wrc.unr.edu
Subject: lsoda.c
Cc: augenbau@sparc0.brc.uconn.edu


I'm told by Steve Nichols at Georgia Tech that you are interested in
a stiff integrator.  Here's a translation of the fortran code LSODA.

Please note
that there is no comment.  The interface is the same as the FORTRAN
code and I believe the documentation in LSODA will suffice.
As usual, a free software comes with no guarantee.

Hon Wah Tam
Wolfram Research, Inc.
tam@wri.com
*/

class LSODEA {

 LSODEA();


 virtual void fevaldgl (double x, double y[], double yd[],
         int n)=0;

 void lsoda (int neq, double *y, double *t, double tout,
                    int itol, double *rtol, double *atol,
                    int itask, int *istate, int iopt, int jt   );

 void     stoda(int neq, double *y);
 void     correction(int neq, double *y, 
						   int *corflag, double pnorm, double *del, double *delp, double *told,
                                                   int *ncf, double *rh, int *m);
 void     prja(int neq, double *y);
 void     terminate(int *istate);
 void     terminate2(double *y, double *t);
 void     successreturn(double *y, double *t, int itask, int ihit, 
							  double tcrit, int *istate);
 void     freevectors(void); /* this function does nothing */
 void     _freevectors(void);
 void     ewset(int itol, double *rtol, double *atol, double *ycur);
 void     resetcoeff(void);
 void     solsy(double *y);
 void     endstoda(void);
 void     orderswitch(double *rhup, double dsm, double *pdh, double *rh, int *orderflag);
 void     intdy(double t, int k, double *dky, int *iflag);
 void     corfailure(double *told, double *rh, int *ncf, int *corflag);
 void     methodswitch(double dsm, double pnorm, double *pdh, double *rh);
 void     cfode(int meth);
 void     scaleh(double *rh, double *pdh);
 double   fnorm(int n, double **a, double *w);
 double   vmnorm(int n, double *v, double *w);

 int      g_nyh, g_lenyh;

/* newly added static variables */

 int      ml, mu, imxer;
 int      mord[3];
 double   sqrteta, *yp1, *yp2;
 double   sm1[13];

/* static variables for lsoda() */

 double   ccmax, el0, h, hmin, hmxi, hu, rc, tn;
 int      illin, init, mxstep, mxhnil, nhnil, ntrep, nslast, nyh, ierpj, iersl,
                jcur, jstart, kflag, l, meth, miter, maxord, maxcor, msbp, mxncf, n, nq, nst,
                nfe, nje, nqu;
 double   tsw, pdnorm;
 int      ixpr, jtyp, mused, mxordn, mxords;

/* no static variable for prja(), solsy() */
/* static variables for stoda() */

 double   conit, crate, el[14], elco[13][14], hold, rmax, tesco[13][4];
 int      ialth, ipup, lmax, nslp;
 double   pdest, pdlast, ratio, cm1[13], cm2[6];
 int      icount, irflag;

/* static variables for various vectors and the Jacobian. */

 double  **yh, **wm, *ewt, *savf, *acor;
 int     *ipvt;

 int lsoda_warning;

};

