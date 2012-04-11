#include "linalgtest.hh"
#include "linalg/linalg.hh"
#include <iostream>

using namespace Fluc;
using namespace Fluc::Linalg;


void
LinalgTest::testGemv()
{
  Vector<double> x(3);
  x(0) = 1; x(1) = 0; x(2) = 0;

  Matrix<double> A(3,3);
  A(0,0) = 1; A(0,1) = 0; A(0,2) = 0;
  A(1,0) = 0; A(1,1) = 1; A(1,2) = 0;
  A(2,0) = 0; A(2,1) = 0; A(1,3) = 1;

  Vector<double> y(3);

  gemv(1., A, x, 0., y);

  UT_ASSERT_NEAR(y(0), 1.0);
  UT_ASSERT_NEAR(y(1), 0.0);
  UT_ASSERT_NEAR(y(2), 0.0);
}


void
LinalgTest::testGemm()
{
  Matrix<double> A(3,3);
  A(0,0) = 1.; A(0,1) = 0.; A(0,2) = 0.;
  A(1,0) = 0.; A(1,1) = 1.; A(1,2) = 0.;
  A(2,0) = 0.; A(2,1) = 0.; A(1,3) = 1.;

  Matrix<double> B(A.copy());
  Matrix<double> C(A.rows(), A.cols());

  gemm(1., A, B, 0., C);

  for (size_t i=0; i<A.rows(); i++)
  {
    for (size_t j=0; j<A.cols(); j++)
    {
      UT_ASSERT_NEAR(A(i,j), C(i,j));
    }
  }
}



void
LinalgTest::testDotMV()
{
  Vector<double> x(3);
  x(0) = 1; x(1) = 0; x(2) = 0;

  Matrix<double> A(3,3);
  A(0,0) = 1.; A(0,1) = 0.; A(0,2) = 0.;
  A(1,0) = 0.; A(1,1) = 1.; A(1,2) = 0.;
  A(2,0) = 0.; A(2,1) = 0.; A(1,3) = 1.;

  Vector<double> y(dot(A,x));

  UT_ASSERT_NEAR(y(0), 1.0);
  UT_ASSERT_NEAR(y(1), 0.0);
  UT_ASSERT_NEAR(y(2), 0.0);
}


void
LinalgTest::testDotMM()
{
  Matrix<double> A = Matrix<double>::unit(3);

  Matrix<double> B(A.copy());
  Matrix<double> C(dot(A,B));

  for (size_t i=0; i<A.rows(); i++)
  {
    for (size_t j=0; j<A.cols(); j++)
    {
      UT_ASSERT_NEAR(A(i,j), C(i,j));
    }
  }
  //
}


void
LinalgTest::testLUDec()
{
  std::srand(time(0));
  Matrix<double> A = Matrix<double>::rand(4,3);

  // Perform LU decomp.
  LUDec lu(A);

  Matrix<double> P = lu.getP();
  Matrix<double> L = lu.getL();
  Matrix<double> U = lu.getU();
  Matrix<double> Q = lu.getQ();

  Matrix<double> A2 = dot(P, dot(L, dot(U, Q)));

  for (size_t i=0; i<A.rows(); i++)
  {
    for (size_t j=0; j<A.cols(); j++)
    {
      //UT_ASSERT_NEAR(A2(i,j), A(i,j));
    }
    std::cerr << std::endl;
  }
}


UnitTest::TestSuite *
LinalgTest::suite()
{
  UnitTest::TestSuite *s = new UnitTest::TestSuite("LinAlg Tests");

  s->addTest(new UnitTest::TestCaller<LinalgTest>("BLAS gemv", &LinalgTest::testGemv));
  s->addTest(new UnitTest::TestCaller<LinalgTest>("BLAS gemm", &LinalgTest::testGemm));
  s->addTest(new UnitTest::TestCaller<LinalgTest>("Linalg dot(M, V)", &LinalgTest::testDotMV));
  s->addTest(new UnitTest::TestCaller<LinalgTest>("Linalg dot(M, M)", &LinalgTest::testDotMM));
  s->addTest(new UnitTest::TestCaller<LinalgTest>("Linalg LU", &LinalgTest::testLUDec));

  return s;
}
