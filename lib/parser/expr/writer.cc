#include "writer.hh"
#include "exception.hh"

using namespace iNA;
using namespace iNA::Parser::Expr;


Writer::Writer(const Ast::Scope &scope, std::ostream &output)
  : GiNaC::visitor(), _scope(scope), _output(output), _current_precedence(0)
{
  // Pass...
}


void
Writer::visit(const GiNaC::add &node)
{
  int old_precedence = _current_precedence;
  _current_precedence = 1;

  if (old_precedence > _current_precedence) { _output << "("; }
  if (0 < node.nops()) {
    node.op(0).accept(*this);
    for (size_t i=1; i<node.nops(); i++) {
      _output << "+";
      node.op(i).accept(*this);
    }
  }
  if (old_precedence > _current_precedence) { _output << ")"; }

  _current_precedence = old_precedence;
}


void
Writer::visit(const GiNaC::mul &node)
{
  int old_precedence = _current_precedence; _current_precedence = 2;
  if (old_precedence > _current_precedence) { _output << "("; }

  if (0 < node.nops()) {
    node.op(0).accept(*this);
    for (size_t i=1; i<node.nops(); i++) {
      _output << "*";
      node.op(i).accept(*this);
    }
  }

  if (old_precedence > _current_precedence) { _output << ")"; }
  _current_precedence = old_precedence;
}


void
Writer::visit(const GiNaC::power &node)
{
  int old_precedence = _current_precedence; _current_precedence = 3;
  if (old_precedence >= _current_precedence) { _output << "("; }

  node.op(0).accept(*this);
  _output << "^";
  node.op(1).accept(*this);

  if (old_precedence >= _current_precedence) { _output << ")"; }
  _current_precedence = old_precedence;
}


void
Writer::visit(const GiNaC::function &node)
{
  // Dispatch by function serial:
  if (node.get_serial() == GiNaC::abs_SERIAL::serial) {
    _output << "abs("; node.op(0).accept(*this); _output << ")"; return;
  }

  if (node.get_serial() == GiNaC::exp_SERIAL::serial) {
    _output << "exp("; node.op(0).accept(*this); _output << ")"; return;
  }

  if (node.get_serial() == GiNaC::log_SERIAL::serial) {
    _output << "log("; node.op(0).accept(*this); _output << ")"; return;
  }

  InternalError err;
  err << "Can not serialize function call " << node << ": Unknown function.";
  throw err;
}


void
Writer::visit(const GiNaC::symbol &node) {
  if (! _scope.hasVariable(node)) {
    SymbolError err;
    err << "Can not serialize symbol " << node << ": Symbol not defined in scope.";
    throw err;
  }

  Ast::VariableDefinition *var = _scope.getVariable(node);
  _output << var->getIdentifier();
}

void
Writer::visit(const GiNaC::numeric &node) {
  if (node.is_integer()) {
    _output << node.to_long();
  } else if (node.is_real()) {
    _output << node.to_double();
  }
}


void
Writer::visit(const GiNaC::basic &node)
{
  InternalError err; err << "Can not serialize expression " << node << ": unknown expression type.";
  throw err;
}


void
Writer::write(GiNaC::ex expression, const Ast::Scope &scope, std::ostream &stream)
{
  Writer writer(scope, stream); expression.accept(writer);
}
