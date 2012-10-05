#include "kroneckerproduct.hh"

Eigen::MatrixXd
iNA::KroneckerSum(const Eigen::MatrixXd &m1, const Eigen::MatrixXd &m2)
{

    if(m1.rows()!=m1.cols() || m2.rows()!=m2.cols()) throw("Kronecker sum is only defined for quadratic matrices");

    Eigen::MatrixXd msum(m1.rows()*m2.rows(), m1.cols()*m2.cols());
    Eigen::MatrixXd mp1(m1.rows()*m2.rows(), m1.cols()*m2.cols());
    Eigen::MatrixXd mp2(m1.rows()*m2.rows(), m1.cols()*m2.cols());

    /* @todo dense unit matrix, this is ugly */
    Eigen::MatrixXd unity = Eigen::MatrixXd::Identity(m2.rows(),m2.rows());
    KroneckerProduct(mp1,m1,unity);

    unity = Eigen::MatrixXd::Identity(m1.rows(),m1.rows());
    KroneckerProduct(mp2,unity,m2);

    msum = mp1 + mp2;

    return msum;

}

Eigen::MatrixXcd
iNA::ComplexKroneckerSum(const Eigen::MatrixXcd &m1, const Eigen::MatrixXcd &m2)
{

    if(m1.rows()!=m1.cols() || m2.rows()!=m2.cols()) throw("Kronecker sum is only defined for quadratic matrices");

    Eigen::MatrixXcd msum(m1.rows()*m2.rows(), m1.cols()*m2.cols());
    Eigen::MatrixXcd mp1(m1.rows()*m2.rows(), m1.cols()*m2.cols());
    Eigen::MatrixXcd mp2(m1.rows()*m2.rows(), m1.cols()*m2.cols());

    /* @todo dense unit matrix, this is ugly */
    Eigen::MatrixXcd unity = Eigen::MatrixXcd::Identity(m2.rows(),m2.rows());
    KroneckerProduct(mp1,m1,unity);

    unity = Eigen::MatrixXcd::Identity(m1.rows(),m1.rows());
    KroneckerProduct(mp2,unity,m2);

    msum = mp1 + mp2;

    return msum;

}

void
iNA::ComplexSylvesterSolve(const Eigen::MatrixXcd &m1, const Eigen::MatrixXcd &m2,
               const Eigen::MatrixXcd &Dm , Eigen::MatrixXcd &solutionM)
{

    if (Dm.rows()!=Dm.cols()) throw("Oops ... SylvesterSolve called with invalid arguments.");

    Eigen::VectorXcd tail( Dm.rows()*Dm.rows() );
    Eigen::VectorXcd sol( Dm.rows()*Dm.rows() );

    size_t idx = 0;
    for(int i=0;i<Dm.rows();i++)
      for(int j=0;j<Dm.cols();j++)
        tail(idx++) = Dm(i,j);

    // solve Sylvester matrix equation: JMsum*\vec{x}+\vec{D}=0
    Eigen::FullPivLU<Eigen::MatrixXcd> ludecomp(-ComplexKroneckerSum(m1,m2));
    sol = ludecomp.solve(tail);

    solutionM.resize(Dm.rows(),Dm.cols());

    // prepare solution in matrix form
    idx = 0;
    for(int i=0;i<Dm.rows();i++)
      for(int j=0;j<Dm.cols();j++)
          solutionM(i,j) = sol(idx++);

    // done.

}


