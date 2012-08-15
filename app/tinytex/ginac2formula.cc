#include "ginac2formula.hh"
#include "tinytex.hh"
#include <QGraphicsScene>
#include <QPainter>
#include <QPixmap>
#include "utils/logger.hh"


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
    Fluc::Utils::Message msg = LOG_MESSAGE(Fluc::Utils::Message::INFO);
    msg << "Can not resolve symbol: " << node;
    Fluc::Utils::Logger::get().log(msg);

    _stack.push_back(new MathText(node.get_name().c_str()));
    return;
  }

  Ast::Definition *def = _scope.getDefinition(node.get_name());

  Ast::VariableDefinition *var = 0;
  if (0 == (var = dynamic_cast<Ast::VariableDefinition *>(def))) {
    Fluc::Utils::Message msg = LOG_MESSAGE(Fluc::Utils::Message::INFO);
    msg << "Symbol : " << node << " does not refer to a variable.";
    Fluc::Utils::Logger::get().log(msg);

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
  QList<MathFormulaItem *> numerator, denumerator;

  // Handle factors:
  for (size_t i=0; i<node.nops(); i++) {
    // Process factor
    node.op(i).accept(*this);
    // Add factor to the formula:
    if (GiNaC::is_a<GiNaC::power>(node.op(i))) {
      // Get exponent
      GiNaC::ex exponent = GiNaC::ex_to<GiNaC::power>(node.op(i)).op(1);
      if (GiNaC::is_a<GiNaC::numeric>(exponent)) {
        // get numeric value
        GiNaC::numeric value = GiNaC::ex_to<GiNaC::numeric>(exponent);
        // If expoent is negative integer:
        if (value.is_integer() && value.is_negative()) {
          (1/node.op(i)).accept(*this);
          if (0 < denumerator.size()) { denumerator.append(new MathText(QChar(0x00B7))); }
          denumerator.append(_stack.back()); _stack.pop_back();
        }
      }
    } else {
      node.op(i).accept(*this);
      if (0 < numerator.size()) { numerator.append(new MathText(QChar(0x00B7))); }
      numerator.append(_stack.back()); _stack.pop_back();
    }
  }

  /*
   * Now, assemble formula (product/quotient)
   */
  if ((old_precedence > _current_precedence) && (node.nops()>1)) {
    formula->appendItem(new MathText("("));
  }

  // If there are no numerator elements:
  if (0 == numerator.size()) {
    numerator.append(new MathText("1"));
  }

  // if there are no denumerator elements:
  if (0 == denumerator.size()) {
    for (QList<MathFormulaItem *>::iterator item=numerator.begin(); item!=numerator.end(); item++) {
      formula->appendItem(*item);
    }
  } else {
    MathFormula *num = new MathFormula();
    MathFormula *denum = new MathFormula();
    for (QList<MathFormulaItem *>::iterator item=numerator.begin(); item!=numerator.end(); item++) {
      num->appendItem(*item);
    }
    for (QList<MathFormulaItem *>::iterator item=denumerator.begin(); item!=denumerator.end(); item++) {
      denum->appendItem(*item);
    }
    formula->appendItem(new MathFraction(num, denum));
  }

  if ((old_precedence > _current_precedence) && (node.nops()>1)) {
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


MathFormulaItem *
Ginac2Formula::toFormula(GiNaC::ex expression, Ast::Scope &scope, bool tex_names)
{
  MathFormulaItem *formula = 0;
  try {
    // Assemble formula from GiNaC expression
    Ginac2Formula converter(scope, tex_names); expression.accept(converter);
    formula = converter.getFormula();
  } catch (Exception &err) {
    std::stringstream buffer; buffer << expression;
    formula = new MathText(buffer.str().c_str());

    Fluc::Utils::Message msg = LOG_MESSAGE(Fluc::Utils::Message::WARN);
    msg << "Can not layout expression: " << expression << ": " << err.what();
    Fluc::Utils::Logger::get().log(msg);

  }

  return formula;
}


QVariant
Ginac2Formula::toPixmap(GiNaC::ex expression, Ast::Scope &scope, bool tex_names)
{
  // Assemble formula from GiNaC expression
  MathFormulaItem *formula = toFormula(expression, scope, tex_names);
  // Render formula
  QGraphicsItem *rendered_formula = formula->layout(MathContext());
  // Draw formula into pixmap:
  QGraphicsScene *scene = new QGraphicsScene();
  scene->addItem(rendered_formula);
  QSize size = scene->sceneRect().size().toSize();
  QPixmap pixmap(size.width(), size.height());
  QPainter painter(&pixmap);
  painter.fillRect(0,0, size.width(), size.height(), QColor(255,255,255));
  scene->render(&painter);
  delete formula; delete scene;
  return pixmap;
}
