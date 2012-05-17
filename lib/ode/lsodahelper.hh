
/************
 * idamax.c *
 ************/

//void fevaldgl (double x, double y[], double yd[],
//	int n, char *fname, int argn, header *args, long size,
//	int columns);


static int idamax(int n, double *dx, int incx)

/* Purpose : Find largest component of double vector dx


   --- Input ---

   n    : number of elements in input vector
   dx   : double vector with n+1 elements, dx[0] is not used
   incx : storage spacing between elements of dx


   --- Output ---

   idamax : smallest index, 0 if n <= 0


   Find smallest index of maximum magnitude of dx.
   idamax = first i, i=1 to n, to minimize fabs( dx[1-incx+i*incx] ).

*/

{
        double          dmax, xmag;
        int             i, ii, xindex;

        xindex = 0;
        if (n <= 0)
                return xindex;
        xindex = 1;
        if (n <= 1 || incx <= 0)
                return xindex;

        /* Code for increments not equal to 1.   */

        if (incx != 1) {
                dmax = fabs(dx[1]);
                ii = 2;
                for (i = 1 + incx; i <= n * incx; i = i + incx) {
                        xmag = fabs(dx[i]);
                        if (xmag > dmax) {
                                xindex = ii;
                                dmax = xmag;
                        }
                        ii++;
                }
                return xindex;
        }
        /* Code for increments equal to 1.  */

        dmax = fabs(dx[1]);
        for (i = 2; i <= n; i++) {
                xmag = fabs(dx[i]);
                if (xmag > dmax) {
                        xindex = i;
                        dmax = xmag;
                }
        }
        return xindex;

}

/***********
 * dscal.c *
 ***********/

void dscal (int n, double da, double *dx, int incx)

/* Purpose : scalar vector multiplication

   dx = da * dx


   --- Input ---

   n    : number of elements in input vector
   da   : double scale factor
   dx   : double vector with n+1 elements, dx[0] is not used
   incx : storage spacing between elements of dx


   --- Output ---

   dx = da * dx, unchanged if n <= 0


   For i = 0 to n-1, replace dx[1+i*incx] with
   da * dx[1+i*incx].

*/

{
        int             m, i;

        if (n <= 0)
                return;

        /* Code for increments not equal to 1.  */

        if (incx != 1) {
                for (i = 1; i <= n * incx; i = i + incx)
                        dx[i] = da * dx[i];
                return;
        }
        /* Code for increments equal to 1.  */

        /* Clean-up loop so remaining vector length is a multiple of 5.  */

        m = n % 5;
        if (m != 0) {
                for (i = 1; i <= m; i++)
                        dx[i] = da * dx[i];
                if (n < 5)
                        return;
        }
        for (i = m + 1; i <= n; i = i + 5) {
                dx[i] = da * dx[i];
                dx[i + 1] = da * dx[i + 1];
                dx[i + 2] = da * dx[i + 2];
                dx[i + 3] = da * dx[i + 3];
                dx[i + 4] = da * dx[i + 4];
        }
        return;

}

/**********
 * ddot.c *
 **********/

static double ddot (int n, double *dx, int incx, double *dy, int incy)

/*
   Purpose : Inner product dx . dy


   --- Input ---

   n    : number of elements in input vector(s)
   dx   : double vector with n+1 elements, dx[0] is not used
   incx : storage spacing between elements of dx
   dy   : double vector with n+1 elements, dy[0] is not used
   incy : storage spacing between elements of dy


   --- Output ---

   ddot : dot product dx . dy, 0 if n <= 0


   ddot = sum for i = 0 to n-1 of
   dx[lx+i*incx] * dy[ly+i*incy] where lx = 1 if
   incx >= 0, else lx = (-incx)*(n-1)+1, and ly
   is defined in a similar way using incy.

*/

