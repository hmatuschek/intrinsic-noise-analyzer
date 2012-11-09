#ifndef __INA_MAXIMUMENTROPY_HH
#define __INA_MAXIMUMENTROPY_HH

#include <eigen3/Eigen/Eigen>
#include <ginac/ginac.h>

namespace iNA {
namespace Models {

class MaximumEntropyPDF
{

    double epsilon;


public:

    MaximumEntropyPDF()
        : epsilon(1.e-9)
    {



    }


    static Eigen::VectorXd
    getNonCentralMoments(const Eigen::VectorXd &centralMoments)

    {
        // Initialize with central moments
        Eigen::VectorXd moments(centralMoments.size()+1);
        // Insert one
        moments(0) = 1.; moments.tail(centralMoments.size()) = centralMoments;

        // Compute non-central moments
        for(int i=2; i<moments.size(); i++)
        {
            for(int j=0; j<i; j++)
            {
                double binomial = GiNaC::numeric(GiNaC::binomial(GiNaC::numeric(i),GiNaC::numeric(j))).to_double();
                moments(i) -= binomial*moments(j)*std::pow(-moments(1),i-j);
            }
        }

        //return moments.tail(centralMoments.size());
        return moments;
    }

    Eigen::VectorXd
    computePDFfromCentralMoments(const Eigen::VectorXd &domain, const Eigen::VectorXd &centralMoments)
    {
        Eigen::VectorXd m = getNonCentralMoments(centralMoments);
        return computePDFfromMoments(domain,m,true);
    }


    Eigen::VectorXd
    computePDFfromMoments(const Eigen::VectorXd &domain, const Eigen::VectorXd &inputMoments, bool hasZeroMoment = false)

    {

        Eigen::VectorXd moments=inputMoments;
        if(!hasZeroMoment)
        {
            moments.resize(inputMoments.size()+1);
            // Insert zero moment
            moments<<1.,inputMoments;
        }


        size_t N = moments.size();
        size_t M = 2*N-1;

        Eigen::VectorXd pdf;
        Eigen::VectorXd Gn;
        Eigen::MatrixXd Gmat(N,N);
        double entropy;

        // Initialize x^n
        Eigen::MatrixXd function(domain.size(),M);
        // Set x^0
        function.col(0) = Eigen::VectorXd::Ones(domain.size());
        // Fill the rest for n = 1...2*N
        for(size_t n=1; n<M; n++) function.col(n)=domain.cwiseProduct(function.col(n-1));

        // spacing
        double dx = domain(1)-domain(0);

        // Initialize lambda with a normal distribution
        Eigen::VectorXd lambda = Eigen::VectorXd::Zero(N);

        double sig = moments(2)-moments(1)*moments(1);
        lambda(1) = -moments(1)/sig;
        lambda(2) = 1./sig/2.;
        pdf = -function.leftCols(N)*lambda;
        for(int i=0; i<pdf.size(); i++)
            pdf(i) = exp(pdf(i));

        std::cerr << sig <<std::endl;

        // Normalize
        lambda(0) = - std::log(pdf.sum()*dx);
        pdf/=pdf.sum()*dx;

        int iter = 0;

        // Iterate...

        int maxIter = 100;

        while(iter < maxIter)
        {

            iter++;

            // Calculate pdf(x)
            //pdf = (-function.leftCols(N)*lambda).array().exp();
            pdf = -function.leftCols(N)*lambda;
            pdf = pdf.array().exp();
            //for(int i=0; i<pdf.size(); i++)  pdf(i) = exp(pdf(i));

            // Calculate G_n = integral dx x^n p(x)  of length M
            Eigen::VectorXd Gn = dx*(pdf.transpose()*function);

            // Normalize
            //lambda(0) = - log(pdf.sum()*dx);
            //pdf/=pdf.sum()*dx;

            std::cerr << "Norm:" << pdf.sum()*dx << std::endl << std::endl;


            // Calculate the entropy
            //double entropyOLD = entropy;
            entropy = lambda.transpose()*Gn.head(N);

            std::cerr << "Entropy: " << entropy << std::endl<< std::endl;
            std::cerr << lambda.transpose() << std::endl<< std::endl;

            // Generate coefficient matrix
            for(size_t i=0; i<N; i++)
                Gmat.row(i) = - Gn.segment(i,N);

            // Simple Newton step
            Eigen::VectorXd v = moments - Gn.head(N);
            Eigen::FullPivLU<Eigen::MatrixXd> lu(Gmat);
            Eigen::VectorXd delta = lu.solve(v);

            lambda += delta;

            std::cerr << delta.cwiseQuotient(lambda).norm() << std::endl<< std::endl;

            // Test for convergence
            if((delta.cwiseQuotient(lambda).norm())<epsilon || (delta).norm()<epsilon )
            {
                std::cerr << "Converged!" << std::endl;
                return pdf;
            }

        }

        std::cerr << "Maximum iterations reached" << std::endl;

        return pdf;


    }


    Eigen::VectorXd
    computePDF(const Eigen::VectorXd &domain, const Eigen::VectorXd &centralMoments)

    {


        // Compute non-central moments
        Eigen::VectorXd moments = getNonCentralMoments(centralMoments);

        size_t N = moments.size();
        size_t M = 2*N-1;

        Eigen::VectorXd pdf;
        Eigen::VectorXd Gn;
        Eigen::MatrixXd Gmat(N,N);
        double entropy;

        // Initialize x^n
        Eigen::MatrixXd function(domain.size(),M);
        // Set x^0
        function.col(0) = Eigen::VectorXd::Ones(domain.size());
        // Fill the rest for n = 1...2*N
        for(size_t n=1; n<M; n++) function.col(n)=domain.cwiseProduct(function.col(n-1));

        // Initialize lambda with a uniform distribution (should adjust to Gaussian!!!)
        Eigen::VectorXd lambda = Eigen::VectorXd::Zero(N);
        lambda(0) = std::log(domain(domain.size()-1)-domain(0));

        double dx = domain(1)-domain(0);

        int iter = 0;

        // Iterate...

        int maxIter = 100;

        while(iter < maxIter)
        {

            iter++;

            // Calculate pdf(x)
            pdf = (-function.leftCols(N)*lambda).array().exp();
            //pdf = -function.leftCols(N)*lambda;
            //for(int i=0; i<pdf.size(); i++)
            //    pdf(i) = exp(pdf(i));

            // Calculate G_n = integral dx x^n p(x)  of length M
            Eigen::VectorXd Gn = dx*(pdf.transpose()*function);
            // Calculate the entropy
            entropy = lambda.transpose()*Gn.head(N);

            std::cerr << "Norm: " << pdf.sum()*dx << std::endl << std::endl;
            std::cerr << "Entropy: " << entropy << std::endl<< std::endl;
            std::cerr << "Lambda: " << lambda.transpose() << std::endl<< std::endl;

            // Generate coefficient matrix
            for(size_t i=0; i<N; i++)
                Gmat.row(i) = - Gn.segment(i,N);

            // Simple Newton step
            Eigen::VectorXd v = moments - Gn.head(N);
            Eigen::FullPivLU<Eigen::MatrixXd> lu(Gmat);
            Eigen::VectorXd delta = lu.solve(v);
            lambda += delta/2;

            std::cerr << (delta.cwiseQuotient(lambda)).norm() << std::endl;

            // Test for convergence
            if(delta.cwiseQuotient(lambda).norm()<epsilon) break;

        }

        if(iter!=maxIter) std::cerr << "Converged" << std::endl;

        return pdf;


    }




};

}}

#endif // MAXIMUMENTROPY_HH
