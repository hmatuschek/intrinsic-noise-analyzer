#ifndef __INA_MAXIMUMENTROPY_HH
#define __INA_MAXIMUMENTROPY_HH

#include <eigen3/Eigen/Eigen>
#include <ginac/ginac.h>
#include "nlesolve/minpack.hh"
#include "math.hh"

namespace iNA {
namespace Models {

class MaximumEntropyPDF
{

    double epsilon;


public:

    MaximumEntropyPDF()
        : epsilon(1.e-10)
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
        //return computePDFfromMoments(domain,m,true);
        return computePDF(domain,m);
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

        // Normalize
        lambda(0) = - std::log(pdf.sum()*dx);
        pdf/=pdf.sum()*dx;

        int iter = 0;

        // Iterate...

        int maxIter = 300;



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
            lambda(0) = - log(pdf.sum()*dx);
            pdf/=pdf.sum()*dx;

            std::cerr << "Norm:" << pdf.sum()*dx << std::endl << std::endl;


            // Calculate the entropy
            //double entropyOLD = entropy;
            entropy = lambda.transpose()*Gn.head(N);

            std::cerr << "Entropy: " << entropy << std::endl<< std::endl;
            //std::cerr << lambda.transpose() << std::endl<< std::endl;

            // Generate coefficient matrix
            for(size_t i=0; i<N; i++)
                Gmat.row(i) = - Gn.segment(i,N);

            // Simple Newton step
            Eigen::VectorXd v = moments - Gn.head(N);
            Eigen::FullPivLU<Eigen::MatrixXd> lu(Gmat);
            Eigen::VectorXd delta = lu.solve(v);

            lambda += delta;

            //std::cerr << delta.cwiseQuotient(lambda).norm() << std::endl<< std::endl;

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
    computePDF(const Eigen::VectorXd &domain, const Eigen::VectorXd &nonCentralMoments)

