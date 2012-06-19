#ifndef __FLUC_EVALUATE_LLVM_ASSEMBLER_HH__
#define __FLUC_EVALUATE_LLVM_ASSEMBLER_HH__

#include "code.hh"
#include "exception.hh"
#include "builder.hh"
#include <ginac/ginac.h>


namespace Fluc {
namespace Eval {
namespace jit {

/**
 * Enumerates the known built-in functions.
 * @ingroup jit
 */
typedef enum {
    FUNCTION_ABS,  /// < Function code for the absolute value "abs()".
    FUNCTION_LOG,  /// < Function code for the natural logarithm "log()".
    FUNCTION_EXP   /// < Function code for the exponential function "exp()".
} FunctionCode;


/**
 * This class implements the actual translation of GiNaC expressions into LLVM IR.
 *
 * @ingroup jit
 */
template <typename Scalar>
class Assembler
    : public GiNaC::visitor, public GiNaC::numeric::visitor, public GiNaC::add::visitor,
    public GiNaC::mul::visitor, public GiNaC::symbol::visitor, public GiNaC::power::visitor,
    public GiNaC::function::visitor
{
protected:
  /**
   * Holds a weak reference to the code.
   */
  Code *code;

  /**
   * Maps a GiNaC symbol to an index of the input vector.
   */
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table;

  /**
   * Holds the translation-table GiNaC Function serial -> Function Code:
   */
  std::map<unsigned, FunctionCode> function_codes;

  /**
   * The value stack.
   */
  std::list<llvm::Value *> stack;


public:
  Assembler(Code *code, std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> &index_table)
    : code(code), index_table(index_table)
  {
      // Populate function-code table:
      this->function_codes[GiNaC::abs_SERIAL::serial] = FUNCTION_ABS;
      this->function_codes[GiNaC::log_SERIAL::serial] = FUNCTION_LOG;
      this->function_codes[GiNaC::exp_SERIAL::serial] = FUNCTION_EXP;
  }

  /**
   * Pops a value from the stack, that have been left there.
   */
  llvm::Value *popValue()
  {
    if (1 != this->stack.size()) {
      InternalError err;
      err << "Invalid stack size: Expected 1 got: " << (unsigned) this->stack.size();
      throw err;
    }

    llvm::Value *value = this->stack.back(); this->stack.pop_back();
    return value;
  }

  /**
   * Handles constant numerical (float) values.
   */
  virtual void visit(const GiNaC::numeric &value)
  {
    this->stack.push_back(Builder<Scalar>::createConstant(code, value));
  }

  /**
   * Handles a variable (symbol).
   */
  virtual void visit(const GiNaC::symbol &symbol)
  {
    // Resolve index for symbol:
    std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::iterator item;
    if (this->index_table.end() == (item = this->index_table.find(symbol))) {
      SymbolError err;
      err << "Can not resolve symbol " << symbol;
      throw err;
    }

    this->stack.push_back(Builder<Scalar>::createLoad(code, item->second));
  }

  /**
   * First, processes all summands and finally assembles sum.
   */
  virtual void visit(const GiNaC::add &sum)
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
      if (2 > this->stack.size()) {
        InternalError err;
        err << "Can not assemble value: Not enougth values on stack: "
            << (unsigned) this->stack.size();
        throw err;
      }

      llvm::Value *rhs = this->stack.back(); this->stack.pop_back();
      llvm::Value *lhs = this->stack.back(); this->stack.pop_back();
      this->stack.push_back(Builder<Scalar>::createAdd(this->code, lhs, rhs));
    }
  }

  /**
   * First, processes all factors and finally assembles product.
   */
  virtual void visit(const GiNaC::mul &prod) {
    // For factors of the product:
    for (size_t i=0; i<prod.nops(); i++)
    {
      // First, traverse into each factor:
      prod.op(i).accept(*this);
    }

    // Then assemble the product on the stack:
    for (size_t i=1; i<prod.nops(); i++)
    {
      if (2 > this->stack.size()) {
        InternalError err;
        err << "Can not assemble value: Not enougth values on stack: "
            << (unsigned) this->stack.size();
        throw err;
      }

      llvm::Value *rhs = this->stack.back(); this->stack.pop_back();
      llvm::Value *lhs = this->stack.back(); this->stack.pop_back();
      this->stack.push_back(Builder<Scalar>::createMul(this->code, lhs, rhs));
    }
  }

  /**
   * Handles powers.
   */
  virtual void visit(const GiNaC::power &pow)
  {
    // handle basis
    pow.op(0).accept(*this);
    // handle exponent
    pow.op(1).accept(*this);

    if (2 > this->stack.size()) {
      InternalError err;
      err << "Can not assemble value: Not enougth values on stack: "
          << (unsigned) this->stack.size();
      throw err;
    }

    llvm::Value *exponent = this->stack.back(); this->stack.pop_back();
    llvm::Value *base     = this->stack.back(); this->stack.pop_back();
    this->stack.push_back(Builder<Scalar>::createPow(this->code, base, exponent));
  }


  virtual void visit(const GiNaC::function &function)
  {
    std::map<unsigned, FunctionCode>::iterator item = this->function_codes.find(function.get_serial());

    if (this->function_codes.end() == item) {
        InternalError err;
        err << "Can not compile function evaluation " << function << ": unknown function.";
        throw err;
    }

    switch(item->second) {
    case FUNCTION_ABS:
    {
        // Handle function argument:
        function.op(0).accept(*this);

        if (1 > this->stack.size()) {
            InternalError err;
            err << "Can not assemble value: Not enough values on stack: "
                << (unsigned) this->stack.size();
            throw err;
        }

        llvm::Value *arg = this->stack.back(); this->stack.pop_back();
        this->stack.push_back(Builder<Scalar>::createAbs(this->code, arg));
    }
    break;

    case FUNCTION_LOG:
    {
        // Handle function argument:
        function.op(0).accept(*this);

        if (1 > this->stack.size()) {
            InternalError err;
            err << "Can not assemble value: Not enough values on stack: "
                << (unsigned) this->stack.size();
            throw err;
        }

        llvm::Value *arg = this->stack.back(); this->stack.pop_back();
        this->stack.push_back(Builder<Scalar>::createLog(this->code, arg));
    }
    break;

    case FUNCTION_EXP:
    {
        // Handle function argument:
        function.op(0).accept(*this);

        if (1 > this->stack.size()) {
            InternalError err;
            err << "Can not assemble value: Not enough values on stack: "
                << (unsigned) this->stack.size();
            throw err;
        }

        llvm::Value *arg = this->stack.back(); this->stack.pop_back();
        this->stack.push_back(Builder<Scalar>::createExp(this->code, arg));
    }
    break;
    }
  }


  /**
   * Handles all unhandled expression parts -> throws an exception.
   */
  void visit(const GiNaC::basic &basic) {
      InternalError err;
      err << "Can not compile expression " << basic << ": Unknown expression type.";
      throw err;
  }

};


}
}
}

#endif // ASSEMBLER_HH
