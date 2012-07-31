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
   *     AtomicExpression          : expr[0][0]
   *     ("*" | "/")               : expr[0][1]
   *     ProductExpression) |      : expr[0][2]
   *   AtomicExpression;         : expr[0]   */

   if (0 == expr.getAltIdx()) {
     GiNaC::ex lhs = processAtomic(expr[0][0], ctx, lexer);
     GiNaC::ex rhs = processProduct(expr[0][2], ctx, lexer);
     if (0 == expr[0][1].getAltIdx()) {
       return lhs + rhs;
     } else {
       return lhs - rhs;
     }
   }

   return processAtomic(expr[0], ctx, lexer);
}


GiNaC::ex
Assembler::processAtomic(Utils::ConcreteSyntaxTree &expr, Context &ctx, Utils::Lexer &lexer)
{
  /* AtomicExpression =       : expr
   *   Identifier |           : expr[0]
   *   Number |               : expr[0]
   *   ("(" Expression ")");  : expr[0]; Expression : expr[0][1]; */

  if (0 == expr.getAltIdx()) {
    std::string identifier = lexer[expr[0].getTokenIdx()].getValue();
    return ctx.resolve(identifier);
  } else if (1 == expr.getAltIdx()) {
    return processNumber(expr[0], lexer);
  } else {
    return processExpression(expr[0][1], ctx, lexer);
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
