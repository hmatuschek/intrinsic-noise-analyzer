#ifndef __INA_PARSER_EXPR_WRITER_HH__
#define __INA_PARSER_EXPR_WRITER_HH__

#include <ginac/ginac.h>
#include <ast/ast.hh>
#include <iostream>


namespace Fluc {
namespace Parser {
namespace Expr {

/** Serializes GiNaC::ex expressions into strings.
 * This uses a much nicer serialization than the one implemented by GiNaC. It also can only
 * serialize those expressions that can be parsed using the Fluc::Parser::Expr parsers.
 */
class Writer :
    public GiNaC::visitor, public GiNaC::basic::visitor, public GiNaC::add::visitor,
    public GiNaC::mul::visitor, public GiNaC::power::visitor, public GiNaC::function::visitor,
    public GiNaC::symbol::visitor, public GiNaC::numeric::visitor
{
protected:
  /** Holds a constant weak reference to an @c Ast::Scope instance for symbol resolution. */
  const Ast::Scope &_scope;
  /** Holds a weak reference to the stream, the expression is serialized into. */
  std::ostream &_output;
  /** Holds the current precedence. */
  int _current_precedence;

public:
  /** Constructor. */
  Writer(const Ast::Scope &scope, std::ostream &output);

  /** Serialize sums. */
  virtual void visit(const GiNaC::add &node);
  /** Serialize products. */
  virtual void visit(const GiNaC::mul &node);
  /** Serialize powers. */
  virtual void visit(const GiNaC::power &node);
  /** Serialize function calls. */
  virtual void visit(const GiNaC::function &node);
  /** Serialize symbols. */
  virtual void visit(const GiNaC::symbol &node);
  /** Serialize numerical values. */
  virtual void visit(const GiNaC::numeric &node);
  /** Throws an exception signalling that there is an unknown expression type. */
  virtual void visit(const GiNaC::basic &node);

public:
  static void write(GiNaC::ex expression, const Ast::Scope &scope, std::ostream &stream);
};

}
}
}

#endif // __INA_PARSER_EXPR_WRITER_HH__
