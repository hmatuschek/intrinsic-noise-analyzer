#ifndef __INA_PARSER_EXPR2_ASTNODE_HH__
#define __INA_PARSER_EXPR2_ASTNODE_HH__

#include <complex>
#include <string>
#include <vector>


namespace iNA {
namespace Parser {
namespace Expr2 {

class AstNode {
public:
  typedef enum {
    TYPE_INTEGER, TYPE_FLOAT, TYPE_COMPLEX,
    TYPE_SYMBOL,
    TYPE_ADD, TYPE_SUB, TYPE_MUL, TYPE_DIV, TYPE_POW,
    TYPE_FUNCTION
  } Type;

  typedef enum {
    FUNCTION_EXP,
    FUNCTION_LOG
  } Function;

protected:
  AstNode(long value);
  AstNode(double value);
  AstNode(const std::complex<double> &value);
  AstNode(const std::string &name);
  AstNode(Type type, AstNode *lhs, AstNode *rhs);
  AstNode(Function function, AstNode *arg1);
  ~AstNode();

public:
  /** Factory method. */
  static inline AstNode *newInteger(long value) {
    return new AstNode(value);
  }
  /** Factory method. */
  static inline AstNode *newFloat(double value) {
    return new AstNode(value);
  }
  /** Factory method. */
  static inline AstNode *newComplex(double real, double imag) {
    return new AstNode(std::complex<double>(real, imag));
  }
  /** Factory method. */
  static inline AstNode *newComplex(const std::complex<double> &value) {
    return new AstNode(value);
  }
  /** Factory method. */
  static inline AstNode *newSymbol(const std::string &name) {
    return new AstNode(name);
  }
  /** Factory method. */
  static inline AstNode *newAdd(AstNode *lhs, AstNode *rhs) {
    return new AstNode(TYPE_ADD, lhs, rhs);
  }
  /** Factory method. */
  static inline AstNode *newSub(AstNode *lhs, AstNode *rhs) {
    return new AstNode(TYPE_SUB, lhs, rhs);
  }
  /** Factory method. */
  static inline AstNode *newMul(AstNode *lhs, AstNode *rhs) {
    return new AstNode(TYPE_MUL, lhs, rhs);
  }
  /** Factory method. */
  static inline AstNode *newDiv(AstNode *lhs, AstNode *rhs) {
    return new AstNode(TYPE_DIV, lhs, rhs);
  }
  /** Factory method. */
  static inline AstNode *newPow(AstNode *base, AstNode *expo) {
    return new AstNode(TYPE_POW, base, expo);
  }
  /** Factory method. */
  static inline AstNode *newExp(AstNode *arg) {
    return new AstNode(FUNCTION_EXP, arg);
  }
  /** Factory method. */
  static inline AstNode *newLog(AstNode *arg) {
    return new AstNode(FUNCTION_LOG, arg);
  }

private:
  Type _type;
  long _integer;
  double _float;
  std::complex<double> _complex;
  std::string _identifier;

  Function _function_id;
  std::vector<AstNode *> _arguments;
};
}
}
}
#endif // __INA_PARSER_EXPR2_ASTNODE_HH__
