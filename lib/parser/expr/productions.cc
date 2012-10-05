#include "productions.hh"
#include "lexer.hh"

using namespace iNA;
using namespace iNA::Parser::Expr;


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
        new Parser::OptionalProduction(new iNA::Parser::TokenProduction(T_MINUS)));

  // (INTEGER | FLOAT)
  this->elements.push_back(
        new Parser::AltProduction(
          2, new Parser::TokenProduction(T_INTEGER), new iNA::Parser::TokenProduction(T_FLOAT)));
}

NumberProduction *NumberProduction::instance = 0;

Parser::Production *
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
      new iNA::Parser::Production(
        3, ProductExpressionProduction::get(),
        new Parser::AltProduction(
          2, new Parser::TokenProduction(T_PLUS), new Parser::TokenProduction(T_MINUS)),
        ExpressionProduction::get());

  this->alternatives[1] = ProductExpressionProduction::get();
}

ExpressionProduction *ExpressionProduction::instance = 0;

Parser::Production *
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
 *   (PowerExpression ("*" | "/") ProductExpression) | PowerExpression;
 * ******************************************************************************************** */
ProductExpressionProduction::ProductExpressionProduction()
  : AltProduction()
{
  ProductExpressionProduction::instance = this;

  this->alternatives.resize(2);

  this->alternatives[0] =
      new Parser::Production(
        3, PowerExpressionProduction::get(),
        new Parser::AltProduction(
          2, new Parser::TokenProduction(T_TIMES), new Parser::TokenProduction(T_DIVIVE)),
        ProductExpressionProduction::get());

  this->alternatives[1] = PowerExpressionProduction::get();
}

ProductExpressionProduction *ProductExpressionProduction::instance = 0;

Parser::Production *
ProductExpressionProduction::get()
{
  if (0 == ProductExpressionProduction::instance)
    new ProductExpressionProduction();

  return ProductExpressionProduction::instance;
}


/* ******************************************************************************************** *
 * Implementation of PowerExpressionProduction:
 *
 * PowerExpression =
 *   (AtomicExpression ("**" | "^") PowerExpression) | AtomicExpression;
 * ******************************************************************************************** */
PowerExpressionProduction::PowerExpressionProduction()
  : AltProduction()
{
  PowerExpressionProduction::instance = this;

  this->alternatives.resize(2);
  this->alternatives[0] =
      new Parser::Production(
        3, AtomicExpressionProduction::get(),new Parser::TokenProduction(T_POWER), this);
  this->alternatives[1] = AtomicExpressionProduction::get();
}

PowerExpressionProduction *PowerExpressionProduction::instance = 0;

Parser::Production *
PowerExpressionProduction::get()
{
  if (0 == PowerExpressionProduction::instance)
    return new PowerExpressionProduction();
  return PowerExpressionProduction::instance;
}


/* ******************************************************************************************** *
 * Implementation of FunctionCallArgumentsProduction:
 *
 * FunctionCallArguments =
 *   Expression [, FunctionCallArguments];
 * ******************************************************************************************** */
FunctionCallArgumentsProduction::FunctionCallArgumentsProduction()
  : Production()
{
  FunctionCallArgumentsProduction::instance = this;

  this->elements.push_back(ExpressionProduction::get());

  this->elements.push_back(
        new Parser::OptionalProduction(
          new Parser::Production(
            2, new Parser::TokenProduction(T_COMMA), this)));
}

FunctionCallArgumentsProduction *FunctionCallArgumentsProduction::instance = 0;
Parser::Production *
FunctionCallArgumentsProduction::get()
{
  if (0 == FunctionCallArgumentsProduction::instance)
    return new FunctionCallArgumentsProduction();
  return FunctionCallArgumentsProduction::instance;
}


/* ******************************************************************************************** *
 * Implementation of FunctionCallProduction:
 *
 * FunctionCall =
 *   Identifier "(" FunctionCallArguments ")";
 * ******************************************************************************************** */
FunctionCallProduction::FunctionCallProduction()
  : Production()
{
  FunctionCallProduction::instance = this;

  this->elements.push_back(new Parser::TokenProduction(T_IDENTIFIER));
  this->elements.push_back(new Parser::TokenProduction(T_LPAR));
  this->elements.push_back(FunctionCallArgumentsProduction::get());
  this->elements.push_back(new Parser::TokenProduction(T_RPAR));
}

FunctionCallProduction *FunctionCallProduction::instance = 0;
Parser::Production *
FunctionCallProduction::get()
{
  if (0 == FunctionCallProduction::instance)
    return new FunctionCallProduction();
  return FunctionCallProduction::instance;
}


/* ******************************************************************************************** *
 * Implementation of AtomicExpressionProduction:
 *
 * AtomicExpression =
 *   Number | FunctionCall | Identifier | ("(" Expression ")") | "-" AtomicExpression;
 * ******************************************************************************************** */
AtomicExpressionProduction::AtomicExpressionProduction()
  : AltProduction()
{
  AtomicExpressionProduction::instance = this;

  this->alternatives.resize(5);

  this->alternatives[0] = NumberProduction::get();
  this->alternatives[1] = FunctionCallProduction::get();
  this->alternatives[2] = new Parser::TokenProduction(T_IDENTIFIER);
  this->alternatives[3] = new Parser::Production(
        3, new Parser::TokenProduction(T_LPAR),
        ExpressionProduction::get(),
        new Parser::TokenProduction(T_RPAR));
  this->alternatives[4] = new Parser::Production(
        2, new Parser::TokenProduction(T_MINUS), this);
}

AtomicExpressionProduction *AtomicExpressionProduction::instance = 0;

Parser::Production *
AtomicExpressionProduction::get()
{
  if (0 == AtomicExpressionProduction::instance)
    new AtomicExpressionProduction();

  return AtomicExpressionProduction::instance;
}


/* ******************************************************************************************** *
 * ExpressionGrammar =
 *    Expression END_OF_LINE
 * ******************************************************************************************** */
ExpressionGrammar::ExpressionGrammar()
  : Production()
{
  ExpressionGrammar::instance = this;
  elements.push_back(ExpressionProduction::get());
  elements.push_back(new TokenProduction(Token::END_OF_INPUT));
}

ExpressionGrammar *ExpressionGrammar::instance = 0;

Parser::Production *
ExpressionGrammar::get() {
  if (0 == ExpressionGrammar::instance) {
    return new ExpressionGrammar();
  }
  return ExpressionGrammar::instance;
}
