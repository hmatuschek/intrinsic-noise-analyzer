#include "plotcustommulaparser.hh"
#include <utils/logger.hh>

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
 * Implementation of Tokenrules custom Plot custommula parser:
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
 * customward declaration of parser productions...
 * ******************************************************************************************** */
class PlotcustommulaGrammar : public iNA::Parser::Production {
public:
  static iNA::Parser::Production *get();
protected:
  PlotcustommulaGrammar();
  static PlotcustommulaGrammar *_instance;
};

class PlotcustommulaProduction : public iNA::Parser::AltProduction {
public:
  static iNA::Parser::Production *get();
protected:
  PlotcustommulaProduction();
  static PlotcustommulaProduction *_instance;
};

class PlotcustommulaProductProduction : public iNA::Parser::AltProduction {
public:
  static iNA::Parser::Production *get();
protected:
  PlotcustommulaProductProduction();
  static PlotcustommulaProductProduction *_instance;
};

class PlotcustommulaPowerProduction : public iNA::Parser::AltProduction {
public:
  static iNA::Parser::Production *get();
protected:
  PlotcustommulaPowerProduction();
  static PlotcustommulaPowerProduction *_instance;
};

class PlotcustommulaAtomicProduction : public iNA::Parser::AltProduction {
public:
  static iNA::Parser::Production *get();
protected:
  PlotcustommulaAtomicProduction();
  static PlotcustommulaAtomicProduction *_instance;
};

class PlotcustommulaFunction : public iNA::Parser::Production {
public:
  static iNA::Parser::Production *get();
public:
  PlotcustommulaFunction();
  static PlotcustommulaFunction *_instance;
};

class PlotcustommulaFunctionArgumentList : public iNA::Parser::Production {
public:
  static iNA::Parser::Production *get();
protected:
  PlotcustommulaFunctionArgumentList();
  static PlotcustommulaFunctionArgumentList *_instance;
};


/* ******************************************************************************************** *
 *  FunctionArgumentList := Expression [, FunctionArgumentList];
 * ******************************************************************************************** */
PlotcustommulaFunctionArgumentList *PlotcustommulaFunctionArgumentList::_instance = 0;
Parser::Production *PlotcustommulaFunctionArgumentList::get() {
  if (0 == _instance) {
    return new PlotcustommulaFunctionArgumentList();
  }
  return _instance;
}

PlotcustommulaFunctionArgumentList::PlotcustommulaFunctionArgumentList()
  : Parser::Production()
{
  _instance = this;
  elements.push_back(PlotcustommulaProduction::get());
  elements.push_back(
        new Parser::OptionalProduction(
          new Parser::Production(2, new Parser::TokenProduction(T_COMMA), this)));
}

/* ******************************************************************************************** *
 *  Identifier ([FunctionArgumentList]);
 * ******************************************************************************************** */
PlotcustommulaFunction *PlotcustommulaFunction::_instance = 0;
Parser::Production *PlotcustommulaFunction::get() {
  if (0 == PlotcustommulaFunction::_instance) {
    return new PlotcustommulaFunction();
  }
  return PlotcustommulaFunction::_instance;
}

PlotcustommulaFunction::PlotcustommulaFunction()
  : Parser::Production()
{
  _instance = this;
  elements.push_back(new Parser::TokenProduction(T_IDENTIFIER));
  elements.push_back(new Parser::TokenProduction(T_LPAR));
  elements.push_back(new Parser::OptionalProduction(PlotcustommulaFunctionArgumentList::get()));
  elements.push_back(new Parser::TokenProduction(T_RPAR));
}

/* ******************************************************************************************** *
 *  Number | FunctionCall | Identifier | ("(" Expression ")") | "-" AtomicExpression;
 * ******************************************************************************************** */
PlotcustommulaAtomicProduction *PlotcustommulaAtomicProduction::_instance = 0;
Parser::Production *PlotcustommulaAtomicProduction::get() {
  if (0 == _instance) { return new PlotcustommulaAtomicProduction(); }
  return _instance;
}

