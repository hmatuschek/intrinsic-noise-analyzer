#include "ginac2formula.hh"
#include "tinytex.hh"
#include <QGraphicsScene>
#include <QPainter>
#include <QPixmap>
#include "utils/logger.hh"
#include "ast/model.hh"


using namespace iNA;


/* ********************************************************************************************* *
 * Implementation of ModelExpressionContext
 * ********************************************************************************************* */
ModelExpressionContext::ModelExpressionContext(const Ast::Scope &scope)
  : _scope(scope)
{
  // Pass...
}

GiNaC::symbol
ModelExpressionContext::resolve(const std::string &identifier) {
  return _scope.getVariable(identifier)->getSymbol();
}

std::string
ModelExpressionContext::identifier(GiNaC::symbol symbol) const {
  return _scope.getVariable(symbol)->getLabel();
}

bool
ModelExpressionContext::hasConcentrationUnits(GiNaC::symbol symbol) {
  const iNA::Ast::Model *model = dynamic_cast<const iNA::Ast::Model *>(_scope.getRootScope());
  if (0 == model) { return false; }
  return (iNA::Ast::Node::isSpecies(_scope.getVariable(symbol)) &&
          (!model->speciesHaveSubstanceUnits()));
}


/* ********************************************************************************************* *
 * Implementation of Ginac2Formula
 * ********************************************************************************************* */
Ginac2Formula::Ginac2Formula(ModelExpressionContext &context, bool tex_names)
  : _context(context), _tex_names(tex_names)
{
  // pass...
}


MathItem *
Ginac2Formula::_assembleFormula(SmartPtr<Parser::Expr::Node> node, size_t precedence)
{
  if (node->isAddNode()) {
    MathFormula *formula = new MathFormula();
    formula->appendItem(_assembleFormula(node->argument(0), 1));
    formula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    formula->appendItem(new MathText("+"));
    formula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    formula->appendItem(_assembleFormula(node->argument(1), 1));
    if (precedence > 1) { return new MathBlock(formula, new MathText("("), new MathText(")")); }
    return formula;
  }

  if (node->isSubNode()) {
    MathFormula *formula = new MathFormula();
    formula->appendItem(_assembleFormula(node->argument(0), 1));
    formula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    formula->appendItem(new MathText("-"));
    formula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    formula->appendItem(_assembleFormula(node->argument(1), 2));
    if (precedence > 1) { return new MathBlock(formula, new MathText("("), new MathText(")")); }
    return formula;
  }

  if (node->isMulNode()) {
    MathFormula *formula = new MathFormula();
    formula->appendItem(_assembleFormula(node->argument(0), 2));
    formula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    formula->appendItem(new MathText(QChar(0x00B7)));
    formula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    formula->appendItem(_assembleFormula(node->argument(1), 2));
    if (precedence > 2) {
      return new MathBlock(formula, new MathText("("), new MathText(")"));
    }
    return formula;
  }

  if (node->isDivNode()) {
    MathFraction *formula =
        new MathFraction(_assembleFormula(node->argument(0), 2),
                         _assembleFormula(node->argument(1), 3));
    if (precedence > 2) {
      return new MathBlock(formula, new MathText("("), new MathText(")"));
    }
    return formula;
  }

  if (node->isPowNode()) {
    MathSup *formula = new MathSup(
          _assembleFormula(node->argument(0), 3),
          _assembleFormula(node->argument(1), 3));
    if (precedence >= 3) { return new MathBlock(formula, new MathText("("), new MathText(")")); }
    return formula;
  }

  if (node->isNegNode()) {
    MathFormula *formula = new MathFormula();
    formula->appendItem(new MathText("-"));
    formula->appendItem(_assembleFormula(node->argument(0), 2));
    if (precedence >= 2) {
      return new MathBlock(formula, new MathText("("), new MathText(")"));
    }
    return formula;
  }

  if (node->isFunctionNode()) {
    MathFormula *formula = new MathFormula();
    if (node->isFuncExpNode()) {
      formula->appendItem(new MathText("exp"));
      formula->appendItem(
            new MathBlock(_assembleFormula(node->argument(0), 0),
                          new MathText("("), new MathText(")")));
    }
    if (node->isFuncLogNode()) {
      formula->appendItem(new MathText("log"));
      formula->appendItem(
            new MathBlock(_assembleFormula(node->argument(0), 0),
                          new MathText("("), new MathText(")")));
    }
    return formula;
  }

  if (node->isSymbolNode()) {
    // Get symbols
    MathItem *symbol=0;
    if (_tex_names) { symbol = TinyTex::parseQuoted(_context.identifier(node->symbol()).c_str()); }
    else { symbol = new MathText(_context.identifier(node->symbol()).c_str()); }
    if (_context.hasConcentrationUnits(node->symbol())) {
      symbol = new MathBlock(symbol, new MathText("["), new MathText("]"));
    }
    return symbol;
  }

  if (node->isIntegerNode()) {
    long value = node->intValue();
    if (std::abs(value)<1e6) { return new MathText(QString("%1").arg(value)); }
    return new MathText(QString("%1").arg(double(value)));
  }

  if (node->isRealNode()) {
    return new MathText(QString("%1").arg(node->realValue()));
  }

  if (node->isComplexNode()) {
    MathFormula *value = new MathFormula();
    value->appendItem(new MathText(QString("%1").arg(node->complexValue().real())));
    value->appendItem(new MathText("+"));
    value->appendItem(new MathText(QString("%1j").arg(node->complexValue().imag())));
    if (precedence > 1) {
      return new MathBlock(value, new MathText("("), new MathText(")"));
    }
    return value;
  }

  // Return placeholder for unkown expressions:
  return new MathText("<Unknown Expression>");
}

MathItem *
Ginac2Formula::toFormula(GiNaC::ex expression, Ast::Scope &scope, bool tex_names)
{
  MathItem *formula = 0;
  try {
    // Assemble formula from GiNaC expression
    ModelExpressionContext context(scope);
    Ginac2Formula converter(context, tex_names);
    iNA::SmartPtr<iNA::Parser::Expr::Node> ir = iNA::Parser::Expr::Node::fromExpression(expression);
    iNA::Parser::Expr::PrettySerializationTrafo::apply(ir);
    formula = converter._assembleFormula(ir, 0);
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
