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
        : epsilon(1e-12)
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
        for(int i=1; i<=centralMoments.size(); i++)
            for(int j=0; j<i; j++)
                moments(i) -= GiNaC::numeric(GiNaC::binomial(GiNaC::numeric(i),GiNaC::numeric(j))).to_double()*moments(j)*std::pow(-moments(1),i-j);

        return moments.tail(centralMoments.size());

    }



    Eigen::VectorXd
    computePDF(const Eigen::VectorXd &domain, const Eigen::VectorXd &centralMoments)

    {

        size_t N = centralMoments.size();
        size_t M = 2*N-1;

        Eigen::VectorXd pdf;
        Eigen::VectorXd Gn;
        Eigen::MatrixXd Gmat(N,N);
        double entropy;

        // Compute non-central moments
        Eigen::VectorXd moments = getNonCentralMoments(centralMoments);

        // Initialize x^n
        Eigen::MatrixXd fin(domain.size(),M);
        fin.col(0) = Eigen::VectorXd::Ones(domain.size());
        for(size_t n=1; n<M; n++) fin.col(n)=domain.cwiseProduct(fin.col(n-1));

        // Initialize lambda with a uniform distribution (should adjust to Gaussian!!!)
        Eigen::VectorXd lambda = Eigen::VectorXd::Zero(N);
        lambda(0) = std::log(domain(domain.size()-1)-domain(0));

        double dx = domain(1)-domain(0);

        int iter = 0;

        // Iterate...

        while(1)
        {

            iter++;

            // Calculate pdf(x)
            pdf = (-fin.leftCols(N)*lambda).array().exp();

            std::cout << pdf.transpose() << std::endl<< std::endl;

            // Calculate G_n of length M
            Eigen::VectorXd Gn = dx*(pdf.transpose()*fin);

            // Calculate the entropy
            entropy = lambda.transpose()*Gn.head(N);

            std::cerr << "Entropy: " << entropy << std::endl<< std::endl;
            std::cerr << lambda.transpose() << std::endl<< std::endl;

            // Generate coefficient matrix
            for(size_t i=0; i<N; i++)
                Gmat.row(i) = - Gn.segment(i,N);

            // Simple Newton step
            Eigen::VectorXd v = moments - Gn.head(N);
            Eigen::VectorXd delta = Gmat.lu().solve(v);
            lambda += delta;

            // Test for convergence
            double test=0.;
            for(size_t i=0; i<N; i++)
                if(abs(lambda(i)/delta(i))>test)
                    test = abs(lambda(i)/delta(i));
            if(test<epsilon)
            {
                std::cerr << "Converged" << std::endl;
                break;
            }

            double entropyNEW = lambda.transpose()*Gn.head(N);

            if(iter>2 && (abs(entropyNEW-entropy)/entropyNEW)<epsilon) break;

        }

        std::cerr << "Converged" << std::endl;

        return pdf;


    }



};

}}

#endif // MAXIMUMENTROPY_HH
