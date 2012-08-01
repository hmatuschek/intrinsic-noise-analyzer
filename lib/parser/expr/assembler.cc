#include "assembler.hh"

using namespace Fluc;
using namespace Fluc::Parser::Expr;


#define ASSERT_UNARY_FUNCTION(name, nargs) if (1 != nargs) { \
  SBMLParserError err; err << name << "() takes exactly one argument, " << nargs << " given."; \
  throw err; }

#define ASSERT_BINARY_FUNCTION(name, nargs) if (2 != nargs) { \
  SBMLParserError err; err << name << "() takes exactly two arguments, " << nargs << " given."; \
  throw err; }



Assembler::Assembler(Ast::Scope *model, Utils::Lexer &lexer)
  : Context(model), _lexer(lexer)
{
  // pass...
}


GiNaC::ex
Assembler::processExpression(Utils::ConcreteSyntaxTree &expr)
{
  /* Expression =               : expr
   *   (                          : expr[0]
   *     ProductExpression          : expr[0][0]
   *     ("+"|"-")                  : expr[0][1]
   *     Expression) |              : expr[0][2]
   *    ProductExpression;        : expr[0] */

  if (0 == expr.getAltIdx()) {
    GiNaC::ex lhs = processProduct(expr[0][0]);
    GiNaC::ex rhs = processExpression(expr[0][2]);
    if (0 == expr[0][1].getAltIdx()) {
      return lhs + rhs;
    } else {
      return lhs - rhs;
    }
  }

  return processProduct(expr[0]);
}


GiNaC::ex
Assembler::processProduct(Utils::ConcreteSyntaxTree &expr)
{
  /* ProductExpression =         : expr
   *   (                         : expr[0]
   *     PowerExpression          : expr[0][0]
   *     ("*" | "/")               : expr[0][1]
   *     ProductExpression) |      : expr[0][2]
   *   PowerExpression;         : expr[0]   */

   if (0 == expr.getAltIdx()) {
     GiNaC::ex lhs = processPower(expr[0][0]);
     GiNaC::ex rhs = processProduct(expr[0][2]);
     if (0 == expr[0][1].getAltIdx()) {
       return lhs + rhs;
     } else {
       return lhs - rhs;
     }
   }

   return processPower(expr[0]);
}


GiNaC::ex
Assembler::processPower(Utils::ConcreteSyntaxTree &expr)
{
  /* PowerExpression =
   *   (AtomicExpression ("^"|"**") PowerExpression) | AtomicExpression. */

  if (0 == expr.getAltIdx()) {
    GiNaC::ex lhs = processAtomic(expr[0][0]);
    GiNaC::ex rhs = processPower(expr[0][2]);
    return GiNaC::pow(lhs, rhs);
  }

  return processAtomic(expr[0]);
}


GiNaC::ex
Assembler::processAtomic(Utils::ConcreteSyntaxTree &expr)
{
  /* AtomicExpression =       : expr
   *   Number |                 : expr[0]
   *   FunctionCall |           : expr[0]
   *   Identifier |             : expr[0]
   *   ("(" Expression ")");    : expr[0]; Expression : expr[0][1]
   *   "-" AtomicExpression     : expr[0]; AtomicExpression -> expr[0][1] */

  if (0 == expr.getAltIdx()) {
    return processNumber(expr[0]);
  } else if (1 == expr.getAltIdx()) {
    return processFunctionCall(expr[0]);
  } else if (2 == expr.getAltIdx()) {
    std::string identifier = _lexer[expr[0].getTokenIdx()].getValue();
    return resolve(identifier);
  } else if (3 == expr.getAltIdx()) {
    return processExpression(expr[0][1]);
  } else {
    return -processAtomic(expr[0][1]);
  }
}


GiNaC::ex
Assembler::processFunctionCall(Utils::ConcreteSyntaxTree &expr)
{
  /* FunctionCall =            : expr
   *   Identifier                : expr[0]
   *   "("
   *   FunctionCallArguments     : expr[2]
   *   ")";  */
  // Get function name
  std::string name = _lexer[expr[0].getTokenIdx()].getValue();
  // Get function arguments
  std::vector<GiNaC::ex> args; processFunctionCallArguments(expr[2], args);

  // Dispatch...
  if ("abs" == name) {
    ASSERT_UNARY_FUNCTION("abs", args.size());
    return GiNaC::abs(args[0]);
  } else if ("acos" == name) {
    ASSERT_UNARY_FUNCTION("acos", args.size());
    return GiNaC::acos(args[0]);
  } else if ("acosh" == name) {
    ASSERT_UNARY_FUNCTION("acosh", args.size());
    return GiNaC::acosh(args[0]);
  } else if ("asin" == name) {
    ASSERT_UNARY_FUNCTION("asin", args.size());
    return GiNaC::asin(args[0]);
  } else if ("asinh" == name) {
    ASSERT_UNARY_FUNCTION("asinh", args.size());
    return GiNaC::asinh(args[0]);
  } else if ("cos" == name) {
    ASSERT_UNARY_FUNCTION("cos", args.size());
    return GiNaC::cos(args[0]);
  } else if ("cosh" == name) {
    ASSERT_UNARY_FUNCTION("cosh", args.size());
    return GiNaC::cosh(args[0]);
  } else if ("exp" == name) {
    ASSERT_UNARY_FUNCTION("exp", args.size());
    return GiNaC::exp(args[0]);
  } else if ("factorial" == name) {
    ASSERT_UNARY_FUNCTION("factorial", args.size());
    return GiNaC::factorial(args[0]);
  } else if ("log" == name) {
    ASSERT_UNARY_FUNCTION("log", args.size());
    return GiNaC::log(args[0]);
  } else if ("power" == name) {
    ASSERT_BINARY_FUNCTION("power", args.size());
    return GiNaC::power(args[0], args[1]);
  } else if ("sqrt" == name) {
    ASSERT_UNARY_FUNCTION("sqrt", args.size());
    return GiNaC::sqrt(args[0]);
  } else if ("sin" == name) {
    ASSERT_UNARY_FUNCTION("sin", args.size());
    return GiNaC::sin(args[0]);
  } else if ("sinh" == name) {
    ASSERT_UNARY_FUNCTION("sinh", args.size());
    return GiNaC::sinh(args[0]);
  } else if ("tan" == name) {
    ASSERT_UNARY_FUNCTION("tan", args.size());
    return GiNaC::tan(args[0]);
  } else if ("tanh" == name) {
    ASSERT_UNARY_FUNCTION("tanh", args.size());
    return GiNaC::tanh(args[0]);
  }

  SBMLParserError err;
  err << "Unkown function: " << name;
  throw err;
}


void
Assembler::processFunctionCallArguments(Utils::ConcreteSyntaxTree &expr, std::vector<GiNaC::ex> &args)
{
  /* FunctionCallArguments =
   *   Expression [ "," FunctionCallArguments] */
  args.push_back(processExpression(expr[0]));
  if (expr[1].matched()) {
    processFunctionCallArguments(expr[1][0][1], args);
  }
}


double
Assembler::processNumber(Utils::ConcreteSyntaxTree &num)
{
  /* Number =
   *   ["-"] (INTEGER | FLOAT); */
  double value = 1.0;

  if (num[0].matched()) {
    value = -1.0;
  }

  if (0 == num[1].getAltIdx()) {
    value *= toNumber<int>(_lexer[num[1][0].getTokenIdx()].getValue());
  } else {
    value *= toNumber<double>(_lexer[num[1][0].getTokenIdx()].getValue());
  }

  return value;
}
