#ifndef __INA_PARSER_EXPR_IR_HH__
#define __INA_PARSER_EXPR_IR_HH__

#include <ginac/ginac.h>
#include "smartptr.hh"
#include <vector>
#include <complex>


namespace iNA {
namespace Parser {
namespace Expr {


/** Generic node of expression intermediate representation. */
class Node {
public:
  /** Lists all expression types. */
  typedef enum {
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DIVISION,
    POWER,
    NEGATION,
    FUNCTION,
    SYMBOL,
    VALUE_INTEGER,
    VALUE_REAL,
    VALUE_COMPLEX
  } NodeType;

  /** Lists all known functions. */
  typedef enum {
    FUNCTION_EXP,
    FUNCTION_LOG
  } FunctionId;


protected:
  /** Hidden constructor. */
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
  static SmartPtr<Node> createDiv(SmartPtr<Node> lhs, SmartPtr<Node> rhs);
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


}
}
}

#endif
