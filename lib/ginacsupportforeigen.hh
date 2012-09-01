#ifndef EIGENGINACSUPPORT_HH
#define EIGENGINACSUPPORT_HH

#include <eigen3/Eigen/Eigen>
#include <ginac/ginac.h>


namespace Eigen {

/**
 * Using Ginac expressions with Eigen matrices.
 */
template<> struct NumTraits<GiNaC::ex>
{
  /** Defines the real value-type. */
  typedef GiNaC::ex Real;
  /** Defines any non-integer value type. */
  typedef GiNaC::ex NonInteger;
  /** Ahhm, what ever nested means. */
  typedef GiNaC::ex Nested;

  enum {
    IsComplex = 1,             ///< Yes, GiNaC expressions can be complex in some cases.
    IsInteger = 0,             ///< Nop, an they are not generally integer types.
    RequireInitialization = 1, ///< Jup, they need a constructor.
    IsSigned = 1,              ///< and they are signed.
    ReadCost = 1,              ///< GiNaC expressions are always expensive.
    AddCost = 1,               ///< GiNaC expressions are always expensive.
    MulCost = 1                ///< GiNaC expressions are always expensive.
  };

  /** Yes, ginac expressions are exact! */
  static Real epsilon()
  {
    return Real(0.);
  }
};


}


// Ginac's expression type ex is defined in the GiNaC namespace
// therefore, the following functions *must* be defined
// in the same namespace
namespace Eigen {
  namespace internal {
    inline GiNaC::ex conj(const GiNaC::ex& x)  { return x.conjugate(); }
    inline GiNaC::ex real(const GiNaC::ex& x)  { return x.real_part(); }
    inline GiNaC::ex imag(const GiNaC::ex& x)  { return x.imag_part(); }
    inline GiNaC::ex abs(const GiNaC::ex& x)   { GiNaC::ex norm = x.real_part()*x.real_part()+x.imag_part()*x.imag_part(); return GiNaC::sqrt(norm); }
    inline GiNaC::ex abs2(const GiNaC::ex& x)  { GiNaC::ex norm = x.real_part()*x.real_part()+x.imag_part()*x.imag_part(); return norm; }
    inline GiNaC::ex sqrt(const GiNaC::ex& x)  { return GiNaC::sqrt(x); }
    inline GiNaC::ex exp(const GiNaC::ex&  x)  { return GiNaC::exp(x); }
    inline GiNaC::ex log(const GiNaC::ex&  x)  { return GiNaC::log(x); }
    inline GiNaC::ex sin(const GiNaC::ex&  x)  { return GiNaC::sin(x); }
    inline GiNaC::ex cos(const GiNaC::ex&  x)  { return GiNaC::cos(x); }
    inline GiNaC::ex pow(const GiNaC::ex&  x, const GiNaC::ex &y)  { return GiNaC::pow(x, y); }
  }
}


// Some typedefs here
namespace Eigen{

    /**
     * matrix of Ginac expressions.
     */
    typedef Eigen::Matrix< GiNaC::ex , Dynamic, Dynamic > MatrixXex;

    /**
     * vector of Ginac expressions.
     */
    typedef Eigen::Matrix< GiNaC::ex , Dynamic, 1> VectorXex;

    /**
    * Simple inline evaluator
    */
    inline Eigen::MatrixXd ex2double(const Eigen::MatrixXex &In)
    {

        Eigen::MatrixXd Out(In.rows(),In.cols());
        // ... and fold all constants due to conservation laws
        for (int i=0; i<In.rows(); i++)
        for (int j=0; j<In.cols(); j++)
            Out(i,j)=GiNaC::ex_to<GiNaC::numeric>(In(i,j)).to_double();

        return Out;

    }

}

#endif // EIGENGINACSUPPORT_HH
