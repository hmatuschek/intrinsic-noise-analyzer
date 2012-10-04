#include "plotformulaparser.hh"

using namespace iNA;


/* ******************************************************************************************** *
 * Definition of token IDs.
 * ******************************************************************************************** */
typedef enum {
  T_WHITESPACE = Parser::Token::FIRST_USER_DEFINED,  ///< A white space char (SPACE, TAB)
  T_COLUMN_ID,        ///< A column identifier "$NUMBER".
  T_IDENTIFIER,       ///< C-style identifier.
  T_INTEGER,          ///< Integers (w/o) sign.
  T_FLOAT,            ///< Floats (w/o) sign.
  T_COMMA,            ///< ','
  T_PLUS,             ///< '+'
  T_MINUS,            ///< '-'
  T_TIMES,            ///< '*'
  T_POWER,            ///< '**' or '^'
  T_DIVIVE,           ///< '/'
  T_LPAR,             ///< '('
  T_RPAR              ///< ')'
} TokenId;


/* ******************************************************************************************** *
 * Implementation of Tokenrules for Plot formula parser:
 * ******************************************************************************************** */
class ColumnIdentifierTokenRule : public Parser::TokenRule
{
public:
  ColumnIdentifierTokenRule()
    : Parser::TokenRule(T_COLUMN_ID)
  {
    allocStates(3);
    State *si = createState(false);
    State *s1 = createState(false);
    State *sf = createState(true);
    onChar('$', si, s1);
    onNumber(s1,sf);
    onNumber(sf, sf);
  }
};



/* ******************************************************************************************** *
 * Forward declaration of parser productions...
 * ******************************************************************************************** */
class PlotFormulaGrammar : public iNA::Parser::Production {
public:
  static iNA::Parser::Production *get();
protected:
  PlotFormulaGrammar();
  static PlotFormulaGrammar *_instance;
};

class PlotFormulaProduction : public iNA::Parser::AltProduction {
public:
  static iNA::Parser::Production *get();
protected:
  PlotFormulaProduction();
  static PlotFormulaProduction *_instance;
};

class PlotFormulaProductProduction : public iNA::Parser::AltProduction {
public:
  static iNA::Parser::Production *get();
protected:
  PlotFormulaProductProduction();
  static PlotFormulaProductProduction *_instance;
};

class PlotFormulaPowerProduction : public iNA::Parser::AltProduction {
public:
  static iNA::Parser::Production *get();
protected:
  PlotFormulaPowerProduction();
  static PlotFormulaPowerProduction *_instance;
};

class PlotFormulaAtomicProduction : public iNA::Parser::AltProduction {
public:
  static iNA::Parser::Production *get();
protected:
  PlotFormulaAtomicProduction();
  static PlotFormulaAtomicProduction *_instance;
};


/* ******************************************************************************************** *
 *  Number | FunctionCall | Identifier | ("(" Expression ")") | "-" AtomicExpression;
 * ******************************************************************************************** */
PlotFormulaAtomicProduction *PlotFormulaAtomicProduction::_instance = 0;
Parser::Production *PlotFormulaAtomicProduction::get() {
  if (0 == _instance) { return new PlotFormulaAtomicProduction(); }
  return _instance;
}

PlotFormulaAtomicProduction::PlotFormulaAtomicProduction()
  : Parser::AltProduction()
{
  _instance = this;
  //alternatives.push_back(NumberProduction::get());
  //alternatives.push_back(FunctionCallProduction::get());
  alternatives.push_back(new Parser::TokenProduction(T_COLUMN_ID));
  alternatives.push_back(
        new Parser::Production(
          3, new Parser::TokenProduction(T_LPAR),
          PlotFormulaProduction::get(),
          new Parser::TokenProduction(T_RPAR)));
  alternatives.push_back(
        new Parser::Production(
          2, new Parser::TokenProduction(T_MINUS), this));
}


/* ******************************************************************************************** *
 * (AtomicExpression ("**" | "^") PowerExpression) | AtomicExpression;
 * ******************************************************************************************** */
PlotFormulaPowerProduction *PlotFormulaPowerProduction::_instance = 0;
Parser::Production *PlotFormulaPowerProduction::get() {
  if (0 == _instance) { return new PlotFormulaPowerProduction(); }
  return _instance;
}

PlotFormulaPowerProduction::PlotFormulaPowerProduction()
  : Parser::AltProduction()
{
  _instance = this;
  alternatives.push_back(
        new Parser::Production(
          3,
          PlotFormulaAtomicProduction::get(),
          new Parser::TokenProduction(T_POWER),
          PlotFormulaPowerProduction::get()));
  alternatives.push_back(PlotFormulaAtomicProduction::get());
}


