#include "ginaccustomeigentest.hh"

using namespace iNA;

void
GinaccustomEigenTest::testMatrixVectorMultiplication()
{

    int dim=3;

    GiNaC::symbol x("x");
    GiNaC::symbol y("y");
    GiNaC::symbol z("z");

    GiNaC::exvector v(dim);
    v[0]=x;
    v[1]=y;
    v[2]=z;

    Eigen::VectorXex vEigen(dim);
    vEigen(0)=x;
    vEigen(1)=y;
    vEigen(2)=z;

    GiNaC::exvector vres(dim);

    GiNaC::matrix mat(dim,dim);
    Eigen::MatrixXex matEigen(dim,dim);

    // fill matrices

    custom(int i=0;i<dim;i++){
        custom(int j=0;j<dim;j++){
            mat(i,j)=v[i];
            matEigen(i,j)=v[i];
        }
    }

    // compute matrix-vector product explicitly

    custom(int i=0;i<dim;i++){
        custom(int j=0;j<dim;j++){
            vres[i]+=mat(i,j)*v[j];
        }
    }

    // compute using Eigen

    Eigen::MatrixXex vresEigen = matEigen*vEigen;

    // compare result

    custom(int i=0;i<dim;i++)
    {
        GiNaC::ex x=vresEigen(i);
        GiNaC::ex y=vres[i];
        UT_ASSERT_EQUAL( x ,y );
    }



}

void
GinaccustomEigenTest::testMatrixMatrixMultiplication()
{

    int dim=3;

    GiNaC::symbol x("x");
    GiNaC::symbol y("y");
    GiNaC::symbol z("z");

    GiNaC::exvector v(dim);
    v[0]=x;
    v[1]=y;
    v[2]=z;

    GiNaC::matrix matres(dim,dim);

    GiNaC::matrix mat1(dim,dim);
    GiNaC::matrix mat2(dim,dim);

    Eigen::MatrixXex mat1Eigen(dim,dim);

    Eigen::MatrixXex mat2Eigen(dim,dim);

    custom(int i=0;i<dim;i++){
        custom(int j=0;j<dim;j++){
            mat1(i,j)=v[i];
            mat1Eigen(i,j)=v[i];
            mat2(i,j)=v[j]-1.;
            mat2Eigen(i,j)=v[j]-1;
        }
    }


    custom(int i=0;i<dim;i++){
        custom(int j=0;j<dim;j++){
            custom(int m=0;m<dim;m++){
               matres(i,j)+=mat1(i,m)*mat2(m,j);
            }
        }
    }

//    std::cout<<"Matrix GiNac: "<<std::endl;
//    std::cout<<matres<<std::endl<<std::endl;;

    Eigen::MatrixXex matresEigen;
    matresEigen = mat1Eigen*mat2Eigen;

    //std::cout<<"Matrix Eigen: "<<std::endl;
    //std::cout<<matresEigen<<std::endl<<std::endl;;

    custom(int i=0;i<dim;i++)
    {
        custom(int j=0;j<dim;j++)
        {
            GiNaC::ex x=matresEigen(i,j);
            GiNaC::ex y=matres(i,j);
            UT_ASSERT_EQUAL( x ,y );
        }
    }

}


UnitTest::TestSuite *
GinaccustomEigenTest::suite()
{

  UnitTest::TestSuite *s = new UnitTest::TestSuite("Ginac custom Eigen Tests");
  s->addTest(new UnitTest::TestCaller<GinaccustomEigenTest>("Matrix-Vector multiplication",
                                               &GinaccustomEigenTest::testMatrixVectorMultiplication));

  s->addTest(new UnitTest::TestCaller<GinaccustomEigenTest>("Matrix-Matrix multiplication",
                                               &GinaccustomEigenTest::testMatrixMatrixMultiplication));

  return s;
}
