#ifndef __FLUC_EVALUATE_DIRECT_INTERPRETER_HH__
#define __FLUC_EVALUATE_DIRECT_INTERPRETER_HH__

#include "code.hh"
#include "exception.hh"
#include <complex>


namespace iNA {
namespace Eval {
namespace direct {

/**
 * This little nasty hack is needed to have some destination-type dependent unpacking
 * of GiNaC values.
 *
 * @ingroup direct
 */
template <typename Scalar>
class GiNaCValuePacker
{
public:
  /**
   * Unpacks a ginac value.
   */
  static Scalar unpack(const GiNaC::ex &value);

  static GiNaC::ex pack(const Scalar &value);
};


/**
 * Specialized unpacker for real numbers.
 *
 * @ingroup direct
 */
template<>
class GiNaCValuePacker<double>
{
public:
  /**
   * Implements the upacking of @c double values.
   */
  static double unpack(const GiNaC::ex &value)
  {
    GiNaC::numeric num = GiNaC::ex_to<GiNaC::numeric>(value);
    if (! num.is_real()) {
      TypeError err;
      err << "Can not convert some non-real value " << value << " to double.";
      throw err;
    }
    return num.to_double();
  }

  static GiNaC::ex pack(const double &value)
  {
    return value;
  }
};


/**
 * Specialized unpacker for complex numbers.
 *
 * @ingroup direct
 */
template<>
class GiNaCValuePacker< std::complex<double> >
{
public:
  /**
   * Implements the unpacking of @c complex<double> values.
   */
  static std::complex<double> unpack(const GiNaC::ex &value)
  {
    GiNaC::numeric num = GiNaC::ex_to<GiNaC::numeric>(value);
    return std::complex<double>(
          GiNaC::real(num).to_double(), GiNaC::imag(num).to_double());
  }

  static GiNaC::ex pack(const std::complex<double> &value)
  {
    return value.real() + GiNaC::I * value.imag();
  }
};



/**
 * Interpreter for the direct evaluation engine.
 *
 * @ingroup direct
 */
template <class InType, class OutType=InType>
class Interpreter
{
protected:
  /**
   * Holds the code to execute.
   */
  Code *code;


public:
  /**
   * Constructor.
   */
  Interpreter()
    : code(0)
  {
    // Pass...
  }


  /**
   * Constructor.
   *
   * Also sets the code to execute.
   */
  Interpreter(Code *code)
    : code(code)
  {
    // Pass...
  }


  /**
   * (Re-) Sets the code.
   */
  void setCode(Code *code)
  {
    this->code = code;
  }


  /**
   * Evaluates the "code".
   */
  inline void run(const InType &input, OutType &output) {
    this->run(input.data(), output.data());
  }


  inline void run(const typename InType::Scalar *input, typename OutType::Scalar *output)
  {
    GiNaC::exmap values;

    // Populate values from input vector:
    for(Code::IndexTable::iterator item = this->code->getIndexTable().begin();
        item != this->code->getIndexTable().end(); item++)
    {
      values[item->first] = GiNaCValuePacker<typename OutType::Scalar>::pack(input[item->second]);
    }

    // Evaluate expressions
    for (Code::iterator item = this->code->begin(); item != this->code->end(); item++)
    {
      GiNaC::ex value;

      // First, try to evaluate the ginac expression
      try {
        value = item->first.subs(values).evalf();
      } catch (std::exception &err) {
        InternalError new_err;
        new_err << "Cannot evaluate expression " << item->first
                << " : An unhandled std::exception was thrown: " << err.what();
        throw new_err;
      } catch (...) {
        InternalError new_err;
        new_err << "Cannot evaluate expression " << item->first
                << " : An unhandled, unkown exception was thrown!";
        throw new_err;
      }

      // Check if value is a numerical value:
      if (! GiNaC::is_a<GiNaC::numeric>(value)) {
        iNA::RuntimeError err;
        err << "Can not evaluate GiNaC expression " << item->first
            << " some symbols can not be resolved! Minimal expression: " << value;
        throw err;
      }

      // Get value as what ever is requested by output type:
      output[item->second] = GiNaCValuePacker<typename OutType::Scalar>::unpack(value);
    }
  }
};


}
}
}

#endif