/* ******************************************************************************************** *
 * (PowerExpression ("*" | "/") ProductExpression) | PowerExpression;
 * ******************************************************************************************** */
PlotFormulaProductProduction *PlotFormulaProductProduction::_instance = 0;
Parser::Production *PlotFormulaProductProduction::get() {
  if (0 == _instance) { return new PlotFormulaProductProduction(); }
  return _instance;
}

PlotFormulaProductProduction::PlotFormulaProductProduction()
  : Parser::AltProduction()
{
  _instance = this;
  alternatives.push_back(
        new Parser::Production(
          3,
          PlotFormulaPowerProduction::get(),
          new Parser::AltProduction(
            2, new Parser::TokenProduction(T_TIMES),
            new Parser::TokenProduction(T_DIVIVE)),
          PlotFormulaProductProduction::get()));
  alternatives.push_back(PlotFormulaPowerProduction::get());
}


/* ******************************************************************************************** *
 * (ProductExpression ('+'|'-') Expression) | ProductExpression
 * ******************************************************************************************** */
PlotFormulaProduction *PlotFormulaProduction::_instance = 0;

Parser::Production *
PlotFormulaProduction::get() {
  if (0 == _instance) { return new PlotFormulaProduction(); }
  return _instance;
}

PlotFormulaProduction::PlotFormulaProduction()
  : Parser::AltProduction()
{
  _instance = this;
  alternatives.push_back(
        new Parser::Production(
          3,
          PlotFormulaProductProduction::get(),
          new Parser::AltProduction(
            2, new Parser::TokenProduction(T_PLUS),
            new Parser::TokenProduction(T_MINUS)),
          PlotFormulaProduction::get()));
  alternatives.push_back(PlotFormulaProductProduction::get());
}


/* ******************************************************************************************** *
 * Implementation of Plot formula grammar
 * Grammar = PlotFormulaProduction END_OF_LINE
 * ******************************************************************************************** */
PlotFormulaGrammar *PlotFormulaGrammar::_instance = 0;

Parser::Production *
PlotFormulaGrammar::get() {
  if (0 == _instance) { return new PlotFormulaGrammar(); }
  return _instance;
}

PlotFormulaGrammar::PlotFormulaGrammar()
  : Parser::Production()
{
  _instance = this;
  elements.push_back(PlotFormulaProduction::get());
  elements.push_back(new Parser::TokenProduction(Parser::Token::END_OF_INPUT));
}

/* ******************************************************************************************** *
 * Implementation of assembler:
 * ******************************************************************************************** */
GiNaC::ex __plot_formula_process_atomic(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotFormulaParser::Context &ctx);
GiNaC::ex __plot_formula_process_power(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotFormulaParser::Context &ctx);
GiNaC::ex __plot_formula_process_product(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotFormulaParser::Context &ctx);
GiNaC::ex __plot_formula_process_expression(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotFormulaParser::Context &ctx);

GiNaC::ex __plot_formula_process_atomic(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotFormulaParser::Context &ctx) {
  if (0 == node.getAltIdx()) {
    std::stringstream buffer;
    buffer << lexer[node[0].getTokenIdx()].getValue().substr(1);
    size_t index; buffer >> index;
    return ctx.getColumnSymbol(index);
  } else if (1 == node.getAltIdx()) {
    return __plot_formula_process_expression(node[0][1], lexer, ctx);
  }
  // On alt-idx == 2:
  return __plot_formula_process_atomic(node[0][1], lexer, ctx);
}

GiNaC::ex __plot_formula_process_power(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotFormulaParser::Context &ctx) {
  if (0 == node.getAltIdx()) {
    GiNaC::ex lhs = __plot_formula_process_atomic(node[0][0], lexer, ctx);
    GiNaC::ex rhs = __plot_formula_process_power(node[0][2], lexer, ctx);
    return GiNaC::pow(lhs, rhs);
  }
  return __plot_formula_process_atomic(node[0], lexer, ctx);
}

GiNaC::ex __plot_formula_process_product(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotFormulaParser::Context &ctx) {
  if (0 == node.getAltIdx()) {
    GiNaC::ex lhs = __plot_formula_process_power(node[0][0], lexer, ctx);
    GiNaC::ex rhs = __plot_formula_process_product(node[0][2], lexer, ctx);
    if (0 == node[0][1].getAltIdx()) {
      return lhs * rhs;
    }
    return lhs / rhs;
  }
  return __plot_formula_process_power(node[0], lexer, ctx);
}

