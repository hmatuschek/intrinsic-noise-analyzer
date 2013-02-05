#ifndef __INA_PARSER_EXPR_IR_HH__
#define __INA_PARSER_EXPR_IR_HH__

#include <ginac/ginac.h>
#include "smartptr.hh"
#include <vector>
#include <complex>
#include "parser.hh"


namespace iNA {
namespace Parser {
namespace Expr {


/** Generic node of expression intermediate representation.
 * This itermediate representation is generated from GiNaC expressions to apply several
 * transformations in order to allow a more readable representation of the expression. The user
 * may not utilize this representation directly, use @c iNA::Parser::serializeExpression
 * to obtain a textual representation of an expression that can be parsed again by the expression
 * parser. */
class Node {
public:
  /** Lists all expression types. */
  typedef enum {
    ADDITION,        ///< +
    SUBTRACTION,     ///< -
    MULTIPLICATION,  ///< *
    DIVISION,        ///< /
    POWER,           ///< **
    NEGATION,        ///< -()
    FUNCTION,        ///< f(x)
    SYMBOL,          ///< x
    VALUE_INTEGER,   ///< 1
    VALUE_REAL,      ///< 1.0
    VALUE_COMPLEX    ///< 1+1j;
  } NodeType;

  /** Lists all known functions. */
  typedef enum {
    FUNCTION_EXP,    ///< exp(x)
    FUNCTION_LOG     ///< log(x)
  } FunctionId;


protected:
  /** Hidden constructor. Use one of the static factory methods to assemble an expression. */
  Node(NodeType type);

public:
  /** Returns true if the node is an addition node. */
  bool isAddNode() const;
  /** Returns true if the node is a subtraction node. */
  bool isSubNode() const;
  /** Returns true if the node is a multiplication node. */
  bool isMulNode() const;
  /** Returns true if the node is a division node. */
  bool isDivNode() const;
  /** Returns true if the node is a power node. */
  bool isPowNode() const;
  /** Returns true if the node is a negation node. */
  bool isNegNode() const;
  /** Returns true if the node is a symbol node. */
  bool isSymbolNode() const;
  /** Returns true if the node is a value node. */
  bool isValueNode() const;
  /** Returns true if the node is an integer node. */
  bool isIntegerNode() const;
  /** Returns true if the node is a real node. */
  bool isRealNode() const;
  /** Returns true if the node is a complex node. */
  bool isComplexNode() const;
  /** Returns true if the node is a function node. */
  bool isFunctionNode() const;
  /** Returns true if the node is an exp() function node. */
  bool isFuncExpNode() const;
  /** Returns true if the node is a log() function node. */
  bool isFuncLogNode() const;
  /** Returns true if the node has child nodes. */
  bool hasArguments() const;
  /** Returns the number of child nodes. */
  size_t numArguments() const;
  /** Returns the i-th child node if the node. */
  SmartPtr<Node> &argument(size_t i);
  /** Returns the value of an integer expression. */
  long &intValue();
  /** Returns the value of a real expression. */
  double &realValue();
  /** Returns the value of a complex expression. */
  std::complex<double> &complexValue();
  /** Returns the symbol instance. */
  GiNaC::symbol symbol() const;
  /** Serializes the node into the given stream. */
  void serialize(std::ostream &stream, Context &ctx);


public:
  /** Constructs a IR expressions from the given GiNaC expression. */
  static SmartPtr<Node> fromExpression(GiNaC::ex expression);
  /** Creates a sum node. */
  static SmartPtr<Node> createAdd(SmartPtr<Node> lhs, SmartPtr<Node> rhs);
  /** Creates a difference node. */
  static SmartPtr<Node> createSub(SmartPtr<Node> lhs, SmartPtr<Node> rhs);
  /** Creates a product node. */
  static SmartPtr<Node> createMul(SmartPtr<Node> lhs, SmartPtr<Node> rhs);
  /** Creates a division node. */
  static SmartPtr<Node> createDiv(SmartPtr<Node> nom, SmartPtr<Node> denom);
  /** Creates a power node. */
  static SmartPtr<Node> createPow(SmartPtr<Node> base, SmartPtr<Node> exponent);
  /** Creates a negation node. */
  static SmartPtr<Node> createNeg(SmartPtr<Node> arg);
  /** Creates a symbol node. */
  static SmartPtr<Node> createSymbol(GiNaC::symbol symbol);
  /** Creates an integer constant. */
  static SmartPtr<Node> createValue(long value);
  /** Creates a real constant. */
  static SmartPtr<Node> createValue(const double &value);
  /** Creates a complex constant. */
  static SmartPtr<Node> createValue(const std::complex<double> &value);
  /** Creates an exp() function call. */
  static SmartPtr<Node> createFuncExp(SmartPtr<Node> arg);
  /** Creates a log() function call. */
  static SmartPtr<Node> createFuncLog(SmartPtr<Node> arg);


protected:
  /** Serializes this node into the given stream. */
  void _serialize(std::ostream &stream, Context &ctx, size_t precedence);


private:
  /** Holds the node type. */
  NodeType _type;
  /** Holds the arguments of functions and operations. */
  std::vector< SmartPtr<Node> > _args;
  /** Holds the function identifier if the node is a function call. */
  FunctionId _function;
  /** Holds the GiNaC symbol if the node is a symbol. */
  GiNaC::symbol _symbol;
  /** Holds the value if the node is an integer. */
  long _integer;
  /** Holds the value if the node is a real value. */
  double _real;
  /** Holds the value if the node is a complex value. */
  std::complex<double> _complex;
};


/** Generic expression pass. */
class Pass {
public:
  /** Destructor. Does nothing at all. */
  virtual ~Pass();

