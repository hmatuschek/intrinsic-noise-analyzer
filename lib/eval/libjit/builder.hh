#ifndef __FLUC_EVALUATE_LIBJIT_BUILDER_HH__
#define __FLUC_EVALUATE_LIBJIT_BUILDER_HH__

#include <jit/jit.h>
#include "exception.hh"
#include <ginac.h>
#include <complex>


namespace Fluc {
namespace Evaluate {
namespace libjit {

/**
 * Helper class to assemble libjit types and values.
 */
class ComplexTypeBuilder
{
public:
  /**
   * Constructs a complex type, as a struct of two doubles.
   */
  static jit_type_t create()
  {
    jit_type_t elements[2];
    elements[0] = jit_type_sys_double;
    elements[1] = jit_type_sys_double;
    return jit_type_create_struct(elements, 2, 1);
  }
};


/**
 * Helper class to construct a constant uint.
 */
class IndexBuilder
{
public:
  static jit_value_t create(jit_function_t function, size_t index)
  {
    jit_value_t ret = jit_value_create_long_constant(function, jit_type_sys_uint, index);

    // Check for errors:
    if (0 == ret) {
      InternalError err;
      err << "Can not compile symbol-index " << index << ".";
      throw err;
    }

    return ret;
  }
};


/**
 * Template prototype for a constructor of a constant value.
 */
template <typename Scalar>
class ConstantBuilder
{
public:
  /**
   * Constructor method.
   */
  static jit_value_t create(jit_function_t function, const GiNaC::numeric &value);
};


/**
 * Actual implementation for real values.
 */
template <>
class ConstantBuilder<double>
{
public:
  /**
   * Constructor method.
   */
  static jit_value_t create(jit_function_t function, const GiNaC::numeric &value)
  {
    // Check if value is real:
    if (! GiNaC::is_real(value)) {
      TypeError err;
      err << "Can not construct real constant from " << value
          << ": Value not real.";
      throw err;
    }

    // Construct constant value:
    jit_value_t ret = jit_value_create_float64_constant(
          function, jit_type_sys_double, value.to_double());

    // Check for errors:
    if(0 == ret) {
      InternalError err;
      err << "Can not compile numerical GiNaC value: " << value;
      throw err;
    }

    return ret;
  }
};


/**
 * Actual implementation for complex values.
 */
template <>
class ConstantBuilder < std::complex<double> >
{
public:
  /** Constructor method. */
  static jit_value_t create(jit_function_t function, const GiNaC::numeric &value)
  {
    jit_value_t real = ConstantBuilder<double>::create(function, GiNaC::real(value));
    jit_value_t imag = ConstantBuilder<double>::create(function, GiNaC::imag(value));
    jit_value_t ret = jit_value_create(function, ComplexTypeBuilder::create());
    jit_value_set_addressable(ret);
    jit_value_t ptr = jit_insn_address_of(function, ret);

    jit_insn_store_elem(function, ptr, IndexBuilder::create(function, 0), real);
    jit_insn_store_elem(function, ptr, IndexBuilder::create(function, 1), imag);

    return ret;
  }
};



template <typename Scalar>
class LoadBuilder
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t index);
};


template <>
class LoadBuilder<double>
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t index)
  {
    // Construct load instruction:
    jit_value_t load = jit_insn_load_elem(
          function, jit_value_get_param(function, 0),
          index, jit_type_sys_double);

    // Check for errors:
    if (0 == load) {
      InternalError err;
      err << "Can not compile load instruction.";
      throw err;
    }

    return load;
  }
};


template <>
class LoadBuilder< std::complex<double> >
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t index)
  {
    // Construct load instruction:
    jit_value_t load = jit_insn_load_elem(
          function, jit_value_get_param(function, 0),
          index, ComplexTypeBuilder::create());

    // Check for errors:
    if (0 == load) {
      InternalError err;
      err << "Can not compile load instruction.";
      throw err;
    }

    return load;
  }
};



template <typename Scalar>
class AddBuilder
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t lhs, jit_value_t rhs);
};


template <>
class AddBuilder <double>
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t lhs, jit_value_t rhs)
  {
    jit_value_t add = jit_insn_add(function, lhs, rhs);

    if (0 == add) {
      InternalError err;
      err << "Can not construct ADD instruction.";
      throw err;
    }

    return add;
  }
};