    {


        // Compute non-central moments
        //Eigen::VectorXd moments = getNonCentralMoments(centralMoments);
        Eigen::VectorXd moments = nonCentralMoments;

        // domain length
        double L = domain(domain.size()-1)-domain(0);
        // Normalize them
        for(int i=0; i<moments.size(); i++)
        {
          moments(i)*=pow(L,-i);
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
        for(size_t n=1; n<M; n++) function.col(n)=domain.cwiseProduct(function.col(n-1))/L;

        // Initialize lambda with a uniform distribution (should adjust to Gaussian!!!)
        Eigen::VectorXd lambda = Eigen::VectorXd::Zero(N);

        double dx = (domain(1)-domain(0))/L;

        int iter = 0;

        // Iterate...

        int maxIter = 10000;

        while(iter < maxIter)
        {

            iter++;

            // Calculate pdf(x)
            //pdf = (-function.leftCols(N)*lambda).array().exp();
            pdf = -function.leftCols(N)*lambda;
            for(int i=0; i<pdf.size(); i++)
                pdf(i) = exp(pdf(i));

            // Calculate G_n = integral dx x^n p(x)  of length M
            Eigen::VectorXd Gn = dx*(pdf.transpose()*function);
            // Calculate the entropy
            entropy = lambda.transpose()*Gn.head(N);

            std::cerr << "Norm: " << pdf.sum()*dx << std::endl << std::endl;
            std::cerr << "Entropy: " << entropy << std::endl<< std::endl;
            std::cerr << "Moments computed: " << Gn.head(N).transpose() << std::endl<< std::endl;
            std::cerr << "Moments expected: " << moments.transpose() << std::endl<< std::endl;

            // Generate coefficient matrix
            for(size_t i=0; i<N; i++)
                Gmat.row(i) = - Gn.segment(i,N);

            // Simple Newton step
            Eigen::VectorXd v = moments - Gn.head(N);
            Eigen::FullPivLU<Eigen::MatrixXd> lu(Gmat);
            Eigen::VectorXd delta = lu.solve(v);
            lambda += delta;

            std::cerr << " Rel. error:" << delta.cwiseQuotient(lambda).cwiseAbs().maxCoeff() << std::endl << std::endl;
            //if(delta.cwiseQuotient(lambda).norm()<epsilon) break;
            if(delta.cwiseQuotient(lambda).cwiseAbs().maxCoeff()<epsilon) break;

        }

        if(iter!=maxIter) std::cerr << "Converged" << std::endl;

        return pdf/L;


    }

    // Iterative method
    Eigen::VectorXd
    computeIt(const Eigen::VectorXd &domain, const Eigen::VectorXd &nonCentralMoments)

    {


        // Compute non-central moments
        //Eigen::VectorXd moments = getNonCentralMoments(centralMoments);
        Eigen::VectorXd moments = nonCentralMoments;

        // domain length
        double L = domain(domain.size()-1)-domain(0);
        // Normalize them
        for(int i=0; i<moments.size(); i++)
        {
          moments(i)*=pow(L,-i);
        }

        //size_t N = moments.size();


        Eigen::VectorXd pdf;


        // Initialize lambda with a uniform distribution (should adjust to Gaussian!!!)
        Eigen::VectorXd lambda = Eigen::VectorXd::Zero(2);

        for(size_t N=3; N<=(unsigned)moments.size(); N++)
        {

        Eigen::VectorXd temp(N);
        temp << lambda,0.;
        lambda=temp;

        size_t M = 2*N-1;

        Eigen::VectorXd Gn;
        Eigen::MatrixXd Gmat(N,N);
        double entropy;

        // Initialize x^n
        Eigen::MatrixXd function(domain.size(),M);
        // Set x^0
        function.col(0) = Eigen::VectorXd::Ones(domain.size());
        // Fill the rest for n = 1...2*N
        for(size_t n=1; n<M; n++) function.col(n)=domain.cwiseProduct(function.col(n-1))/L;

        double dx = (domain(1)-domain(0))/L;

        int iter = 0;

        // Iterate...

        int maxIter = 10000;

        while(iter < maxIter)
        {

            iter++;

            // Calculate pdf(x)
            //pdf = (-function.leftCols(N)*lambda).array().exp();
            pdf = -function.leftCols(N)*lambda;
            for(int i=0; i<pdf.size(); i++)
                pdf(i) = exp(pdf(i));

            // Calculate G_n = integral dx x^n p(x)  of length M
            Eigen::VectorXd Gn = dx*(pdf.transpose()*function);
            // Calculate the entropy
            entropy = lambda.transpose()*Gn.head(N);

            std::cerr << "Norm: " << pdf.sum()*dx << std::endl << std::endl;
            std::cerr << "Entropy: " << entropy << std::endl<< std::endl;
            std::cerr << "Moments computed: " << Gn.head(N).transpose() << std::endl;
            std::cerr << "Moments expected: " << moments.head(N).transpose() << std::endl<< std::endl;


            // Generate coefficient matrix
            for(size_t i=0; i<N; i++)
                Gmat.row(i) = - Gn.segment(i,N);

            // Simple Newton step
            Eigen::VectorXd v = moments.head(N) - Gn.head(N);

            Eigen::FullPivLU<Eigen::MatrixXd> lu(Gmat);
            Eigen::VectorXd delta = lu.solve(v);


            lambda += delta;


            std::cerr << " Rel. error:" << delta.cwiseQuotient(lambda).cwiseAbs().maxCoeff() << std::endl << std::endl;
            //if(delta.cwiseQuotient(lambda).norm()<epsilon) break;
            if(delta.cwiseQuotient(lambda).cwiseAbs().maxCoeff()<epsilon) break;

        }

        if(iter!=maxIter) std::cerr << "Converged in " << N << std::endl;

        } // end N
        return pdf/L;


    }


    Eigen::VectorXd
    computePDF2(const Eigen::VectorXd &domain, const Eigen::VectorXd &nonCentralMoments, double epsilon=0.0001)

    {

        // domain length
        double L = domain(domain.size()-1)-domain(0);

        // Compute non-central moments
        Eigen::VectorXd moments = nonCentralMoments;//getNonCentralMoments(centralMoments);
        // Normalize them
        for(int i=1; i<moments.size(); i++)
          moments(i)*=pow(L,-i);

        size_t M = moments.size();
        size_t N = domain.size();

        double maxError;
        int it;

        Eigen::VectorXd pdf;
        Eigen::VectorXd Gn;
        double entropy;

        // Initialize x^n
        Eigen::MatrixXd function(N,M);
        // Fill the rest for n = 0...M-1
        for(size_t i=0; i<N; i++)
          for(size_t j=0; j<M; j++)
            function(i,j) = pow(domain(i)/L,j);

        // Initialize lambda with a uniform distribution (should adjust to Gaussian!!!)
        Eigen::VectorXd lambda = Eigen::VectorXd::Zero(M);
        //lambda(0) = -std::log(domain(domain.size()-1)-domain(0));

        double dx = (domain(1)-domain(0))/L;

        int iter = 0;

        // Iterate...

        int maxIter = 1000000*M;

        while(iter < maxIter)
        {

            // Calculate pdf(x)
            pdf = (function*lambda);
            for(size_t i=0; i<N; i++)
                pdf(i) = exp(pdf(i)-1);

            // Calculate G_n = integral dx x^n p(x)  of length M
            Gn = dx*(pdf.transpose()*function);

            // Test convergence
            maxError = 0.;
            for(size_t i=0; i<M; i++)
              if(std::abs(Gn(i)-moments(i))>maxError) maxError=std::abs(Gn(i)-moments(i));


            if(maxError<epsilon) break;

            it = iter % M;
            iter++;

            if(iter %10000==0){

              std::cerr << "Relative error:" << maxError << std::endl<< std::endl;

            std::cerr << "Norm: " << pdf.sum()*dx << std::endl << std::endl;
            std::cerr << "Entropy: " << entropy << std::endl<< std::endl;

            std::cerr << "Moments computed: " << Gn.head(M).transpose() << std::endl;
            std::cerr << "Moments expected: " << moments.head(M).transpose() << std::endl<< std::endl;
            }



            double temp=0.;
            for(size_t j=0; j<N; j++)
            {
              //std::cerr << "bla: " << function(j,it) << " " << pdf(j) << std::endl << std::endl;
              temp += function(j,it)*pdf(j);
            }
            temp = std::log(moments(it)/temp/dx);

            lambda(it)+=temp;

            // Calculate the entropy
            entropy = -lambda.transpose()*Gn;

        }

        if(iter!=maxIter) std::cerr << "Converged in " << iter << " iterations." << std::endl;

        return pdf/L;


    }




};

class MaximumEntropyMP
    : public NLEsolve::MinPack
{

protected:

  Eigen::VectorXd domain;

  Eigen::VectorXd moments;

  Eigen::VectorXd pdf;

  Eigen::MatrixXd function;

  double epsilon;

  double L;
  double dx;

public:



  MaximumEntropyMP(const Eigen::VectorXd &domain, const Eigen::VectorXd &nonCentralMoments, double epsilon=0.0001)
    : MinPack(),
      domain(domain), moments(nonCentralMoments),
      function(domain.size(),moments.size()), epsilon(epsilon)
  {


    // set domain length
    L = domain(domain.size()-1)-domain(0);
    dx = (domain(1)-domain(0))/L;

    // Normalize moments
    for(int i=1; i<moments.size(); i++)
      moments(i)*=pow(L,-i);

    size_t M = moments.size();
    //size_t N = domain.size();

    // Initialize x^n
    Eigen::MatrixXd function(domain.size(),M);
    // Set x^0
    function.col(0) = Eigen::VectorXd::Ones(domain.size());
    // Fill the rest for n = 1...M
    for(size_t n=1; n<M; n++) function.col(n)=domain.cwiseProduct(function.col(n-1))/L;


  }

  Eigen::VectorXd computePDF()
  {


    size_t M = moments.size();

    Eigen::VectorXd lambda=Eigen::VectorXd::Zero(M);

    Eigen::VectorXd fvec=Eigen::VectorXd::Zero(M);

    int lwa = (M*(3*M+13))/2;

    Eigen::VectorXd wa=Eigen::VectorXd::Zero(lwa);

    // solve
    //std::cout << this->hybrd1 ( M, lambda.data(), fvec.data(), epsilon, wa.data(), lwa ) << std::endl;


    // return pdf
    Eigen::VectorXd pdf;
    pdf = -function.leftCols(M)*lambda;
    for(int i=0; i<pdf.size(); i++)
        pdf(i) = exp(pdf(i));

    return pdf;

  }

  virtual void fcn(int n, double _lambda[], double _vec[], int *iflag)

  {

    size_t M = moments.size();

    Eigen::Map<Eigen::VectorXd> lambda(_lambda,M);
    Eigen::Map<Eigen::VectorXd> vec(_vec,M);

    Eigen::VectorXd pdf = -function.leftCols(M)*lambda;
    for(int i=0; i<pdf.size(); i++)
        pdf(i) = exp(pdf(i));

    // Calculate G_n = integral dx x^n p(x)  of length M
    Eigen::VectorXd Gn = dx*(pdf.transpose()*function);

    vec = (moments-Gn);

  }

};


}}

#endif // MAXIMUMENTROPY_HH
