#include "tinytex.hh"
#include "exception.hh"
#include "formula.hh"

#include <sstream>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPainter>
#include <QDebug>

using namespace Fluc;


/* ******************************************************************************************** *
 * Exception class
 * ******************************************************************************************** */
TinyTex::Error::Error() : ParserError() {
  // pass
}

TinyTex::Error::Error(const Error &other) : ParserError(other) {
  // pass...
}

TinyTex::Error::Error(const Fluc::Parser::ParserError &other) : ParserError(other) {
  // pass...
}

TinyTex::Error::~Error() throw () {
  // pass...
}


/* ******************************************************************************************** *
 * Token rules for tinyTeX lexer
 * ******************************************************************************************** */
TinyTex::TextTokenRule::TextTokenRule(unsigned id)
  : Parser::TokenRule(id)
{
  allocStates(2);
  State *s1 = createState(false);
  State *s2 = createState(true);

  Condition &cond12 = s1->createTransition(s2);
  cond12.addOnValue('\\'); cond12.addOnValue('_'); cond12.addOr();
  cond12.addOnValue('^');  cond12.addOr();
  cond12.addOnValue(' ');  cond12.addOr();
  cond12.addOnValue('\t'); cond12.addOr();
  cond12.addOnValue('{');  cond12.addOr();
  cond12.addOnValue('}');  cond12.addOr();
  cond12.addNot();

  Condition &cond22 = s2->createTransition(s2);
  cond22.addOnValue('\\'); cond22.addOnValue('_'); cond22.addOr();
  cond22.addOnValue('^');  cond22.addOr();
  cond22.addOnValue(' ');  cond22.addOr();
  cond22.addOnValue('\t'); cond22.addOr();
  cond22.addOnValue('{');  cond22.addOr();
  cond22.addOnValue('}');  cond22.addOr();
  cond22.addNot();
}


TinyTex::SymbolTokenRule::SymbolTokenRule(unsigned id)
  : Parser::TokenRule(id)
{
  allocStates(3);
  State *s1 = createState(false);
  State *s2 = createState(false);
  State *s3 = createState(true);
  onChar('\\', s1, s2);
  onAlpha(s2, s3);
  onAlpha(s3, s3);
}


/* ******************************************************************************************** *
 * The tinyTeX lexer
 * ******************************************************************************************** */
TinyTex::Lexer::Lexer(std::istream &input)
  : Parser::Lexer(input)
{
  addRule(new TinyTex::TextTokenRule(TEXT_TOKEN));
  addRule(new TinyTex::SymbolTokenRule(SYMBOL_TOKEN));
  addRule(new Parser::KeyWordTokenRule(SUP_TOKEN, "^"));
  addRule(new Parser::KeyWordTokenRule(SUB_TOKEN, "_"));
  addRule(new Parser::KeyWordTokenRule(LBRA_TOKEN, "{"));
  addRule(new Parser::KeyWordTokenRule(RBRA_TOKEN, "}"));
  addRule(new Parser::WhiteSpaceTokenRule(WHITESPACE_TOKEN));

  addIgnoredToken(WHITESPACE_TOKEN);
}


/* ******************************************************************************************** *
 * The tinyTeX productions
 * ******************************************************************************************** */
TinyTex::GrammarProduction::GrammarProduction()
  : Parser::Production()
{
  GrammarProduction::instance = this;
  elements.push_back(FormulaProduction::factory());
  elements.push_back(new Parser::TokenProduction(Parser::Token::END_OF_INPUT));
}

TinyTex::GrammarProduction *TinyTex::GrammarProduction::instance = 0;

Parser::Production *
TinyTex::GrammarProduction::factory() {
  if (0 == GrammarProduction::instance) {
    return new GrammarProduction();
  }
  return GrammarProduction::instance;
}


TinyTex::FormulaProduction::FormulaProduction()
  : Parser::Production()
{
  FormulaProduction::instance = this;

  elements.push_back(SupSubScriptProduction::factory());
  elements.push_back(new Parser::OptionalProduction(this));
}

TinyTex::FormulaProduction *TinyTex::FormulaProduction::instance = 0;

Parser::Production *
TinyTex::FormulaProduction::factory() {
  if (0 == FormulaProduction::instance) {
    return new FormulaProduction();
  }
  return FormulaProduction::instance;
}


TinyTex::SupSubScriptProduction::SupSubScriptProduction()
  : Parser::Production()
{
  SupSubScriptProduction::instance = this;

  elements.push_back(ElementProduction::factory());
  elements.push_back(
        new Parser::OptionalProduction(
          new Parser::Production(
            2, new Parser::AltProduction(
              2, new Parser::TokenProduction(Lexer::SUP_TOKEN),
              new Parser::TokenProduction(Lexer::SUB_TOKEN)),
            ElementProduction::factory())));
}