PlotcustommulaAtomicProduction::PlotcustommulaAtomicProduction()
  : Parser::AltProduction()
{
  _instance = this;
  alternatives.push_back(new Parser::TokenProduction(T_INTEGER));
  alternatives.push_back(new Parser::TokenProduction(T_FLOAT));
  alternatives.push_back(PlotcustommulaFunction::get());
  alternatives.push_back(new Parser::TokenProduction(T_COLUMN_ID));
  alternatives.push_back(
        new Parser::Production(
          3, new Parser::TokenProduction(T_LPAR),
          PlotcustommulaProduction::get(),
          new Parser::TokenProduction(T_RPAR)));
  alternatives.push_back(
        new Parser::Production(
          2, new Parser::TokenProduction(T_MINUS), this));
}


/* ******************************************************************************************** *
 * (AtomicExpression ("**" | "^") PowerExpression) | AtomicExpression;
 * ******************************************************************************************** */
PlotcustommulaPowerProduction *PlotcustommulaPowerProduction::_instance = 0;
Parser::Production *PlotcustommulaPowerProduction::get() {
  if (0 == _instance) { return new PlotcustommulaPowerProduction(); }
  return _instance;
}

PlotcustommulaPowerProduction::PlotcustommulaPowerProduction()
  : Parser::AltProduction()
{
  _instance = this;
  alternatives.push_back(
        new Parser::Production(
          3,
          PlotcustommulaAtomicProduction::get(),
          new Parser::TokenProduction(T_POWER),
          PlotcustommulaPowerProduction::get()));
  alternatives.push_back(PlotcustommulaAtomicProduction::get());
}


/* ******************************************************************************************** *
 * (PowerExpression ("*" | "/") ProductExpression) | PowerExpression;
 * ******************************************************************************************** */
PlotcustommulaProductProduction *PlotcustommulaProductProduction::_instance = 0;
Parser::Production *PlotcustommulaProductProduction::get() {
  if (0 == _instance) { return new PlotcustommulaProductProduction(); }
  return _instance;
}

PlotcustommulaProductProduction::PlotcustommulaProductProduction()
  : Parser::AltProduction()
{
  _instance = this;
  alternatives.push_back(
        new Parser::Production(
          3,
          PlotcustommulaPowerProduction::get(),
          new Parser::AltProduction(
            2, new Parser::TokenProduction(T_TIMES),
            new Parser::TokenProduction(T_DIVIVE)),
          PlotcustommulaProductProduction::get()));
  alternatives.push_back(PlotcustommulaPowerProduction::get());
}


/* ******************************************************************************************** *
 * (ProductExpression ('+'|'-') Expression) | ProductExpression
 * ******************************************************************************************** */
PlotcustommulaProduction *PlotcustommulaProduction::_instance = 0;

Parser::Production *
PlotcustommulaProduction::get() {
  if (0 == _instance) { return new PlotcustommulaProduction(); }
  return _instance;
}

PlotcustommulaProduction::PlotcustommulaProduction()
  : Parser::AltProduction()
{
  _instance = this;
  alternatives.push_back(
        new Parser::Production(
          3,
          PlotcustommulaProductProduction::get(),
          new Parser::AltProduction(
            2, new Parser::TokenProduction(T_PLUS),
            new Parser::TokenProduction(T_MINUS)),
          PlotcustommulaProduction::get()));
  alternatives.push_back(PlotcustommulaProductProduction::get());
}


/* ******************************************************************************************** *
 * Implementation of Plot custommula grammar
 * Grammar = PlotcustommulaProduction END_OF_LINE
 * ******************************************************************************************** */
PlotcustommulaGrammar *PlotcustommulaGrammar::_instance = 0;

Parser::Production *
PlotcustommulaGrammar::get() {
  if (0 == _instance) { return new PlotcustommulaGrammar(); }
  return _instance;
}

