#include "ginac2formula.hh"
#include "tinytex.hh"
#include <QGraphicsScene>
#include <QPainter>
#include <QPixmap>
#include "utils/logger.hh"


using namespace iNA;


Ginac2Formula::Ginac2Formula(iNA::Ast::Scope &scope, bool tex_names)
  : _stack(), _scope(scope), _tex_names(tex_names), _current_precedence(0)
{
  // pass...
}


void
Ginac2Formula::visit(const GiNaC::symbol &node)
{
  if (! _scope.hasDefinition(node.get_name())) {
    iNA::Utils::Message msg = LOG_MESSAGE(iNA::Utils::Message::INFO);
    msg << "Can not resolve symbol: " << node;
    iNA::Utils::Logger::get().log(msg);

    _stack.push_back(new MathText(node.get_name().c_str()));
    return;
  }

  Ast::Definition *def = _scope.getDefinition(node.get_name());

  Ast::VariableDefinition *var = 0;
  if (0 == (var = dynamic_cast<Ast::VariableDefinition *>(def))) {
    iNA::Utils::Message msg = LOG_MESSAGE(iNA::Utils::Message::INFO);
    msg << "Symbol : " << node << " does not refer to a variable.";
    iNA::Utils::Logger::get().log(msg);

    _stack.push_back(new MathText(node.get_name().c_str()));
    return;
  }

  if (var->hasName()) {
    if (TinyTex::isTexQuoted(var->getName())) {
      _stack.push_back(TinyTex::parse(TinyTex::texUnquote(var->getName())));
    } else {
      _stack.push_back(new MathText(var->getName().c_str()));
    }
  } else {
    _stack.push_back(new MathText(var->getIdentifier().c_str()));
  }

  // Now, the name has been rendered -> if variable was a species with concentration untis ->
  // put name in brackets:
  if (iNA::Ast::Node::isSpecies(var) && var->getUnit().isConcentrationUnit()) {
    MathFormula *tmp = new MathFormula();
    tmp->appendItem(new MathText("["));
    tmp->appendItem(_stack.back()); _stack.pop_back();
    tmp->appendItem(new MathText("]"));
    _stack.push_back(tmp);
  }
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

  /*if (old_precedence > _current_precedence) {
    formula->appendItem(new MathText("("));
  }*/

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
    //formula->appendItem(new MathText(")"));
    _stack.push_back(new MathBlock(formula, new MathText("("), new MathText(")")));
  } else {
    _stack.push_back(formula);
  }
  _current_precedence = old_precedence;
}


void
Ginac2Formula::visit(const GiNaC::mul &node)
{
  int old_precedence = _current_precedence;
  _current_precedence = 2;

  MathFormula *formula = new MathFormula();
  QList<MathItem *> numerator, denumerator;

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
        if (value.info(GiNaC::info_flags::integer) && value.is_negative()) {
          (1/node.op(i)).accept(*this);
          if (0 < denumerator.size()) { denumerator.append(new MathText(QChar(0x00B7))); }
          denumerator.append(_stack.back()); _stack.pop_back();
          continue;
        }
      }
    }

    node.op(i).accept(*this);
    if (0 < numerator.size()) { numerator.append(new MathText(QChar(0x00B7))); }
    numerator.append(_stack.back()); _stack.pop_back();
  }

  /*
   * Now, assemble formula (product/quotient)
   */
  // If there are no numerator elements:
  if (0 == numerator.size()) {
    numerator.append(new MathText("1"));
  }

  // if there are no denumerator elements:
  if (0 == denumerator.size()) {
    for (QList<MathItem *>::iterator item=numerator.begin(); item!=numerator.end(); item++) {
      formula->appendItem(*item);
    }
  } else {
    MathFormula *num = new MathFormula();
    MathFormula *denum = new MathFormula();
    for (QList<MathItem *>::iterator item=numerator.begin(); item!=numerator.end(); item++) {
      num->appendItem(*item);
    }
    for (QList<MathItem *>::iterator item=denumerator.begin(); item!=denumerator.end(); item++) {
      denum->appendItem(*item);
    }
    formula->appendItem(new MathFraction(num, denum));
  }

  if ((old_precedence > _current_precedence) && (node.nops()>1)) {
    _stack.push_back(new MathBlock(formula, new MathText("("), new MathText(")")));
  } else {
    _stack.push_back(formula);
  }
  _current_precedence = old_precedence;
}


void
Ginac2Formula::visit(const GiNaC::power &node)
{
  int old_precedence = _current_precedence;
  _current_precedence = 3;

  // Handle base & exp:
  node.op(0).accept(*this);
  node.op(1).accept(*this);

  MathItem *exponent = _stack.back(); _stack.pop_back();
  MathItem *base = _stack.back(); _stack.pop_back();
  _stack.push_back(new MathSup(base, exponent));

  _current_precedence = old_precedence;
}


MathItem *
Ginac2Formula::getFormula()
{
  MathItem *item = _stack.back(); _stack.pop_back();
  return item;
}


MathItem *
Ginac2Formula::toFormula(GiNaC::ex expression, Ast::Scope &scope, bool tex_names)
{
  MathItem *formula = 0;
  try {
    // Assemble formula from GiNaC expression
    Ginac2Formula converter(scope, tex_names); expression.accept(converter);
    formula = converter.getFormula();
  } catch (Exception &err) {
    std::stringstream buffer; buffer << expression;
    formula = new MathText(buffer.str().c_str());

    iNA::Utils::Message msg = LOG_MESSAGE(iNA::Utils::Message::WARN);
    msg << "Can not layout expression: " << expression << ": " << err.what();
    iNA::Utils::Logger::get().log(msg);
  }

  return formula;
}


QVariant
Ginac2Formula::toPixmap(GiNaC::ex expression, Ast::Scope &scope, bool tex_names)
{
  // Assemble formula from GiNaC expression
  MathItem *formula = toFormula(expression, scope, tex_names);
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