{
        double          dotprod;
        int             ix, iy, i, m;

        dotprod = 0.;
        if (n <= 0)
                return dotprod;

        /* Code for unequal or nonpositive increments.  */

        if (incx != incy || incx < 1) {
                ix = 1;
                iy = 1;
                if (incx < 0)
                        ix = (-n + 1) * incx + 1;
                if (incy < 0)
                        iy = (-n + 1) * incy + 1;
                for (i = 1; i <= n; i++) {
                        dotprod = dotprod + dx[ix] * dy[iy];
                        ix = ix + incx;
                        iy = iy + incy;
                }
                return dotprod;
        }
        /* Code for both increments equal to 1.  */

        /* Clean-up loop so remaining vector length is a multiple of 5.  */

        if (incx == 1) {
                m = n % 5;
                if (m != 0) {
                        for (i = 1; i <= m; i++)
                                dotprod = dotprod + dx[i] * dy[i];
                        if (n < 5)
                                return dotprod;
                }
                for (i = m + 1; i <= n; i = i + 5)
                        dotprod = dotprod + dx[i] * dy[i] + dx[i + 1] * dy[i + 1] +
                        dx[i + 2] * dy[i + 2] + dx[i + 3] * dy[i + 3] +
                        dx[i + 4] * dy[i + 4];
                return dotprod;
        }
        /* Code for positive equal nonunit increments.   */

        for (i = 1; i <= n * incx; i = i + incx)
                dotprod = dotprod + dx[i] * dy[i];
        return dotprod;

}

/***********
 * daxpy.c *
 ***********/

/*
From tam@dragonfly.wri.com Wed Apr 24 15:48:31 1991
Return-Path: <tam>
Date: Wed, 24 Apr 91 17:48:43 CDT
From: tam@dragonfly.wri.com
To: whitbeck@sanjuan.wrc.unr.edu
*/

static void daxpy (int n, double da, double *dx, int incx, double *dy, int incy)

/*
   Purpose : To compute

   dy = da * dx + dy


   --- Input ---

   n    : number of elements in input vector(s)
   da   : double scalar multiplier
   dx   : double vector with n+1 elements, dx[0] is not used
   incx : storage spacing between elements of dx
   dy   : double vector with n+1 elements, dy[0] is not used
   incy : storage spacing between elements of dy


   --- Output ---

   dy = da * dx + dy, unchanged if n <= 0


   For i = 0 to n-1, replace dy[ly+i*incy] with
   da*dx[lx+i*incx] + dy[ly+i*incy], where lx = 1
   if  incx >= 0, else lx = (-incx)*(n-1)+1 and ly is
   defined in a similar way using incy.

*/

{
        int             ix, iy, i, m;

        if (n < 0 || da == 0.)
                return;

        /* Code for nonequal or nonpositive increments.  */

        if (incx != incy || incx < 1) {
                ix = 1;
                iy = 1;
                if (incx < 0)
                        ix = (-n + 1) * incx + 1;
                if (incy < 0)
                        iy = (-n + 1) * incy + 1;
                for (i = 1; i <= n; i++) {
                        dy[iy] = dy[iy] + da * dx[ix];
                        ix = ix + incx;
                        iy = iy + incy;
                }
                return;
        }
        /* Code for both increments equal to 1.   */

        /* Clean-up loop so remaining vector length is a multiple of 4.  */

        if (incx == 1) {
                m = n % 4;
                if (m != 0) {
                        for (i = 1; i <= m; i++)
                                dy[i] = dy[i] + da * dx[i];
                        if (n < 4)
                                return;
                }
                for (i = m + 1; i <= n; i = i + 4) {
                        dy[i] = dy[i] + da * dx[i];
                        dy[i + 1] = dy[i + 1] + da * dx[i + 1];
                        dy[i + 2] = dy[i + 2] + da * dx[i + 2];
                        dy[i + 3] = dy[i + 3] + da * dx[i + 3];
                }
                return;
        }
        /* Code for equal, positive, nonunit increments.   */

        for (i = 1; i <= n * incx; i = i + incx)
                dy[i] = da * dx[i] + dy[i];
        return;

}

/***********
 * dgesl.c *
 ***********/

static void dgesl (double **a, int n, int *ipvt, double *b, int job)

/*
   Purpose : dgesl solves the linear system
   a * x = b or Transpose(a) * x = b
   using the factors computed by dgeco or degfa.


   On Entry :

      a    : double matrix of dimension ( n+1, n+1 ),
             the output from dgeco or dgefa.
             The 0-th row and column are not used.
      n    : the row dimension of a.
      ipvt : the pivot vector from degco or dgefa.
      b    : the right hand side vector.
      job  : = 0       to solve a * x = b,
             = nonzero to solve Transpose(a) * x = b.


   On Return :

      b : the solution vector x.


   Error Condition :

      A division by zero will occur if the input factor contains
      a zero on the diagonal.  Technically this indicates
      singularity but it is often caused by improper argments or
      improper setting of the pointers of a.  It will not occur
      if the subroutines are called correctly and if dgeco has
      set rcond > 0 or dgefa has set info = 0.


   BLAS : daxpy, ddot
*/

