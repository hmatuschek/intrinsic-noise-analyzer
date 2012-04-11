#ifndef __FLUC_KRONECKERPRODUCT_HH
#define __FLUC_KRONECKERPRODUCT_HH

#include <eigen3/Eigen/Eigen>

namespace Fluc{

/**
* @deprecated
* Implementation of the Kronecker product of matrix 'm1' and 'm2'
* yielding the matrix 'mproduct'
*
* @ingroup math
**/

template <typename T, typename U>
inline void
KroneckerProduct(Eigen::MatrixBase<U> &mproduct_, const Eigen::MatrixBase<T> &m1, const Eigen::MatrixBase<T> &m2)
{

    Eigen::MatrixBase<U>& mproduct = const_cast< Eigen::MatrixBase<U>& >(mproduct_);
    //result is a (m1.rows()*m2.rows(), m1.cols()*m2.cols()) matrix
    mproduct.derived().resize(m1.rows()*m2.rows(), m1.cols()*m2.cols());

    for (int i = 0; i < m1.cols(); i++) {
      for (int j = 0; j < m1.rows(); j++) {
        mproduct.block(i*m2.rows(), j*m2.cols(), m2.rows(), m2.cols()) =  m1(i,j)*m2;
      }
    }

}


/**
* Function yielding the Kronecker product of matrix 'm1' and 'm2'
* yielding the matrix 'mproduct'
*
* @ingroup math
**/

template <class T>
Eigen::MatrixBase<T>
KroneckerProduct(const Eigen::MatrixBase<T> &m1, const Eigen::MatrixBase<T> &m2)
{
    T mproduct;
    KroneckerProduct(mproduct.derived(), m1.derived(), m2.derived());
    return mproduct;
}

/**
* Function that yields the Kronecker sum of matrix 'm1' and 'm2'.
*
* Note that @c m1 and @c m2 must be quadratic.
*
* @ingroup math
**/

Eigen::MatrixXd
KroneckerSum(const Eigen::MatrixXd &m1, const Eigen::MatrixXd &m2);


/**
* Function that yields the Kronecker sum of matrix 'm1' and 'm2'.
*
* Note that @c m1 and @c m2 must be quadratic.
*
* @ingroup math
**/

Eigen::MatrixXcd
ComplexKroneckerSum(const Eigen::MatrixXcd &m1, const Eigen::MatrixXcd &m2);

/**
* Function for simple solution of the Sylvester matrix equation,
*
* \f[ \underline{A} \, \underline{X} + \underline{X} \, \underline{B}^T + \underline{M} = 0 \f]
*
* by simple vectorization.
*
* @param[in] m1 the matrix \f$ \underline{A} \f$
* @param[in] m2 the matrix \f$ \underline{B} \f$
* @param[in] Dm the matrix \f$ \underline{M} \f$
* @param[out] solutionM the matrix \f$ \underline{X} \f$
*
* @ingroup math
**/
void
ComplexSylvesterSolve(const Eigen::MatrixXcd &m1, const Eigen::MatrixXcd &m2,
               const Eigen::MatrixXcd &Dm , Eigen::MatrixXcd &solutionM);

} // end namespace

#endif // __FLUC_KRONECKERPRODUCT_HH
