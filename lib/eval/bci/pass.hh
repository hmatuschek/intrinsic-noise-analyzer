#ifndef __FLUC_EVALUATE_BCI_PASS_HH__
#define __FLUC_EVALUATE_BCI_PASS_HH__

#include "dependencetree.hh"
#include "interpreter.hh"

#include <list>


namespace Fluc {
namespace Eval {
namespace bci {


/**
 * The base class of all passes.
 *
 * A pass implements some manipulation on the dependence-tree by implementing the virtual
 * @c handleValue method.
 *
 * @ingroup bci
 */
class Pass
{
public:
  /**
   * This method may be overridden by any implementation of a @c Pass to perform the actual
   * transformation.
   *
   * By default this method terverses recursively the dependence-tree and returns true immediately,
   * once a pass applied on a value.
   */
  virtual bool handleValue(SmartPtr<Value> &value) = 0;
};


/**
 * A pass manager collect passes and applies them on the dependence tree of a byte-code.
 *
 * @ingroup bci
 */
class PassManager
{
protected:
  /** Holds the list of passes to apply. */
  std::list<Pass *> _passes;


public:
  /**
   * Entry point.
   */
  void apply(Code &bytecode);

  /**
   * Adds a pass to the manager.
   */
  void addPass(Pass *pass);


protected:
  /**
   * Traverses the dependence tree.
   */
  bool handleValue(SmartPtr<Value> &value);

  /**
   * Applies all passes on the given value unless they can not be applied anymore. If a pass applied
   * on that value, the function returns true.
   */
  bool applyOnValue(SmartPtr<Value> &value);
};


/**
 * This simple pass tries to swap LHS and RHS values of ADD and MUL
 * instructions, so that the immediate value (if there is one) is
 * on the RHS expression side.
 *
 * @ingroup bci
 */
class ImmediateValueRHSPass : public Pass {
public:
  /**
   * Implements the actual pass.
   */
  virtual bool handleValue(SmartPtr<Value> &value);
};


/**
 * This pass tries to encode PUSH instructions on the RHS side on an expression (like ADD,
 * SUB, MUL, DIV and POW) as an immediate value.
 *
 * This pass works best together with the @c ImmediateValueRHSPass, which tries to encode PUSH
 * instructions as the RHS value of ADD and MUL instructions.
 *
 * @ingroup bci
 */
class ImmediateValuePass : public Pass
{
public:
  /**
   * Implements the actual pass.
   */
  virtual bool handleValue(SmartPtr<Value> &value);
};


/**
 * Removes expressions like "X + 0", "X - 0", "X * 1" and "X / 1".
 *
 * @ingroup bci
 */
class RemoveUnitsPass : public Pass
{
public:
  /**
   * Implements the actual pass.
   */
  virtual bool handleValue(SmartPtr<Value> &value);
};


/**
 * Performs simple constant folding on functions called with constants.
 *
 * @ingroup bci
 */
class ConstantFoldingPass : public Pass
{
public:
  /**
   * Implements the actual pass.
   */
  virtual bool handleValue(SmartPtr<Value> &value);
};


/**
 * Replaces POW instructions with integer exponent < 5 by product.
 *
 * @ingroup bci
 */
class IPowPass : public Pass
{
public:
    /** Implements the actual pass. */
    virtual bool handleValue(SmartPtr<Value> &value);
};


/**
 * Replaces "PUSH 0; STORE X" with "STORE_ZERO X".
 *
 * @ingroup bci
 */
class ZeroStorePass : public Pass
{
public:
  /**
   * Implements the actual pass.
   */
  virtual bool handleValue(SmartPtr<Value> &value);
};


/**
 * Propergates constants to the outermost right position.
 *
 * I.e. \f$((x + a) + y) \rightarrow ((x + y) + a)\f$, this is particulary useful in conjunction
 * with the @c InstructionCanonization pass.
 *
 * @ingroup bci
 */
class ConstantPropagation : public Pass
{
public:
  virtual bool handleValue(SmartPtr<Value> &value);
};


/**
 * Instruction canonization. X; Y; MUL -1; ADD; -> X; Y; SUB;
 *
 * @ingroup bci
 */
class InstructionCanonization : public Pass
{
public:
  virtual bool handleValue(SmartPtr<Value> &value);
};


}
}
}

#endif //