{
        int             k, j;
        double          t;

        //int nm1 = n - 1;

        /*
        Job = 0, solve a * x = b.
        */
        if (job == 0) {
                /*
                First solve L * y = b.
                */
                for (k = 1; k <= n; k++) {
                        t = ddot(k - 1, a[k], 1, b, 1);
                        b[k] = (b[k] - t) / a[k][k];
                }
                /*
                Now solve U * x = y.
                */
                for (k = n - 1; k >= 1; k--) {
                        b[k] = b[k] + ddot(n - k, a[k] + k, 1, b + k, 1);
                        j = ipvt[k];
                        if (j != k) {
                                t = b[j];
                                b[j] = b[k];
                                b[k] = t;
                        }
                }
                return;
        }
        /*
        Job = nonzero, solve Transpose(a) * x = b.

        First solve Transpose(U) * y = b.
        */
        for (k = 1; k <= n - 1; k++) {
                j = ipvt[k];
                t = b[j];
                if (j != k) {
                        b[j] = b[k];
                        b[k] = t;
                }
                daxpy(n - k, t, a[k] + k, 1, b + k, 1);
        }
        /*
        Now solve Transpose(L) * x = y.
        */
        for (k = n; k >= 1; k--) {
                b[k] = b[k] / a[k][k];
                t = -b[k];
                daxpy(k - 1, t, a[k], 1, b, 1);
        }

}

/***********
 * dgefa.c *
 ***********/

void dgefa (double **a, int n, int *ipvt, int *info)

/*
   Purpose : dgefa factors a double matrix by Gaussian elimination.

   dgefa is usually called by dgeco, but it can be called directly
   with a saving in time if rcond is not needed.
   (Time for dgeco) = (1+9/n)*(time for dgefa).

   This c version uses algorithm kji rather than the kij in dgefa.f.
   Note that the fortran version input variable lda is not needed.


   On Entry :

      a   : double matrix of dimension ( n+1, n+1 ),
            the 0-th row and column are not used.
            a is created using NewDoubleMatrix, hence
            lda is unnecessary.
      n   : the row dimension of a.

   On Return :

      a     : a lower triangular matrix and the multipliers
              which were used to obtain it.  The factorization
              can be written a = L * U where U is a product of
              permutation and unit upper triangular matrices
              and L is lower triangular.
      ipvt  : an n+1 integer vector of pivot indices.
      *info : = 0 normal value,
              = k if U[k][k] == 0.  This is not an error
                condition for this subroutine, but it does
                indicate that dgesl or dgedi will divide by
                zero if called.  Use rcond in dgeco for
                a reliable indication of singularity.

                Notice that the calling program must use &info.

   BLAS : daxpy, dscal, idamax
*/

{
        int             j, k, i;
        double          t;

        /* Gaussian elimination with partial pivoting.   */

        *info = 0;
        for (k = 1; k <= n - 1; k++) {
                /*
                Find j = pivot index.  Note that a[k]+k-1 is the address of
                the 0-th element of the row vector whose 1st element is a[k][k].
                */
                j = idamax(n - k + 1, a[k] + k - 1, 1) + k - 1;
                ipvt[k] = j;
                /*
                Zero pivot implies this row already triangularized.
                */
                if (a[k][j] == 0.) {
                        *info = k;
                        continue;
                }
                /*
                Interchange if necessary.
                */
                if (j != k) {
                        t = a[k][j];
                        a[k][j] = a[k][k];
                        a[k][k] = t;
                }
                /*
                Compute multipliers.
                */
                t = -1. / a[k][k];
                dscal(n - k, t, a[k] + k, 1);
                /*
                Column elimination with row indexing.
                */
                for (i = k + 1; i <= n; i++) {
                        t = a[i][j];
                        if (j != k) {
                                a[i][j] = a[i][k];
                                a[i][k] = t;
                        }
                        daxpy(n - k, t, a[k] + k, 1, a[i] + k, 1);
                }
        }			/* end k-loop  */

        ipvt[n] = n;
        if (a[n][n] == 0.)
                *info = n;

}