PlotcustommulaGrammar::PlotcustommulaGrammar()
  : Parser::Production()
{
  _instance = this;
  elements.push_back(PlotcustommulaProduction::get());
  elements.push_back(new Parser::TokenProduction(Parser::Token::END_OF_INPUT));
}

/* ******************************************************************************************** *
 * Implementation of assembler:
 * ******************************************************************************************** */
void __plot_custommula_process_function_arguments(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx, std::vector<GiNaC::ex> &args);
GiNaC::ex __plot_custommula_process_function(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx);
GiNaC::ex __plot_custommula_process_atomic(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx);
GiNaC::ex __plot_custommula_process_power(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx);
GiNaC::ex __plot_custommula_process_product(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx);
GiNaC::ex __plot_custommula_process_expression(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx);

void __plot_custommula_process_function_arguments(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx, std::vector<GiNaC::ex> &args)
{
  args.push_back(__plot_custommula_process_expression(node[0], lexer, ctx));

  if (node[1].matched()) {
    __plot_custommula_process_function_arguments(node[1][0][1], lexer, ctx, args);
  }
}

GiNaC::ex __plot_custommula_process_function(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx)
{
  std::string function_name = lexer[node[0].getTokenIdx()].getValue();
  std::vector<GiNaC::ex> arguments;
  if (node[2].matched()) {
    __plot_custommula_process_function_arguments(node[2][0], lexer, ctx, arguments);
  }

  // Dispatch function name:
  if("sqrt" == function_name) {
    if (1 != arguments.size()) {
      Parser::SyntaxError err;
      err << "Function 'sqrt' takes exactly 1 argument, " << arguments.size() << " are given.";
      throw err;
    }
    return GiNaC::sqrt(arguments[0]);
  } else if ("log" == function_name) {
    if (1 != arguments.size()) {
      Parser::SyntaxError err;
      err << "Function 'log' takes exactly 1 argument, " << arguments.size() << " are given.";
      throw err;
    }
    return GiNaC::log(arguments[0]);
  } else if ("exp" == function_name) {
    if (1 != arguments.size()) {
      Parser::SyntaxError err;
      err << "Function 'exp' takes exactly 1 argument, " << arguments.size() << " are given.";
      throw err;
    }
    return GiNaC::exp(arguments[0]);
  }

  Parser::SyntaxError err;
  err << "Unkown function " << function_name;
  throw err;
}

GiNaC::ex __plot_custommula_process_atomic(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx)
{
  switch (node.getAltIdx()) {
  // On integer | float
  case 0:
  case 1: {
    std::stringstream buffer; buffer << lexer[node[0].getTokenIdx()].getValue();
    double value; buffer >> value;
    return value;
  }

  // On function call
  case 2: {
    GiNaC::ex value = __plot_custommula_process_function(node[0], lexer, ctx);
    return value;
  }

  // On column identifier
  case 3: {
    std::stringstream buffer;
    buffer << lexer[node[0].getTokenIdx()].getValue().substr(1);
    size_t index; buffer >> index;
    return ctx.getColumnSymbol(index);
  }

  // On ( Expression )
  case 4: {
    return __plot_custommula_process_expression(node[0][1], lexer, ctx);
  }

  // On negation
  case 5: {
    return -__plot_custommula_process_atomic(node[0][1], lexer, ctx);
  }

  default: break;
  }

  throw InternalError(__FILE__ ": Invalid CST node!");
}

GiNaC::ex __plot_custommula_process_power(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx) {
  if (0 == node.getAltIdx()) {
    GiNaC::ex lhs = __plot_custommula_process_atomic(node[0][0], lexer, ctx);
    GiNaC::ex rhs = __plot_custommula_process_power(node[0][2], lexer, ctx);
    return GiNaC::pow(lhs, rhs);
  }
  return __plot_custommula_process_atomic(node[0], lexer, ctx);
}

