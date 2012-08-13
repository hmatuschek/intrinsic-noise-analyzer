#include "tinytex.hh"

using namespace Fluc;


/* ******************************************************************************************** *
 * Token rules for tinyTeX lexer
 * ******************************************************************************************** */
TinyTex::WordTokenRule::WordTokenRule(unsigned id)
  : Parser::TokenRule(id)
{
  allocStates(2);
  State *s1 = createState(false);
  State *s2 = createState(true);
  onAlpha(s1, s2);
  onAlpha(s2, s2);
}


TinyTex::SymbolTokenRule::SymbolTokenRule(unsigned id)
  : Parser::TokenRule(id)
{
  allocStates(3);
  State *s1 = createState(false);
  State *s2 = createState(false);
  State *s3 = createState(true);
  onChar('\'', s1, s2);
  onAlpha(s2, s3);
  onAlpha(s3, s3);
}


/* ******************************************************************************************** *
 * The tinyTeX lexer
 * ******************************************************************************************** */
TinyTex::Lexer::Lexer(std::istream &input)
  : Parser::Lexer(input)
{
  addRule(new TinyTex::WordTokenRule(WORD_TOKEN));
  addRule(new TinyTex::SymbolTokenRule(SYMBOL_TOKEN));
  addRule(new Parser::IntegerTokenRule(NUMBER_TOKEN));
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

  elements.push_back(ElementProduction::factory());
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


TinyTex::ElementProduction::ElementProduction()
  : Parser::AltProduction()
{
  ElementProduction::instance = this;

  alternatives.push_back(new Parser::TokenProduction(Lexer::WORD_TOKEN));
  alternatives.push_back(new Parser::TokenProduction(Lexer::SYMBOL_TOKEN));
  alternatives.push_back(new Parser::TokenProduction(Lexer::NUMBER_TOKEN));
  // Element ('^' | '_') Element
  alternatives.push_back(
        new Parser::Production(
          3, this,
          new Parser::AltProduction(
            2, new Parser::TokenProduction(Lexer::SUP_TOKEN),
            new Parser::TokenProduction(Lexer::SUB_TOKEN)),
          this));
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
MathFormulaItem *
TinyTex::parse(const std::string &source)
{
  std::stringstream buffer; buffer << source;
  Lexer lexer(buffer);

  Parser::Production *grammar = GrammarProduction::factory();
  Parser::ConcreteSyntaxTree cst;
  grammar->parse(lexer, cst);
  grammar->notify(lexer, cst);

  return parseFormula(cst[0], lexer);
}


MathFormula *
TinyTex::parseFormula(Fluc::Parser::ConcreteSyntaxTree &node, Lexer &lexer)
{
  /* Formula = Element [Formula] */
  MathFormula *formula = 0;

  if (node[1].matched()) {
    formula = parseFormula(node[0][1][0], lexer);
    formula->prependItem(parseElement(node[0], lexer));
  } else {
    MathFormulaItem *item = parseElement(node[0], lexer);
    if (0 == dynamic_cast<MathFormula *>(item)) {
      formula = new MathFormula(); formula->appendItem(item);
    } else {
      formula = static_cast<MathFormula *>(item);
    }
  }

  return formula;
}


MathFormulaItem *
TinyTex::parseElement(Fluc::Parser::ConcreteSyntaxTree &node, Lexer &lexer)
{
  switch (node.getAltIdx()) {
  case 0: // WORD
  case 1: // SYMBOL
  case 2: // NUMBER
    return new MathText(lexer[node[0].getTokenIdx()].getValue().c_str());

  case 3: { // Element ('^'|'_') Element
    MathFormulaItem *lhs = parseElement(node[0][0], lexer);
    MathFormulaItem *rhs = parseElement(node[0][2], lexer);
    if (0 == node[0][1].getAltIdx()) { return new MathSup(lhs, rhs); }
    else { return new MathSup(lhs, rhs); }
  }

  case 4: { // '{' Formula  '}'
    return parseFormula(node[0][1], lexer);
  }

  default: break;
  }

  return 0;
}
