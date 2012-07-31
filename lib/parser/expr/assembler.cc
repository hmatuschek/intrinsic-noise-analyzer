#include "assembler.hh"

using namespace Fluc;
using namespace Fluc::Parser::Expr;


GiNaC::ex
Assembler::processExpression(Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer)
{
  /* Expression =               : expr
   *   (                          : expr[0]
   *     ProductExpression          : expr[0][0]
   *     ("+"|"-")                  : expr[0][1]
   *     Expression) |              : expr[0][2]
   *    ProductExpression;        : expr[0] */

  if (0 == expr.getAltIdx()) {
    GiNaC::ex lhs = processProduct(expr[0][0], ctx, lexer);
    GiNaC::ex rhs = processExpression(expr[0][2], ctx, lexer);
    if (0 == expr[0][1].getAltIdx()) {
      return lhs + rhs;
    } else {
      return lhs - rhs;
    }
  }

  return processProduct(expr[0], ctx, lexer);
}


GiNaC::ex
Assembler::processProduct(Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer)
{
  /* ProductExpression =         : expr
   *   (                         : expr[0]
   *     PowerExpression          : expr[0][0]
   *     ("*" | "/")               : expr[0][1]
   *     ProductExpression) |      : expr[0][2]
   *   PowerExpression;         : expr[0]   */

   if (0 == expr.getAltIdx()) {
     GiNaC::ex lhs = processPower(expr[0][0], ctx, lexer);
     GiNaC::ex rhs = processProduct(expr[0][2], ctx, lexer);
     if (0 == expr[0][1].getAltIdx()) {
       return lhs + rhs;
     } else {
       return lhs - rhs;
     }
   }

   return processPower(expr[0], ctx, lexer);
}


GiNaC::ex
Assembler::processPower(Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer)
{
  /* PowerExpression =
   *   (AtomicExpression ("^"|"**") PowerExpression) | AtomicExpression. */

  if (0 == expr.getAltIdx()) {
    GiNaC::ex lhs = processAtomic(expr[0][0], ctx, lexer);
    GiNaC::ex rhs = processPower(expr[0][2], ctx, lexer);
    return GiNaC::pow(lhs, rhs);
  }

  return processAtomic(expr[0], ctx, lexer);
}


GiNaC::ex
Assembler::processAtomic(Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer)
{
  /* AtomicExpression =       : expr
   *   Number |                 : expr[0]
   *   Identifier |             : expr[0]
   *   FunctionCall |           : expr[0]
   *   ("(" Expression ")");    : expr[0]; Expression : expr[0][1]
   *   "-" AtomicExpression     : expr[0]; AtomicExpression -> expr[0][1] */

  if (0 == expr.getAltIdx()) {
    return processNumber(expr[0], lexer);
  } else if (1 == expr.getAltIdx()) {
    std::string identifier = lexer[expr[0].getTokenIdx()].getValue();
    return ctx.resolve(identifier);
  } else if (2 == expr.getAltIdx()) {
    return processFunctionCall(expr[0], ctx, lexer);
  } else if (3 == expr.getAltIdx()) {
    return processExpression(expr[0][1], ctx, lexer);
  } else {
    return -processAtomic(expr[0][1], ctx, lexer);
  }
}


GiNaC::ex
Assembler::processFunctionCall(Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer)
{
  /* FunctionCall =            : expr
   *   Identifier                : expr[0]
   *   "("
   *   FunctionCallArguments     : expr[2]
   *   ")";  */
  std::string name = lexer[expr[0].getTokenIdx()].getValue();
  std::vector<GiNaC::ex> args; processFunctionCallArguments(expr[2], ctx, lexer, args);

  if ("abs" == name) {
    if (1 != args.size()) {
      SBMLParserError err;
      err << "abs() takes exactly one argument, given " << args.size();
      throw err;
    }
    return GiNaC::abs(args[0]);
  }

  SBMLParserError err;
  err << "Unkown function: " << name;
  throw err;
}


void
Assembler::processFunctionCallArguments(
  Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer, std::vector<GiNaC::ex> &args)
{
  /* FunctionCallArguments =
   *   Expression [ "," FunctionCallArguments] */
  args.push_back(processExpression(expr[0], ctx, lexer));
  if (expr[1].matched()) {
    processFunctionCallArguments(expr[1][0][1], ctx, lexer, args);
  }
}


GiNaC::ex
Assembler::processNumber(Utils::ConcreteSyntaxTree &num, Utils::Lexer &lexer)
{
  /* Number =
   *   ["-"] (INTEGER | FLOAT); */
  double value = 1.0;

  if (num[0].matched()) {
    value = -1.0;
  }

  if (0 == num[1].getAltIdx()) {
    value *= toNumber<int>(lexer[num[1][0].getTokenIdx()].getValue());
  } else {
    value *= toNumber<double>(lexer[num[1][0].getTokenIdx()].getValue());
  }

  return value;
}
