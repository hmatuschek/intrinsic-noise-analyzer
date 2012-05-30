#ifndef __FLUC_NEWTONRAPHSON_HH
#define __FLUC_NEWTONRAPHSON_HH

#include <iostream>
#include <eigen3/Eigen/Eigen>

#include "Uncmin.h"
#include "linesearch.hh"

#include "models/linearnoiseapproximation.hh"

namespace Fluc {

// Vector and Matrix classes from Eigen
typedef Eigen::VectorXd dvec;
typedef Eigen::MatrixXd dmat;

namespace Models {

// Class to use as function template argument to Uncmin for this problem
class REwrapper
{

public:

    REwrapper(LinearNoiseApproximation &that) : model(that)
    {

    };

    ~REwrapper();

        // Function to minimize
        double f_to_minimize(dvec &conc){
            Eigen::VectorXd REs;
            // evaluate rate equations
            model.getREs(conc,REs);
            return .5*(REs.squaredNorm());
        };

        // Gradient of function to minimize
        void gradient(dvec &conc, dvec &nablaf){

            Eigen::VectorXd REs;
            Eigen::MatrixXd JacobianM;
            model.getREs(conc,REs);
            // construct Jacobian matrix
            model.getJacobianMatrix(JacobianM);
            nablaf = JacobianM.transpose()*REs;
        }

        // Hessian not used in this example
        void hessian(dvec /* &x */, dmat /* &h */) {}

        // Indicates analytic gradient is used
        int HasAnalyticGradient() {return 1;}

        // Indicates analytic hessian is not used
        int HasAnalyticHessian() {return 0;}

        // Any real vector will contain valid parameter values
        int ValidParameters(dvec &x) {return 1;}

        // Dimension of problem
        int dim() {return model.numIndSpecies();}

private:
        LinearNoiseApproximation &model;
};

}

namespace NLEsolve {


/**
 * Newton-Raphson method.
 *
 * @ingroup nlesolve
 */

template<typename T>
void
newton(T* that, Eigen::VectorXd &conc, bool &check, size_t &maxiter, const double &epsilon)
{

    const double TOLF = epsilon,TOLMIN=1.e-12,STPMX=100.;
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

    Models::REwrapper minimizer(&that);
    Uncmin<dvec, dmat, Models::REwrapper > min(&minimizer);

    for(int k=0;k<kmax;k++)
    {

        maxiter = k;

        // compute f to minimize
//        f = .5*(REs.squaredNorm());

        // calculate steepest descent direction
//        nablaf = JacobianM.transpose()*REs;

        // store old concentrations
        conc_old = conc;

        min.SetMethod(2);

        check = min.Minimize(conc_old,conc,f,nablaf);

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

           /* @todo check this return */
           check = !(test < TOLMIN);
           //std::cerr<<"Spurious convergence of nablaf!"<<std::endl;
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
