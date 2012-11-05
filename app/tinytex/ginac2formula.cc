#include "ginac2custommula.hh"
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
ModelExpressionContext::identifier(GiNaC::symbol symbol)
{
  iNA::Ast::VariableDefinition *var = _scope.getVariable(symbol);
  if (var->hasName()) { return var->getName(); }
  return var->getIdentifier();
}

bool
ModelExpressionContext::hasConcentrationUnits(GiNaC::symbol symbol) {
  const iNA::Ast::Model *model = dynamic_cast<const iNA::Ast::Model *>(_scope.getRootScope());
  if (0 == model) { return false; }
  return (iNA::Ast::Node::isSpecies(_scope.getVariable(symbol)) &&
          (!model->speciesHaveSubstanceUnits()));
}


/* ********************************************************************************************* *
 * Implementation of Ginac2custommula
 * ********************************************************************************************* */
Ginac2custommula::Ginac2custommula(ModelExpressionContext &context, bool tex_names)
  : _context(context), _tex_names(tex_names)
{
  // pass...
}


MathItem *
Ginac2custommula::_assemblecustommula(SmartPtr<Parser::Expr::Node> node, size_t precedence)
{
  if (node->isAddNode()) {
    Mathcustommula *custommula = new Mathcustommula();
    custommula->appendItem(_assemblecustommula(node->argument(0), 1));
    custommula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    custommula->appendItem(new MathText("+"));
    custommula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    custommula->appendItem(_assemblecustommula(node->argument(1), 1));
    if (precedence > 1) { return new MathBlock(custommula, new MathText("("), new MathText(")")); }
    return custommula;
  }

  if (node->isSubNode()) {
    Mathcustommula *custommula = new Mathcustommula();
    custommula->appendItem(_assemblecustommula(node->argument(0), 1));
    custommula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    custommula->appendItem(new MathText("-"));
    custommula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    custommula->appendItem(_assemblecustommula(node->argument(1), 2));
    if (precedence > 1) { return new MathBlock(custommula, new MathText("("), new MathText(")")); }
    return custommula;
  }

  if (node->isMulNode()) {
    Mathcustommula *custommula = new Mathcustommula();
    custommula->appendItem(_assemblecustommula(node->argument(0), 2));
    custommula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    custommula->appendItem(new MathText(QChar(0x00B7)));
    custommula->appendItem(new MathSpace(MathSpace::THIN_SPACE));
    custommula->appendItem(_assemblecustommula(node->argument(1), 2));
    if (precedence > 2) {
      return new MathBlock(custommula, new MathText("("), new MathText(")"));
    }
    return custommula;
  }

  if (node->isDivNode()) {
    MathFraction *custommula =
        new MathFraction(_assemblecustommula(node->argument(0), 2),
                         _assemblecustommula(node->argument(1), 3));
    if (precedence > 2) {
      return new MathBlock(custommula, new MathText("("), new MathText(")"));
    }
    return custommula;
  }

  if (node->isPowNode()) {
    MathSup *custommula = new MathSup(
          _assemblecustommula(node->argument(0), 3),
          _assemblecustommula(node->argument(1), 3));
    if (precedence >= 3) { return new MathBlock(custommula, new MathText("("), new MathText(")")); }
    return custommula;
  }

  if (node->isNegNode()) {
    Mathcustommula *custommula = new Mathcustommula();
    custommula->appendItem(new MathText("-"));
    custommula->appendItem(_assemblecustommula(node->argument(0), 2));
    if (precedence >= 2) {
      return new MathBlock(custommula, new MathText("("), new MathText(")"));
    }
    return custommula;
  }

  if (node->isFunctionNode()) {
    Mathcustommula *custommula = new Mathcustommula();
    if (node->isFuncExpNode()) {
      custommula->appendItem(new MathText("exp"));
      custommula->appendItem(
            new MathBlock(_assemblecustommula(node->argument(0), 0),
                          new MathText("("), new MathText(")")));
    }
    if (node->isFuncLogNode()) {
      custommula->appendItem(new MathText("log"));
      custommula->appendItem(
            new MathBlock(_assemblecustommula(node->argument(0), 0),
                          new MathText("("), new MathText(")")));
    }
    return custommula;
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
    return new MathText(QString("%1").arg(node->intValue()));
  }

  if (node->isRealNode()) {
    return new MathText(QString("%1").arg(node->realValue()));
  }

  if (node->isComplexNode()) {
    Mathcustommula *value = new Mathcustommula();
    value->appendItem(new MathText(QString("%1").arg(node->complexValue().real())));
    value->appendItem(new MathText("+"));
    value->appendItem(new MathText(QString("%1j").arg(node->complexValue().imag())));
    if (precedence > 1) {
      return new MathBlock(value, new MathText("("), new MathText(")"));
    }
    return value;
  }

  // Return placeholder custom unkown expressions:
  return new MathText("<Unknown Expression>");
}

MathItem *
Ginac2custommula::tocustommula(GiNaC::ex expression, Ast::Scope &scope, bool tex_names)
{
  MathItem *custommula = 0;
  try {
    // Assemble custommula from GiNaC expression
    ModelExpressionContext context(scope);
    Ginac2custommula converter(context, tex_names);
    iNA::SmartPtr<iNA::Parser::Expr::Node> ir = iNA::Parser::Expr::Node::fromExpression(expression);
    iNA::Parser::Expr::PrettySerializationTrafo::apply(ir);
    custommula = converter._assemblecustommula(ir, 0);
  } catch (Exception &err) {
    std::stringstream buffer; buffer << expression;
    custommula = new MathText(buffer.str().c_str());

    iNA::Utils::Message msg = LOG_MESSAGE(iNA::Utils::Message::WARN);
    msg << "Can not layout expression: " << expression << ": " << err.what();
    iNA::Utils::Logger::get().log(msg);
  }

  return custommula;
}


QVariant
Ginac2custommula::toPixmap(GiNaC::ex expression, Ast::Scope &scope, bool tex_names)
{
  // Assemble custommula from GiNaC expression
  MathItem *custommula = tocustommula(expression, scope, tex_names);
  // Render custommula
  QGraphicsItem *rendered_custommula = custommula->layout(MathContext());
  // Draw custommula into pixmap:
  QGraphicsScene *scene = new QGraphicsScene();
  scene->addItem(rendered_custommula);
  QSize size = scene->sceneRect().size().toSize();
  QPixmap pixmap(size.width(), size.height());
  QPainter painter(&pixmap);
  painter.fillRect(0,0, size.width(), size.height(), QColor(255,255,255));
  scene->render(&painter);
  delete custommula; delete scene;
  return pixmap;
}