TinyTex::SupSubScriptProduction *TinyTex::SupSubScriptProduction::instance = 0;

Parser::Production *
TinyTex::SupSubScriptProduction::factory() {
  if (0 == SupSubScriptProduction::instance) {
    return new SupSubScriptProduction();
  }
  return SupSubScriptProduction::instance;
}


TinyTex::ElementProduction::ElementProduction()
  : Parser::AltProduction()
{
  ElementProduction::instance = this;

  alternatives.push_back(new Parser::TokenProduction(Lexer::TEXT_TOKEN));
  alternatives.push_back(new Parser::TokenProduction(Lexer::SYMBOL_TOKEN));
  // '{' Formula '}'
  alternatives.push_back(
        new Parser::Production(
          3, new Parser::TokenProduction(Lexer::LBRA_TOKEN),
          FormulaProduction::factory(),
          new Parser::TokenProduction(Lexer::RBRA_TOKEN)));
}

TinyTex::ElementProduction *TinyTex::ElementProduction::instance = 0;

Parser::Production *
TinyTex::ElementProduction::factory() {
  if (0 == ElementProduction::instance) {
    return new ElementProduction();
  }
  return ElementProduction::instance;
}


/* ******************************************************************************************** *
 * The tinyTeX parser
 * ******************************************************************************************** */
TinyTex::TinyTex(Parser::Lexer &lexer)
  : _lexer(lexer), _symbol_table()
{
  // Uppercase greek letters:
  _symbol_table["\\Alpha"] = QChar(0x0391); _symbol_table["\\Beta"] = QChar(0x0392);
  _symbol_table["\\Gamma"] = QChar(0x0393); _symbol_table["\\Delta"] = QChar(0x0394);
  _symbol_table["\\Epsilon"] = QChar(0x0395); _symbol_table["\\Zeta"] = QChar(0x0396);
  _symbol_table["\\Eta"] = QChar(0x0397); _symbol_table["\\Theta"] = QChar(0x0398);
  _symbol_table["\\Iota"] = QChar(0x0399); _symbol_table["\\Kappa"] = QChar(0x039A);
  _symbol_table["\\Lambda"] = QChar(0x039B); _symbol_table["\\Mu"] = QChar(0x039C);
  _symbol_table["\\Nu"] = QChar(0x039D); _symbol_table["\\Xi"] = QChar(0x039E);
  _symbol_table["\\Omicron"] = QChar(0x039F); _symbol_table["\\Pi"] = QChar(0x03A0);
  _symbol_table["\\Roh"] = QChar(0x03A1); _symbol_table["\\Sigma"] = QChar(0x03A3);
  _symbol_table["\\Tau"] = QChar(0x03A4); _symbol_table["\\Upsilon"] = QChar(0x03A5);
  _symbol_table["\\Phi"] = QChar(0x03A6); _symbol_table["\\Chi"] = QChar(0x03A7);
  _symbol_table["\\Psi"] = QChar(0x03A8); _symbol_table["\\Omega"] = QChar(0x03A9);

  // Lowercase greek letters:
  _symbol_table["\\alpha"] = QChar(0x03B1); _symbol_table["\\beta"] = QChar(0x03B2);
  _symbol_table["\\gamma"] = QChar(0x03B3); _symbol_table["\\delta"] = QChar(0x03B4);
  _symbol_table["\\epsilon"] = QChar(0x03B5); _symbol_table["\\zeta"] = QChar(0x03B6);
  _symbol_table["\\eta"] = QChar(0x03B7); _symbol_table["\\theta"] = QChar(0x03B8);
  _symbol_table["\\iota"] = QChar(0x03B9); _symbol_table["\\kappa"] = QChar(0x03BA);
  _symbol_table["\\lambda"] = QChar(0x03BB); _symbol_table["\\mu"] = QChar(0x03BC);
  _symbol_table["\\nu"] = QChar(0x03BD); _symbol_table["\\xi"] = QChar(0x03BE);
  _symbol_table["\\omicron"] = QChar(0x03BF); _symbol_table["\\pi"] = QChar(0x03C0);
  _symbol_table["\\roh"] = QChar(0x03C1); _symbol_table["\\sigma"] = QChar(0x03C3);
  _symbol_table["\\tau"] = QChar(0x03C4); _symbol_table["\\upsilon"] = QChar(0x03C5);
  _symbol_table["\\phi"] = QChar(0x03C6); _symbol_table["\\chi"] = QChar(0x03C7);
  _symbol_table["\\psi"] = QChar(0x03C8); _symbol_table["\\omega"] = QChar(0x03C9);

  // Arrow symbols:
  _symbol_table["\\leftarrow"] = QChar(0x2190);
  _symbol_table["\\rightarrow"] = QChar(0x2192);
  _symbol_table["\\leftrightarrow"] = QChar(0x2194);
  _symbol_table["\\leftharpoonup"] = QChar(0x21BC);
  _symbol_table["\\rightleftharpoons"] = QChar(0x21CC);
  _symbol_table["\\Leftarrow"] = QChar(0x21D0);
  _symbol_table["\\Leftrightarrow"] = QChar(0x21D4);

  // Operators
  _symbol_table["\\cdot"] = QChar(0x00B7);
  _symbol_table["\\times"] = QChar(0x00D7);
  _symbol_table["\\ast"] = QChar(0x2217);
  _symbol_table["\\partial"] = QChar(0x2202);
}

