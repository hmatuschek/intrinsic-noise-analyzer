#ifndef __FLUC_EVALUATE_LIBJIT_ASSEMBLER_HH__
#define __FLUC_EVALUATE_LIBJIT_ASSEMBLER_HH__

#include "builder.hh"
#include "code.hh"


namespace Fluc {
namespace Evaluate {
namespace libjit {


/**
 * Internal used class to compile single GiNaC expressions into libjit IR.
 */
template <typename Scalar>
class Assembler
    : public GiNaC::visitor, public GiNaC::numeric::visitor, public GiNaC::add::visitor,
    public GiNaC::mul::visitor, public GiNaC::symbol::visitor, public GiNaC::power::visitor
{
protected:
  /**
   * Holds a weak reference to the code.
   */
  Code *code;

  /**
   * Holds a weak reference to the index table, mapping symbols to indices of the input-vector.
   */
  const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table;

  /**
   * Internal used stack of values.
   */
  std::list<jit_value_t> stack;


public:
  /**
   * Constructor.
   */
  Assembler(Code *code, const std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table)
    : code(code), index_table(index_table), stack()
  {
    // Pass...
  }


  /**
   * Handles constant numerical (float) values.
   */
  void visit(const GiNaC::numeric &value)
  {
    this->stack.push_back(
          ConstantBuilder<Scalar>::create(this->code->getFunction(), value));
  }


  /**
   * Handles a variable (symbol).
   */
  void visit(const GiNaC::symbol &symbol)
  {
    std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::const_iterator item;

    if(this->index_table.end() == (item = this->index_table.find(symbol)))
    {
      SymbolError err;
      err << "Can not resolve index for symbol " << symbol << "!";
      throw err;
    }

    // Get index in input-vector:
    jit_value_t index = IndexBuilder::create(this->code->getFunction(), item->second);

    // Construct load instruction:
    jit_value_t load = LoadBuilder<Scalar>::create(this->code->getFunction(), index);

    this->stack.push_back(load);
  }


  /**
   * First, processes all summands and finally assembles sum.
   */
  void visit(const GiNaC::add &sum)
  {
    // For summands of the sum:
    for (size_t i=0; i<sum.nops(); i++)
    {
      // First, traverse into each summand:
      sum.op(i).accept(*this);
    }

    // Then assemble the sum on the stack:
    for (size_t i=1; i<sum.nops(); i++)
    {
      // Get RHS and LHS values from stack:
      jit_value_t rhs = this->stack.back(); this->stack.pop_back();
      jit_value_t lhs = this->stack.back(); this->stack.pop_back();

      // Construct SUM instruction and push it on the stack:
      this->stack.push_back(
            AddBuilder<Scalar>::create(this->code->getFunction(), lhs, rhs));
    }
  }


  /**
   * First, processes all factors and finally assembles product.
   */
  void visit(const GiNaC::mul &prod)
  {
    // For factor of the product:
    for (size_t i=0; i<prod.nops(); i++)
    {
      // First, traverse into each factor:
      prod.op(i).accept(*this);
    }

    // Then assemble the factor on the stack:
    for (size_t i=1; i<prod.nops(); i++)
    {
      // Get RHS and LHS values from stack:
      jit_value_t rhs = this->stack.back(); this->stack.pop_back();
      jit_value_t lhs = this->stack.back(); this->stack.pop_back();

      // Construct MUL instruction and push it on the stack:
      this->stack.push_back(
            MulBuilder<Scalar>::create(this->code->getFunction(), lhs, rhs));
    }
  }


  /**
   * Handles powers.
   */
  void visit(const GiNaC::power &pow)
  {
    // handle basis
    pow.op(0).accept(*this);
    // handle exponent
    pow.op(1).accept(*this);

    // Get base and exponent:
    jit_value_t exponent = this->stack.back(); this->stack.pop_back();
    jit_value_t base     = this->stack.back(); this->stack.pop_back();

    // Construct POW instruction and push it on the stack:
    this->stack.push_back(
          PowBuilder<Scalar>::create(this->code->getFunction(), base, exponent));
  }


  /**
   * Pops the last value on the stack.
   */
  jit_value_t popValue()
  {
    if (0 == this->stack.size()) {
      InternalError err;
      err << "Can not pop any JIT value from stack: Stack empty.";
      throw err;
    }

    if (1 != this->stack.size()) {
      std::cerr << "Oops. More than one element left on the stack.";
    }

    // Remove element from stack:
    jit_value_t value = this->stack.back(); this->stack.pop_back();

    // Done.
    return value;
  }
};


}
}
}

#endif
