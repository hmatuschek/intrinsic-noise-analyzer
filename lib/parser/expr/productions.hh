#ifndef __INA_PARSER_EXPR_PRODUCTIONS_HH__
#define __INA_PARSER_EXPR_PRODUCTIONS_HH__

#include "../lexer.hh"
#include "../production.hh"


namespace iNA {
namespace Parser {
namespace Expr {

/** Helper production to unify integers and floats to numbers with signs.
 * Number =  ["-"] (INTEGER | FLOAT ); */
class NumberProduction : public Parser::Production
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


/** FunctionCallArguments =
 *   Expression [, FunctionCallArguments] */
class FunctionCallArgumentsProduction : public Parser::Production
{
protected:
  /** Hidden constructor. */
  FunctionCallArgumentsProduction();

public:
  /** Factory method. */
  static iNA::Parser::Production *get();

protected:
  /** Holds the singleton instance. */
  static FunctionCallArgumentsProduction *instance;
};


/** FunctionCall =
 *   Identifier "(" FunctionCallArguments ")"; */
class FunctionCallProduction : public Parser::Production
{
protected:
  /** Hidden constructor. */
  FunctionCallProduction();

public:
  /** Factory method. */
  static iNA::Parser::Production *get();

protected:
  /** Singleton instance. */
  static FunctionCallProduction *instance;
};


/** AtomicExpression =
 *   Number | Identifier | FunctionCall | ("(" Expression ")") | "-" AtomicExpression; */
class AtomicExpressionProduction : public Parser::AltProduction
{
protected:
  /** Hidden constructor. */
  AtomicExpressionProduction();

public:
  /** Factory method. */
  static iNA::Parser::Production *get();

private:
  /** Singleton instance. */
  static AtomicExpressionProduction *instance;
};


/** PowerExpression =
 *   (AtomicExpression ("^"|"**") PowerExpression) | AtomicExpression. */
class PowerExpressionProduction : public Parser::AltProduction
{
protected:
  /** Hidden constructor. */
  PowerExpressionProduction();

public:
  /** Factory method. */
  static iNA::Parser::Production *get();

protected:
  /** Holds the singleton instance. */
  static PowerExpressionProduction *instance;
};


/** ProductExpression =
 *   (PowerExpression ("*" | "/") ProductExpression) | PowerExpression; */
class ProductExpressionProduction : public Parser::Production
{
protected:
  /** Hidden constructor. */
  ProductExpressionProduction();

public:
  /** Factory method. */
  static iNA::Parser::Production *get();

private:
  /** Singleton instance. */
  static ProductExpressionProduction *instance;
};


/** Expression =
 *   (ProductExpression ("+"|"-") Expression) | ProductExpression; */
class ExpressionProduction : public Parser::Production
{
protected:
  /** Hidden constructor. */
  ExpressionProduction();

public:
  /** factory method. */
  static iNA::Parser::Production *get();

private:
  /** Singleton instance. */
  static ExpressionProduction *instance;
};


/** Grammar =  Expression END_OF_FILE; */
class ExpressionGrammar : public Parser::Production
{
protected:
  /** Hidden constructor. */
  ExpressionGrammar();

public:
  /** factory method. */
  static iNA::Parser::Production *get();

private:
  /** Singleton instance. */
  static ExpressionGrammar *instance;
};

}
}
}
#endif // __INA_PARSER_EXPR_PRODUCTIONS_HH__
