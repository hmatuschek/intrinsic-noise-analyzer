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
  AstNode(Type type, AstNode *lhs, AstNode *rhs);
  AstNode(Function function, AstNode *arg1);

public:
  static AstNode *newInteger(long value);
  static AstNode *newFloat(double value);
  static AstNode *newComplex(double real, double imag);
  static AstNode *newComplex(const std::complex<double> &value);
  static AstNode *newSymbol(const std::string &name);
  static AstNode *newAdd(AstNode *lhs, AstNode *rhs);
  static AstNode *newSub(AstNode *lhs, AstNode *rhs);
  static AstNode *newMul(AstNode *lhs, AstNode *rhs);
  static AstNode *newDiv(AstNode *lhs, AstNode *rhs);
  static AstNode *newPow(AstNode *lhs, AstNode *rhs);

private:
  Type _type;
  long _integer;
  double _float;
  std::complex<double> _complex;

  Function _function_id;
  std::vector<AstNode *> _arguments;
};
}
}
}
#endif // __INA_PARSER_EXPR2_ASTNODE_HH__