GiNaC::ex __plot_custommula_process_product(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx) {
  if (0 == node.getAltIdx()) {
    GiNaC::ex lhs = __plot_custommula_process_power(node[0][0], lexer, ctx);
    GiNaC::ex rhs = __plot_custommula_process_product(node[0][2], lexer, ctx);
    if (0 == node[0][1].getAltIdx()) {
      return lhs * rhs;
    }
    return lhs / rhs;
  }
  return __plot_custommula_process_power(node[0], lexer, ctx);
}

GiNaC::ex __plot_custommula_process_expression(Parser::ConcreteSyntaxTree &node, Parser::Lexer &lexer, PlotcustommulaParser::Context &ctx) {
  if (0 == node.getAltIdx()) {
    GiNaC::ex lhs = __plot_custommula_process_product(node[0][0], lexer, ctx);
    GiNaC::ex rhs = __plot_custommula_process_expression(node[0][2], lexer, ctx);
    if (0 == node[0][1].getAltIdx()) {
      return lhs + rhs;
    }
    return lhs-rhs;
  }
  return __plot_custommula_process_product(node[0], lexer, ctx);
}



/* ******************************************************************************************** *
 * Implementation of PlotcustommulaSerializer
 * ******************************************************************************************** */
class PlotcustommulaSerializer
    : public GiNaC::visitor, public GiNaC::basic::visitor, public GiNaC::add::visitor,
    public GiNaC::mul::visitor, public GiNaC::power::visitor, public GiNaC::function::visitor,
    public GiNaC::symbol::visitor, public GiNaC::numeric::visitor
{
protected:
  PlotcustommulaParser::Context &_context;
  int _current_precedence;
  std::ostream &_stream;

public:
  PlotcustommulaSerializer(std::ostream &stream, PlotcustommulaParser::Context &context)
    : _context(context), _stream(stream) {
    // pass...
  }

  virtual void visit(const GiNaC::add &node) {
    int old_precedence = _current_precedence;
    _current_precedence = 1;

    if (old_precedence > _current_precedence) { _stream << "("; }
    if (0 < node.nops()) {
      node.op(0).accept(*this);
      custom (size_t i=1; i<node.nops(); i++) {
        _stream << "+";
        node.op(i).accept(*this);
      }
    }
    if (old_precedence > _current_precedence) { _stream << ")"; }

    _current_precedence = old_precedence;
  }

  virtual void visit(const GiNaC::mul &node) {
    int old_precedence = _current_precedence; _current_precedence = 2;
    if (old_precedence > _current_precedence) { _stream << "("; }

    if (0 < node.nops()) {
      node.op(0).accept(*this);
      custom (size_t i=1; i<node.nops(); i++) {
        _stream << "*";
        node.op(i).accept(*this);
      }
    }

    if (old_precedence > _current_precedence) { _stream << ")"; }
    _current_precedence = old_precedence;
  }

  virtual void visit(const GiNaC::power &node) {
    int old_precedence = _current_precedence; _current_precedence = 3;
    if (old_precedence >= _current_precedence) { _stream << "("; }

    node.op(0).accept(*this);
    _stream << "^";
    node.op(1).accept(*this);

    if (old_precedence >= _current_precedence) { _stream << ")"; }
    _current_precedence = old_precedence;
  }

  virtual void visit(const GiNaC::function &node) {
    // Dispatch by function serial:
    if (node.get_serial() == GiNaC::abs_SERIAL::serial) {
      _stream << "abs("; node.op(0).accept(*this); _stream << ")"; return;
    }

    if (node.get_serial() == GiNaC::exp_SERIAL::serial) {
      _stream << "exp("; node.op(0).accept(*this); _stream << ")"; return;
    }

    if (node.get_serial() == GiNaC::log_SERIAL::serial) {
      _stream << "log("; node.op(0).accept(*this); _stream << ")"; return;
    }

    InternalError err;
    err << "Can not serialize function call " << node << ": Unknown function.";
    throw err;
  }

  virtual void visit(const GiNaC::symbol &node) {
    _stream << "$" << _context.getColumnIdx(node);
  }

  virtual void visit(const GiNaC::numeric &node) {
    if (node.is_integer()) {
      _stream << node.to_long();
    } else if (node.is_real()) {
      _stream << node.to_double();
    }
  }

  virtual void visit(const GiNaC::basic &node) {
    InternalError err; err << "Can not serialize expression " << node << ": unknown expression type.";
    throw err;
  }

};


