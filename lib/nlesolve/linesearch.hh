#ifndef __FLUC_NLESOLVE_LINESEARCH_HH
#define __FLUC_NLESOLVE_LINESEARCH_HH

#include <iostream>


namespace Fluc {

namespace NLEsolve {


/**
 * Line search optimization method using a cubic model.
 *
 * The method uses a cubic model for which the optimal step size is calculated \cite press2007.
 *
 * @ingroup nlesolve
 */
template <class T>
void
linesearch(T* that, const Eigen::VectorXd &xold, Eigen::VectorXd &x, Eigen::VectorXd &dx,
                                    const double fold, double &f, Eigen::VectorXd &nablaf,
                                    bool &check,const double &stpmax)
{

    size_t dim = x.size();

    Eigen::VectorXd REs;

    const double ALF = 1.e-4;
    const double TOLX =  std::numeric_limits<double>::epsilon();

    double temp=0.;
    double test=0.;
    double tmplam=0;
    double rhs1=0.;
    double rhs2=0.;
    double a=0.;
    double alam=0;
    double alam2=0.;
    double alamin=0.;
    double b=0.,disc=0.,f2=0.;

    check=true;

    double norm = dx.norm();
    // scale if too big
    if(norm > stpmax) dx*=(stpmax/norm);

    double slope = nablaf.adjoint()*dx;

    if (slope >= 0.) throw ("Roundoff problem in linsearch.");
    test = 0.0;
    for (size_t i=0;i<dim;i++)
    {
        temp=std::abs(dx(i))/std::max(abs(i),1);
        if (temp > test) test = temp;
    }

    alamin = TOLX/test;

    // first do newton step
    alam   = 1.0;
    for(;;){

        x = xold+alam*dx;

        // evaluate rate equations
        that->getREs(x,REs);

        f = 0.5*(REs.squaredNorm());

        if (alam < alamin) {
           x=xold;
           check=false;
           return;
           // linesearch failed
        }
        else if (f <= (fold + ALF*alam*slope) )
        {
            // success test curvature
            return;
        }
        else
        {
            if (alam == 1.0)
                tmplam = -slope/(2.*(f-fold-slope));
            else {
               rhs1 = f-fold-alam*slope;
               rhs2 = f2-fold-alam2*slope;
               a = (rhs1/(alam*alam)-rhs2/(alam2*alam2))/(alam-alam2);
               b = (-alam2*rhs1/(alam*alam)+alam*rhs2/(alam2*alam2))/(alam-alam2);
               if (a==0.) tmplam = -slope/(2.*b);
               else{
                  disc=b*b-3.*a*slope;
                  if (disc<0.) tmplam = 0.5*alam;
                  else if (b<=0.) tmplam=(-b+std::sqrt(disc))/(3.*a);
                  else tmplam=-slope/(b+std::sqrt(disc));
               }
               if (tmplam>0.5*alam) tmplam = 0.5*alam;
            }
        }

        alam2 = alam;
        f2 = f;
        alam = std::max(tmplam,0.1*alam);

    }

}


/**
 * Damped line search method.
 *
 * @ingroup nlesolve
 *
 * This method simply halfens the step size.
 */

template<typename T>
void
linesearch2(T* that, const Eigen::VectorXd &xold, Eigen::VectorXd &x, Eigen::VectorXd &dx,
                                    const double fold, double &f, Eigen::VectorXd &nablaf,
                                    bool &check, double &epsilon, double &stpmax)
{

    const double alpha = 1.e-5;

    // first try Newton step
    double lambda = 1.;

    int n=0;

    Eigen::VectorXd REstemp;

    // loop over until f has sufficiently decreased
    while(f > (1.-lambda*alpha)*(1.-lambda*alpha)*fold)
    {


        n++;

        x = xold+lambda*dx;

        that->getREs(x,REstemp);

        f = 0.5*REstemp.squaredNorm();

        // take half step size
        lambda /= 2.;

        if(n>32)
        {
            std::cerr<<"linesearch failed"<<std::endl;
            check =  false;
            return;
        }

    }

}

}} // close namespaces

#endif // __FLUC_NLESOLVE_LINESEARCH_HH