MathFormulaItem *
TinyTex::parse(const std::string &source)
{
  std::stringstream buffer; buffer << source;
  Lexer lexer(buffer);

  Parser::Production *grammar = GrammarProduction::factory();
  Parser::ConcreteSyntaxTree cst;

  MathFormulaItem *item = 0;
  try {
    grammar->parse(lexer, cst);
    grammar->notify(lexer, cst);
    TinyTex parser(lexer);
    item = parser.parseFormula(cst[0]);
  } catch (Fluc::Parser::ParserError &err) {
    throw TinyTex::Error(err);
  }

  return item;
}


MathFormula *
TinyTex::parseFormula(Fluc::Parser::ConcreteSyntaxTree &node)
{
  /* Formula = SupSubScript [Formula] */
  MathFormula *formula = 0;

  if (node[1].matched()) {
    formula = parseFormula(node[1][0]);
    formula->prependItem(parseSupSub(node[0]));
  } else {
    MathFormulaItem *item = parseSupSub(node[0]);
    if (0 == dynamic_cast<MathFormula *>(item)) {
      formula = new MathFormula(); formula->appendItem(item);
    } else {
      formula = static_cast<MathFormula *>(item);
    }
  }

  return formula;
}


MathFormulaItem *
TinyTex::parseSupSub(Parser::ConcreteSyntaxTree &node)
{
  /** := Element [('^'|'_') Element] */
  MathFormulaItem *item = parseElement(node[0]);

  if (node[1].matched()) {
    MathFormulaItem *rhs = parseElement(node[1][0][1]);
    if (0 == node[1][0][0].getAltIdx()) {
      item = new MathSup(item, rhs);
    } else {
      item = new MathSub(item, rhs);
    }
  }

  return item;
}


MathFormulaItem *
TinyTex::parseElement(Fluc::Parser::ConcreteSyntaxTree &node)
{
  switch (node.getAltIdx()) {
  case 0: // Text
    return new MathText(
          QString::fromAscii(_lexer[node[0].getTokenIdx()].getValue().c_str(),
                             _lexer[node[0].getTokenIdx()].getValue().size()));

  case 1: // Symbol
    return processSymbol(_lexer[node[0].getTokenIdx()].getValue());

  case 2: { // '{' Formula  '}'
    return parseFormula(node[0][1]);
  }

  default: break;
  }

  return 0;
}


MathFormulaItem *
TinyTex::processSymbol(const std::string &symbol)
{
  std::map<std::string, QString>::iterator item=_symbol_table.find(symbol);

  if (_symbol_table.end() == item) {
    TinyTex::Error err;
    err << "Can not parse expression: Unknown TinyTeX symbol " << symbol;
    throw err;
  }

  return new MathText(item->second);
}


QVariant
TinyTex::toPixmap(const std::string &source)
{
  MathFormulaItem *item = 0;
  try {
    item = TinyTex::parse(source);
  } catch (TinyTex::Error &error) {
    item = new MathText(source.c_str());
  }

  QGraphicsItem *rendered_item = item->layout(MathContext());
  // Draw formula into pixmap:
  QGraphicsScene *scene = new QGraphicsScene();
  scene->addItem(rendered_item);
  QSize size = scene->sceneRect().size().toSize();
  QPixmap pixmap(size.width(), size.height());
  QPainter painter(&pixmap);
  painter.fillRect(0,0, size.width(), size.height(), QColor(255,255,255));
  scene->render(&painter);
  delete item; delete scene;

  return pixmap;
}