/* ******************************************************************************************** *
 * Implementation of PlotcustommulaParser
 * ******************************************************************************************** */
PlotcustommulaParser::Context::Context(Table *table)
  : _table(table)
{
  // Assign custom each column a symbol.
  _symbols.resize(_table->getNumColumns());
  custom (size_t i=0; i<_table->getNumColumns(); i++) {
    _symbols[i] = GiNaC::symbol(table->getColumnName(i).toStdString());
  }
}

PlotcustommulaParser::Context::Context(const Context &other)
  : _table(other._table), _symbols(other._symbols)
{
  // pass...
}


GiNaC::symbol
PlotcustommulaParser::Context::getColumnSymbol(size_t column) {
  return _symbols[column];
}

size_t
PlotcustommulaParser::Context::getColumnIdx(GiNaC::symbol symbol) {
  custom (size_t i=0; i<_symbols.size(); i++) {
    if (_symbols[i] == symbol) { return i; }
  }

  SymbolError err;
  err << "Can not resolve symbol " << symbol << ": Unknown symbol.";
  throw err;
}

double
PlotcustommulaParser::Context::operator ()(size_t row, GiNaC::ex expression)
{
  GiNaC::exmap values;
  custom (size_t i=0; i<_table->getNumColumns(); i++) {
    values[_symbols[i]] = (*_table)(row, i);
  }

  GiNaC::ex value;
  try {
    value = GiNaC::evalf(expression.subs(values));
  } catch (std::runtime_error &err) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::ERROR);
    message << "Can not evaluate expression " << expression
            << ". Got: " << err.what();
    iNA::Utils::Logger::get().log(message);
    return std::numeric_limits<double>::quiet_NaN();
  } catch (std::exception &err) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::ERROR);
    message << "Can not evaluate expression " << expression
            << ". Got: " << err.what();
    iNA::Utils::Logger::get().log(message);
    return std::numeric_limits<double>::quiet_NaN();
}

  if (! GiNaC::is_a<GiNaC::numeric>(value)) {
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::ERROR);
    message << "Can not evaluate expression " << expression
            << ". Value not numeric: " << value;
    iNA::Utils::Logger::get().log(message);
    return std::numeric_limits<double>::quiet_NaN();
  }
  return GiNaC::ex_to<GiNaC::numeric>(value).to_double();
}


bool
PlotcustommulaParser::check(const QString &custommula, Context &context)
{
  std::stringstream buffer; buffer << custommula.toStdString();

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
    PlotcustommulaGrammar::get()->parse(lexer, cst);
    __plot_custommula_process_expression(cst[0], lexer, context);
  } catch (Parser::ParserError &err) {
    return false;
  } catch (std::exception &err) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Unexpected exception: " << err.what();
    Utils::Logger::get().log(message);
    return false;
  }

  return true;
}


GiNaC::ex
PlotcustommulaParser::parse(const QString &custommula, Context &context)
{
  std::stringstream buffer; buffer << custommula.toStdString();

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
  PlotcustommulaGrammar::get()->parse(lexer, cst);
  return __plot_custommula_process_expression(cst[0], lexer, context);
}


void
PlotcustommulaParser::serialize(GiNaC::ex custommula, std::ostream &stream, Context &context)
{
  PlotcustommulaSerializer serializer(stream, context);
  custommula.accept(serializer);
}
