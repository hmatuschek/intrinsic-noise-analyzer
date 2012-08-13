#include "ginac2formula.hh"
#include "tinytex.hh"


using namespace Fluc;


Ginac2Formula::Ginac2Formula(Fluc::Ast::Scope &scope, bool tex_names)
  : _stack(), _scope(scope), _tex_names(tex_names), _current_precedence(0)
{
  // pass...
}


void
Ginac2Formula::visit(const GiNaC::symbol &node)
{
  if (! _scope.hasDefinition(node.get_name())) {
    _stack.push_back(new MathText(node.get_name().c_str()));
    return;
  }

  Ast::Definition *def = _scope.getDefinition(node.get_name());

  Ast::VariableDefinition *var = 0;
  if (0 == (var = dynamic_cast<Ast::VariableDefinition *>(def))) {
      _stack.push_back(new MathText(node.get_name().c_str()));
      return;
  }

  if (var->hasName()) {
    if (_tex_names) {
      _stack.push_back(TinyTex::parse(var->getName()));
      return;
    } else {
      _stack.push_back(new MathText(var->getName().c_str()));
    }
  }

  _stack.push_back(new MathText(var->getIdentifier().c_str()));
}


void
Ginac2Formula::visit(const GiNaC::numeric &node)
{
  if (node.is_integer()) {
    _stack.push_back(new MathText(QString("%1").arg(node.to_int())));
  } else {
    _stack.push_back(new MathText(QString("%1").arg(node.to_double())));
  }
}


void
Ginac2Formula::visit(const GiNaC::add &node)
{
  int old_precedence = _current_precedence;
  _current_precedence = 1;

  MathFormula *formula = new MathFormula();

  if (old_precedence > _current_precedence) {
    formula->appendItem(new MathText("("));
  }

  // Handle summands:
  for (size_t i=0; i<node.nops(); i++) {
    node.op(i).accept(*this);
  }

  formula->appendItem(_stack.back()); _stack.pop_back();
  for (size_t i=1; i<node.nops(); i++) {
    formula->appendItem(new MathText("+"));
    formula->appendItem(_stack.back()); _stack.pop_back();
  }

  if (old_precedence > _current_precedence) {
    formula->appendItem(new MathText(")"));
  }
  _current_precedence = old_precedence;

  _stack.push_back(formula);
}


void
Ginac2Formula::visit(const GiNaC::mul &node)
{
  int old_precedence = _current_precedence;
  _current_precedence = 2;

  MathFormula *formula = new MathFormula();

  if (old_precedence > _current_precedence) {
    formula->appendItem(new MathText("("));
  }

  // Handle summands:
  for (size_t i=0; i<node.nops(); i++) {
    node.op(i).accept(*this);
  }

  formula->appendItem(_stack.back()); _stack.pop_back();
  for (size_t i=1; i<node.nops(); i++) {
    formula->appendItem(new MathText("*"));
    formula->appendItem(_stack.back()); _stack.pop_back();
  }

  if (old_precedence > _current_precedence) {
    formula->appendItem(new MathText(")"));
  }
  _current_precedence = old_precedence;

  _stack.push_back(formula);
}


void
Ginac2Formula::visit(const GiNaC::power &node)
{
  int old_precedence = _current_precedence;
  _current_precedence = 3;

  // Handle base & exp:
  node.op(0).accept(*this);
  node.op(1).accept(*this);

  MathFormulaItem *exponent = _stack.back(); _stack.pop_back();
  MathFormulaItem *base = _stack.back(); _stack.pop_back();
  _stack.push_back(new MathSup(base, exponent));

  _current_precedence = old_precedence;
}


MathFormulaItem *
Ginac2Formula::getFormula()
{
  MathFormulaItem *item = _stack.back(); _stack.pop_back();
  return item;
}