  /** Implement this method to apply any modifications on that. */
  virtual bool apply(SmartPtr<Node> &node) = 0;    
};


/** Applies passes on an expression IR. */
class PassManager {
protected:
  /** Holds the list of passes. */
  std::list<Pass *> _passes;

public:
  /** Destructor, also frees the passes. */
  ~PassManager();
  /** Adds a pass to the list of passes.*/
  void addPass(Pass *pass);
  /** Applies the passes on the node. */
  bool apply(SmartPtr<Node> &node);

protected:
  /** Applies all passes on the given node unless they can not be applied anymore. If a pass
   * was applied on that node, the function returns true. */
  bool applyOnNode(SmartPtr<Node> &value);
};


/** Translates any a^-(b) -> 1/a^b.
 * @todo Move to operation normalization pass. */
class PowerToDivisionPass : public Pass
{
public:
  /** Performs the transformation. */
  bool apply(SmartPtr<Node> &node);
};

/** Remove units pass, tries to remove equivalent expressions.
 * This includes 0+a, 1*a, a/1, a^1, a^0. */
class RemoveUnitsPass : public Pass {
public:
  /** Performs the transformation. */
  bool apply(SmartPtr<Node> &node);

private:
  /** Performs the pass on sums. */
  bool _applyOnAdd(SmartPtr<Node> &node);
  /** Performs the pass on differences. */
  bool _applyOnSub(SmartPtr<Node> &node);
  /** Performs the pass on products. */
  bool _applyOnMul(SmartPtr<Node> &node);
  /** Performs the pass on quotients. */
  bool _applyOnDiv(SmartPtr<Node> &node);
  /** Performs the pass on powers. */
  bool _applyOnPow(SmartPtr<Node> &node);
  /** Performs the pass on negations. */
  bool _applyOnNeg(SmartPtr<Node> &node);
};


/** Translates a+-b -> a-b etc. */
class NormalizeOperatorPass : public Pass
{
public:
  /** Performs the pass on the given expression. */
  bool apply(SmartPtr<Node> &node);

private:
  /** Performs the pass on sums. */
  bool _applyOnAdd(SmartPtr<Node> &node);
  /** Performs the pass on differences .*/
  bool _applyOnSub(SmartPtr<Node> &node);
  /** Performs the pass on products. */
  bool _applyOnMul(SmartPtr<Node> &node);
  /** Performs the pass on quotients. */
  bool _applyOnDiv(SmartPtr<Node> &node);
};


/** Normalizes values.
 * This includes -1 -> (-1), as well as 1.0 -> 1 etc. */
class NormalizeValuesPass : public Pass
{
public:
  /** Applies the pass on the given expression. */
  bool apply(SmartPtr<Node> &node);
};


/** Simply collects some passes to bring the expression into a form for pretty serialization. */
class PrettySerializationTrafo
{
public:
  /** Applies the passes and returns the modified expression. */
  static SmartPtr<Node> &apply(SmartPtr<Node> &expression);
};


}
}
}

#endif
