#ifndef __FLUC_NEWTONRAPHSON_HH
#define __FLUC_NEWTONRAPHSON_HH

#include <iostream>
#include <eigen3/Eigen/Eigen>

#include "linesearch.hh"

namespace Fluc {

namespace NLEsolve {

/**
 * Gradient descent method.
 *
 * @ingroup nlesolve
 */

template<typename T>
void
graddescent(T* that, Eigen::VectorXd &conc, bool &check, int &maxiter, double &epsilon)
{

    const double TOLF = 1.e-8,TOLMIN=1.e-12,STPMX=100.;
    const double TOLX = std::numeric_limits<double>::epsilon();

    Eigen::VectorXd REs;
    Eigen::MatrixXd JacobianM;

    double test,den;
    double f,fold;

    Eigen::VectorXd conc_old;
    Eigen::VectorXd rhs;

    Eigen::VectorXd nablaf;
    Eigen::VectorXd dx;

    // evaluate rate equations
    that->getREs(conc,REs);

    // construct Jacobian matrix
    that->getJacobianMatrix(JacobianM);

    // dimension
    size_t dim = conc.size();

    // calc max step
    double stpmax=STPMX*std::max(conc.norm(),double(dim));

    double kmax = maxiter;

    for(int k=0;k<kmax;k++)
    {

        maxiter = k;

        // compute f to minimize
        f = .5*(REs.squaredNorm());

        // nabla f
        nablaf = JacobianM.transpose()*REs;

        // store old concentrations
        conc_old = conc;
        // store f
        fold = f;

        // steepest descent
        dx = -nablaf;

        // returns new conc and f
        linesearch(that, conc_old, conc, dx, fold, f, nablaf, check, epsilon, stpmax);

        // if linesearch failed try with full pivoting.
        if(!check)
        {
            dx = JacobianM.fullPivLu().solve(-REs);
            linesearch(that, conc_old, conc, dx, fold, f, nablaf, check, epsilon, stpmax);
        }

        // evaluate rate equations
        that->getREs(conc,REs);

        // construct Jacobian matrix
        that->getJacobianMatrix(JacobianM);

        // test for convergence of REs

        // max norm
        test = REs.lpNorm<Eigen::Infinity>();

        if ( test < TOLF )
        {
           //std::cerr<<"convergence in REs"<<std::endl;
           check = true;
           return;
        }


        // check for spurious convergence of nablaf = 0
        double temp;
        if (!check) {
           test = 0.0;
           den = std::max(f,0.5*double(dim));
           for(size_t i=0; i<dim;i++)
           {
               temp = std::abs(nablaf(i))*std::max(std::abs(conc(i)),1.)/den;
               if (temp > test) test = temp;
           }


           check = !(test < TOLMIN);
           std::cerr<<"Spurious convergence of nablaf!"<<std::endl;
           return;
        }

        // test for convergence of dx
        test = 0.;
        for(size_t i=0;i<dim;i++)
        {
            temp = (std::abs(conc(i)-conc_old(i)))/std::max(conc(i),1.);
            if (temp > test) test = temp;
        }
        if (test < TOLX)
        {
            //std::cerr<<"convergence of dx"<<std::endl;
            return;
        }

    } // next newton iteration

    check=false;

}

}

}

#endif