GiNaC::ex __plot_formula_process_expression(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotFormulaParser::Context &ctx) {
  if (0 == node.getAltIdx()) {
    GiNaC::ex lhs = __plot_formula_process_product(node[0][0], lexer, ctx);
    GiNaC::ex rhs = __plot_formula_process_expression(node[0][2], lexer, ctx);
    if (0 == node[0][1].getAltIdx()) {
      return lhs + rhs;
    }
    return lhs-rhs;
  }
  return __plot_formula_process_product(node[0], lexer, ctx);
}


/* ******************************************************************************************** *
 * Implementation of PlotFormulaParser
 * ******************************************************************************************** */
PlotFormulaParser::Context::Context(Table *table)
  : _table(table)
{
  _symbols.resize(_table->getNumColumns());
  for (size_t i=0; i<_table->getNumColumns(); i++) {
    _symbols[i] = GiNaC::symbol(table->getColumnName(i).toStdString());
  }
}

GiNaC::symbol
PlotFormulaParser::Context::getColumnSymbol(size_t column) {
  return _symbols[column];
}

double
PlotFormulaParser::Context::operator ()(size_t row, GiNaC::ex expression)
{
  GiNaC::exmap values;
  for (size_t i=0; i<_table->getNumColumns(); i++) {
    values[_symbols[i]] = (*_table)(row, i);
  }

  GiNaC::ex value = GiNaC::evalf(expression.subs(values));
  if (! GiNaC::is_a<GiNaC::numeric>(value)) {
    return 0./0.;
  }
  return GiNaC::ex_to<GiNaC::numeric>(value).to_double();
}


bool
PlotFormulaParser::check(const QString &formula, Context &context)
{
  std::stringstream buffer; buffer << formula.toStdString();

  Parser::Lexer lexer(buffer);
  lexer.addRule(new Parser::WhiteSpaceTokenRule(T_WHITESPACE));
  lexer.addRule(new ColumnIdentifierTokenRule());
  lexer.addRule(new Parser::IdentifierTokenRule(T_IDENTIFIER));
  lexer.addRule(new Parser::IntegerTokenRule(T_INTEGER));
  lexer.addRule(new Parser::FloatTokenRule(T_FLOAT));
  lexer.addRule(new Parser::KeyWordTokenRule(T_COMMA, ","));
  lexer.addRule(new Parser::KeyWordTokenRule(T_PLUS, "+"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_MINUS, "-"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_TIMES, "*"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_POWER, "**"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_POWER, "^"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_DIVIVE, "/"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_LPAR, "("));
  lexer.addRule(new Parser::KeyWordTokenRule(T_RPAR, ")"));
  lexer.addIgnoredToken(T_WHITESPACE);

  Parser::ConcreteSyntaxTree cts;

  try {
    Parser::ConcreteSyntaxTree cst;
    PlotFormulaGrammar::get()->parse(lexer, cst);
    __plot_formula_process_expression(cst[0], lexer, context);
  } catch (Parser::ParserError &err) {
    return false;
  }

  return true;
}


GiNaC::ex
PlotFormulaParser::parse(const QString &formula, Context &context)
{
  std::stringstream buffer; buffer << formula.toStdString();

  Parser::Lexer lexer(buffer);
  lexer.addRule(new Parser::WhiteSpaceTokenRule(T_WHITESPACE));
  lexer.addRule(new ColumnIdentifierTokenRule());
  lexer.addRule(new Parser::IdentifierTokenRule(T_IDENTIFIER));
  lexer.addRule(new Parser::IntegerTokenRule(T_INTEGER));
  lexer.addRule(new Parser::FloatTokenRule(T_FLOAT));
  lexer.addRule(new Parser::KeyWordTokenRule(T_COMMA, ","));
  lexer.addRule(new Parser::KeyWordTokenRule(T_PLUS, "+"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_MINUS, "-"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_TIMES, "*"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_POWER, "**"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_POWER, "^"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_DIVIVE, "/"));
  lexer.addRule(new Parser::KeyWordTokenRule(T_LPAR, "("));
  lexer.addRule(new Parser::KeyWordTokenRule(T_RPAR, ")"));
  lexer.addIgnoredToken(T_WHITESPACE);

  Parser::ConcreteSyntaxTree cts;

  Parser::ConcreteSyntaxTree cst;
  PlotFormulaGrammar::get()->parse(lexer, cst);
  return __plot_formula_process_expression(cst[0], lexer, context);
}
