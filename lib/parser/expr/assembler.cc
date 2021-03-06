#include "assembler.hh"
#include <exception.hh>


using namespace iNA;
using namespace iNA::Parser::Expr;


#define ASSERT_UNARY_FUNCTION(name, nargs) if (1 != nargs) { \
  iNA::Parser::ParserError err; err << name << "() takes exactly one argument, " << nargs << " given."; \
  throw err; }

#define ASSERT_BINARY_FUNCTION(name, nargs) if (2 != nargs) { \
  iNA::Parser::ParserError err; err << name << "() takes exactly two arguments, " << nargs << " given."; \
  throw err; }



Assembler::Assembler(Context &context, Parser::Lexer &lexer)
  : _lexer(lexer), _context(context)
{
  // pass...
}


GiNaC::ex
Assembler::processExpression(Parser::ConcreteSyntaxTree &expr)
{
  /* Expression =               : expr
   *   ProductExpression        : expr[0]
   *   {                        : expr[1]
   *     ("+"|"-")              : expr[1][i][0]
   *     ProductExpression      : expr[1][i][1]
   *   }                                     */

  GiNaC::ex lhs = processProduct(expr[0]);
  for (size_t i=0; i<expr[1].size(); i++) {
    GiNaC::ex rhs = processProduct(expr[1][i][1]);
    if (0 == expr[1][i][0].getAltIdx()) {
      lhs = lhs + rhs;
    } else {
      lhs = lhs - rhs;
    }
  }

  return lhs;
}


GiNaC::ex
Assembler::processProduct(Parser::ConcreteSyntaxTree &expr)
{
  /* ProductExpression =         : expr
   *   PowerExpression           : expr[0]
   *   {                         : expr[1]
   *     ("*" | "/")             : expr[1][i][0]
   *     PowerExpression         : expr[1][i][1]
   *   } */

  GiNaC::ex lhs = processPower(expr[0]);
  // iterate over remaining factors of product (if present)
  for (size_t i=0; i<expr[1].size(); i++) {
    GiNaC::ex rhs = processPower(expr[1][i][1]);
    // Switch operator:
    if (0 == expr[1][i][0].getAltIdx()) {
      lhs = lhs * rhs;
    } else {
      lhs = lhs / rhs;
    }
  }
  // Return result:
  return lhs;
}


GiNaC::ex
Assembler::processPower(Parser::ConcreteSyntaxTree &expr)
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
Assembler::processAtomic(Parser::ConcreteSyntaxTree &expr)
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
    return _context.resolve(identifier);
  } else if (3 == expr.getAltIdx()) {
    return processExpression(expr[0][1]);
  } else {
    return -processAtomic(expr[0][1]);
  }
}


GiNaC::ex
Assembler::processFunctionCall(Parser::ConcreteSyntaxTree &expr)
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
  if ("exp" == name) {
    ASSERT_UNARY_FUNCTION("exp", args.size());
    return GiNaC::exp(args[0]);
  } else if ("log" == name) {
    ASSERT_UNARY_FUNCTION("log", args.size());
    return GiNaC::log(args[0]);
  } else if ("power" == name) {
    ASSERT_BINARY_FUNCTION("power", args.size());
    return GiNaC::power(args[0], args[1]);
  } else if ("sqrt" == name) {
    ASSERT_UNARY_FUNCTION("sqrt", args.size());
    return GiNaC::sqrt(args[0]);
  }

  /*if ("abs" == name) {
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
  } else if ("factorial" == name) {
    ASSERT_UNARY_FUNCTION("factorial", args.size());
    return GiNaC::factorial(args[0]);
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
  }*/

  ParserError err;
  err << "Unkown function: " << name;
  throw err;
}


void
Assembler::processFunctionCallArguments(Parser::ConcreteSyntaxTree &expr, std::vector<GiNaC::ex> &args)
{
  /* FunctionCallArguments =
   *   Expression [ "," FunctionCallArguments] */
  args.push_back(processExpression(expr[0]));
  if (expr[1].matched()) {
    processFunctionCallArguments(expr[1][0][1], args);
  }
}


GiNaC::numeric
Assembler::processNumber(Parser::ConcreteSyntaxTree &num)
{
  /* Number =
   *   ["-"] (INTEGER | FLOAT); */
  // On integer
  if (0 == num[1].getAltIdx()) {
    if (num[0].matched()) {
      return -toNumber<long>(_lexer[num[1][0].getTokenIdx()].getValue());
    }
    return toNumber<long>(_lexer[num[1][0].getTokenIdx()].getValue());
  }

  // On double:
  if (num[0].matched()) {
    return -toNumber<double>(_lexer[num[1][0].getTokenIdx()].getValue());
  }
  return toNumber<double>(_lexer[num[1][0].getTokenIdx()].getValue());
}