template <>
class AddBuilder < std::complex<double> >
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t lhs, jit_value_t rhs)
  {
    jit_value_t lhs_ptr = jit_insn_address_of(function, lhs);
    jit_value_t rhs_ptr = jit_insn_address_of(function, rhs);
    jit_value_t lhs_real = jit_insn_load_elem(
          function, lhs_ptr, IndexBuilder::create(function, 0), jit_type_sys_double);
    jit_value_t lhs_imag = jit_insn_load_elem(
          function, lhs_ptr, IndexBuilder::create(function, 1), jit_type_sys_double);
    jit_value_t rhs_real = jit_insn_load_elem(
          function, rhs_ptr, IndexBuilder::create(function, 0), jit_type_sys_double);
    jit_value_t rhs_imag = jit_insn_load_elem(
          function, rhs_ptr, IndexBuilder::create(function, 1), jit_type_sys_double);

    jit_value_t add_real = jit_insn_add(function, lhs_real, rhs_real);
    jit_value_t add_imag = jit_insn_add(function, lhs_imag, rhs_imag);
    jit_value_t add = jit_value_create(function, ComplexTypeBuilder::create());
    jit_value_set_addressable(add);
    jit_insn_store_elem(function, jit_insn_address_of(function, add),
                        IndexBuilder::create(function, 0), add_real);
    jit_insn_store_elem(function, jit_insn_address_of(function, add),
                        IndexBuilder::create(function, 1), add_imag);

    return add;
  }
};



template <typename Scalar>
class MulBuilder
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t lhs, jit_value_t rhs);
};


template <>
class MulBuilder <double>
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t lhs, jit_value_t rhs)
  {
    jit_value_t mul = jit_insn_mul(function, lhs, rhs);

    if (0 == mul) {
      InternalError err;
      err << "Can not construct MUL instruction.";
      throw err;
    }

    return mul;
  }
};


template <>
class MulBuilder < std::complex<double> >
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t lhs, jit_value_t rhs)
  {
    jit_value_t lhs_ptr = jit_insn_address_of(function, lhs);
    jit_value_t rhs_ptr = jit_insn_address_of(function, rhs);
    jit_value_t lhs_real = jit_insn_load_elem(
          function, lhs_ptr, IndexBuilder::create(function, 0), jit_type_sys_double);
    jit_value_t lhs_imag = jit_insn_load_elem(
          function, lhs_ptr, IndexBuilder::create(function, 1), jit_type_sys_double);
    jit_value_t rhs_real = jit_insn_load_elem(
          function, rhs_ptr, IndexBuilder::create(function, 0), jit_type_sys_double);
    jit_value_t rhs_imag = jit_insn_load_elem(
          function, rhs_ptr, IndexBuilder::create(function, 1), jit_type_sys_double);

    jit_value_t mul_real = jit_insn_sub(
          function,
          jit_insn_mul(function, lhs_real, rhs_real),
          jit_insn_mul(function, lhs_imag, rhs_imag));
    jit_value_t mul_imag = jit_insn_add(
          function,
          jit_insn_mul(function, lhs_real, rhs_imag),
          jit_insn_mul(function, lhs_imag, rhs_real));
    jit_value_t mul = jit_value_create(function, ComplexTypeBuilder::create());
    jit_value_set_addressable(mul);
    jit_insn_store_elem(function, jit_insn_address_of(function, mul),
                        IndexBuilder::create(function, 0), mul_real);
    jit_insn_store_elem(function, jit_insn_address_of(function, mul),
                        IndexBuilder::create(function, 1), mul_imag);

    return mul;
  }
};



template <typename Scalar>
class PowBuilder
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t base, jit_value_t expo);
};


template <>
class PowBuilder <double>
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t base, jit_value_t expo)
  {
    jit_value_t pow = jit_insn_pow(function, base, expo);

    if (0 == pow) {
      InternalError err;
      err << "Can not construct POW instruction.";
      throw err;
    }

    return pow;
  }
};


template <>
class PowBuilder < std::complex<double> >
{
public:
  static jit_value_t create(jit_function_t function, jit_value_t lhs, jit_value_t rhs)
  {
    // Create function interface:
    jit_type_t params[3];
    jit_type_t signature;
    params[0] = jit_type_create_pointer(ComplexTypeBuilder::create(), 1);
    params[1] = ComplexTypeBuilder::create();
    params[2] = ComplexTypeBuilder::create();
    signature = jit_type_create_signature(jit_abi_cdecl, ComplexTypeBuilder::create(), params, 3, 1);

    // Allocate return value:
    jit_value_t res = jit_value_create(function, ComplexTypeBuilder::create());
    jit_value_set_addressable(res);

    // Assemble function arguments:
    jit_value_t args[3];
    args[0] = jit_insn_address_of(function, res); args[1] = lhs; args[2] = rhs;

    // Create function call
    jit_insn_call_native(function, "cpow", (void *)cpow, signature, args, 3, JIT_CALL_NOTHROW);

    // Done:
    return res;
  }

  static void cpow(std::complex<double> *res, std::complex<double> base, std::complex<double> exp) throw()
  {
    *res = std::pow(base, exp);
    std::cerr << *res << std::endl;
  }
};


}
}
}
#endif
