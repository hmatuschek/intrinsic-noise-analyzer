#ifndef __INA_PARSER_EXPR_PRODUCTIONS_HH__
#define __INA_PARSER_EXPR_PRODUCTIONS_HH__

#include <utils/lexer.hh>
#include <utils/parser.hh>


namespace Fluc {
namespace Parser {
namespace Expr {


/**
 * Helper production to unify integers and floats to numbers with signs.
 *
 * Number =
 *   ["-"] (INTEGER | FLOAT );
 */
class NumberProduction : public Utils::Production
{
protected:
  /** Hidden constructor, use factory method @c get to get an instance. */
  NumberProduction();

public:
  /** Factory method. */
  static Production *get();

private:
  /** Singleton instance. */
  static NumberProduction *instance;
};


/**
 * FunctionCallArguments =
 *   Expression [, FunctionCallArguments]
 */
class FunctionCallArgumentsProduction : public Utils::Production
{
protected:
  /** Hidden constructor. */
  FunctionCallArgumentsProduction();

public:
  /** Factory method. */
  static Utils::Production *get();

protected:
  static FunctionCallArgumentsProduction *instance;
};


/**
 * FunctionCall =
 *   Identifier "(" FunctionCallArguments ")";
 */
class FunctionCallProduction : public Utils::Production
{
protected:
  /** Hidden constructor. */
  FunctionCallProduction();

public:
  /** Factory method. */
  static Utils::Production *get();

protected:
  /** Singleton instance. */
  static FunctionCallProduction *instance;
};


/**
 * AtomicExpression =
 *   Number | Identifier | FunctionCall | ("(" Expression ")") | "-" AtomicExpression;
 */
class AtomicExpressionProduction : public Utils::AltProduction
{
protected:
  /** Hidden constructor. */
  AtomicExpressionProduction();

public:
  /** Factory method. */
  static Utils::Production *get();

private:
  /** Singleton instance. */
  static AtomicExpressionProduction *instance;
};


/**
 * PowerExpression =
 *   (AtomicExpression ("^"|"**") PowerExpression) | AtomicExpression.
 */
class PowerExpressionProduction : public Utils::AltProduction
{
protected:
  /** Hidden constructor. */
  PowerExpressionProduction();

public:
  /** Factory method. */
  static Utils::Production *get();

protected:
  static PowerExpressionProduction *instance;
};


/**
 * ProductExpression =
 *   (PowerExpression ("*" | "/") ProductExpression) | PowerExpression;
 */
class ProductExpressionProduction : public Utils::AltProduction
{
protected:
  /** Hidden constructor. */
  ProductExpressionProduction();

public:
  /** Factory method. */
  static Utils::Production *get();

private:
  /** Singleton instance. */
  static ProductExpressionProduction *instance;
};


/**
 * Expression =
 *   (ProductExpression ("+"|"-") Expression) | ProductExpression;
 */
class ExpressionProduction : public Utils::AltProduction
{
protected:
  /** Hidden constructor. */
  ExpressionProduction();

public:
  /** factory method. */
  static Utils::Production *get();

private:
  /** Singleton instance. */
  static ExpressionProduction *instance;
};


}
}
}
#endif // __INA_PARSER_EXPR_PRODUCTIONS_HH__
