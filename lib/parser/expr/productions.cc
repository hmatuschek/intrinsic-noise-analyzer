#include "productions.hh"
#include "lexer.hh"

using namespace Fluc;
using namespace Fluc::Parser::Expr;


/* ******************************************************************************************** *
 * Implementation of NumberProduction:
 *
 * Number =
 *   ["-"] (INTEGER | FLOAT);
 * ******************************************************************************************** */
NumberProduction::NumberProduction()
  : Production()
{
  // Register singleton instance:
  NumberProduction::instance = this;

  // Assemble grammar:
  // [-]
  this->elements.push_back(
        new Utils::OptionalProduction(new Utils::TokenProduction(T_MINUS)));

  // (INTEGER | FLOAT)
  this->elements.push_back(
        new Utils::AltProduction(
          2, new Utils::TokenProduction(T_INTEGER), new Utils::TokenProduction(T_FLOAT)));
}

NumberProduction *NumberProduction::instance = 0;

Utils::Production *
NumberProduction::get()
{
  if (0 == NumberProduction::instance)
    new NumberProduction();

  return NumberProduction::instance;
}


/* ******************************************************************************************** *
 * Implementation of ExpressionProduction:
 *
 * Expression =
 *   (ProductExpression ("+"|"-") Expression) | ProductExpression;
 * ******************************************************************************************** */
ExpressionProduction::ExpressionProduction()
  : AltProduction()
{
  ExpressionProduction::instance = this;

  this->alternatives.resize(2);

  this->alternatives[0] =
      new Utils::Production(
        3, ProductExpressionProduction::get(),
        new Utils::AltProduction(
          2, new Utils::TokenProduction(T_PLUS), new Utils::TokenProduction(T_MINUS)),
        ExpressionProduction::get());

  this->alternatives[1] = ProductExpressionProduction::get();
}

ExpressionProduction *ExpressionProduction::instance = 0;

Utils::Production *
ExpressionProduction::get()
{
  if (0 == ExpressionProduction::instance)
    new ExpressionProduction();

  return ExpressionProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of ProductExpressionProduction:
 *
 * ProductExpression =
 *   (AtomicExpression ("*" | "/") ProductExpression) | AtomicExpression;
 * ******************************************************************************************** */
ProductExpressionProduction::ProductExpressionProduction()
  : AltProduction()
{
  ProductExpressionProduction::instance = this;

  this->alternatives.resize(2);

  this->alternatives[0] =
      new Utils::Production(
        3, AtomicExpressionProduction::get(),
        new Utils::AltProduction(
          2, new Utils::TokenProduction(T_TIMES), new Utils::TokenProduction(T_DIVIVE)),
        ProductExpressionProduction::get());

  this->alternatives[1] = AtomicExpressionProduction::get();
}

ProductExpressionProduction *ProductExpressionProduction::instance = 0;

Utils::Production *
ProductExpressionProduction::get()
{
  if (0 == ProductExpressionProduction::instance)
    new ProductExpressionProduction();

  return ProductExpressionProduction::instance;
}



/* ******************************************************************************************** *
 * Implementation of AtomicExpressionProduction:
 *
 * AtomicExpression =
 *   Identifier | Number | ("(" Expression ")");
 * ******************************************************************************************** */
AtomicExpressionProduction::AtomicExpressionProduction()
  : AltProduction()
{
  AtomicExpressionProduction::instance = this;

  this->alternatives.resize(3);

  this->alternatives[0] = new Utils::TokenProduction(T_IDENTIFIER);
  this->alternatives[1] = NumberProduction::get();
  this->alternatives[2] = new Utils::Production(
        3, new Utils::TokenProduction(T_LPAR),
        ExpressionProduction::get(),
        new Utils::TokenProduction(T_RPAR));
}

AtomicExpressionProduction *AtomicExpressionProduction::instance = 0;

Utils::Production *
AtomicExpressionProduction::get()
{
  if (0 == AtomicExpressionProduction::instance)
    new AtomicExpressionProduction();

  return AtomicExpressionProduction::instance;
}
