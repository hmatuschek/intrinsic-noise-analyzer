#include "plotformulaparser.hh"
#include <utils/logger.hh>
#include <eval/bci/bci.hh>
#include <parser/expr/lexer.hh>
#include <parser/expr/productions.hh>
#include <parser/expr/assembler.hh>

using namespace iNA;


/* ******************************************************************************************** *
 * Implementation of Tokenrules for Plot formula parser:
 * ******************************************************************************************** */
class ColumnIdentifierTokenRule : public Parser::TokenRule
{
public:
  ColumnIdentifierTokenRule()
    : Parser::TokenRule(Parser::Expr::T_IDENTIFIER)
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
 * Implementation of PlotFormulaParser::Context
 * ******************************************************************************************** */
PlotFormulaParser::Context::Context(Table *table)
  : _table(table)
{
  // Assign for each column a symbol.
  _symbols.resize(_table->getNumColumns());
  for (size_t i=0; i<_table->getNumColumns(); i++) {
    _symbols[i] = GiNaC::symbol(table->getColumnName(i).toStdString());
    _symbol_table[_symbols[i]] = i;
  }
}

PlotFormulaParser::Context::Context(const Context &other)
  : _table(other._table), _symbols(other._symbols), _symbol_table(other._symbol_table)
{
  // pass...
}


GiNaC::symbol
PlotFormulaParser::Context::resolve(const std::string &identifier) {
  if (0 == identifier.size()) {
    SymbolError err; err << "Can not resolve symbol '" << identifier << "'."; throw err;
  }
  if ('$' != identifier[0]) {
    SymbolError err; err << "Can not resolve symbol '" << identifier << "'."; throw err;
  }
  char *end_ptr=0;
  unsigned long column = ::strtoul(identifier.c_str()+1, &end_ptr, 10);
  if (size_t(end_ptr-identifier.c_str()) != identifier.size()) {
    SymbolError err; err << "Can not resolve symbol '" << identifier << "'."; throw err;
  }
  return getColumnSymbol(size_t(column));
}

std::string
PlotFormulaParser::Context::identifier(GiNaC::symbol symbol) const {
  size_t column = getColumnIdx(symbol);
  std::stringstream buffer; buffer << "$" << column;
  return buffer.str();
}

GiNaC::symbol
PlotFormulaParser::Context::getColumnSymbol(size_t column) const {
  return _symbols[column];
}

size_t
PlotFormulaParser::Context::getColumnIdx(GiNaC::symbol symbol) const
{
  // Search for symbol in map
  std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less>::const_iterator item
      = _symbol_table.find(symbol);
  // If not found
  if (_symbol_table.end() == item) {
    SymbolError err;
    err << "Can not resolve symbol " << symbol << ": Unknown symbol.";
    throw err;
  }
  // otherwise, return index
  return item->second;
}

double
PlotFormulaParser::Context::operator ()(size_t row, GiNaC::ex expression) const
{
  // Create compiler, code and interpreter
  Eval::bci::Compiler<Eigen::VectorXd> compiler(_symbol_table);
  Eval::bci::Code code; compiler.setCode(&code);
  Eval::bci::Interpreter<Eigen::VectorXd> interpreter;
  // Compile expression:
  try { compiler.compileExpressionAndStore(expression, 0); }
  catch (Exception &err) {
    Utils::Message message = LOG_MESSAGE(Utils::Message::ERROR);
    message << "Can not compile expression \""
            << expression << "\" to byte code: " << err.what();
    Utils::Logger::get().log(message);
    return std::numeric_limits<double>::quiet_NaN();
  }
  compiler.finalize(0); interpreter.setCode(&code);
  // Evaluate expression
  Eigen::VectorXd output(1);
  interpreter.run(_table->getRow(row), output);
  return output(0);
}


/* ******************************************************************************************** *
 * Implementation of PlotFormulaParser
 * ******************************************************************************************** */
bool
PlotFormulaParser::check(const QString &formula, Context &context)
{
  std::stringstream buffer; buffer << formula.toStdString();

  Parser::Lexer lexer(buffer);
  lexer.addRule(new Parser::WhiteSpaceTokenRule(Parser::Expr::T_WHITESPACE));
  lexer.addRule(new ColumnIdentifierTokenRule());
  lexer.addRule(new Parser::IdentifierTokenRule(Parser::Expr::T_IDENTIFIER));
  lexer.addRule(new Parser::IntegerTokenRule(Parser::Expr::T_INTEGER));
  lexer.addRule(new Parser::FloatTokenRule(Parser::Expr::T_FLOAT));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_COMMA, ","));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_PLUS, "+"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_MINUS, "-"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_TIMES, "*"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_POWER, "**"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_POWER, "^"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_DIVIDE, "/"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_LPAR, "("));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_RPAR, ")"));
  lexer.addIgnoredToken(Parser::Expr::T_WHITESPACE);

  try {
    Parser::ConcreteSyntaxTree cst;
    Parser::Expr::ExpressionGrammar::get()->parse(lexer, cst);
    Parser::Expr::Assembler(context, lexer).processExpression(cst[0]);
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
PlotFormulaParser::parse(const QString &formula, Context &context)
{
  std::stringstream buffer; buffer << formula.toStdString();

  Parser::Lexer lexer(buffer);
  lexer.addRule(new Parser::WhiteSpaceTokenRule(Parser::Expr::T_WHITESPACE));
  lexer.addRule(new ColumnIdentifierTokenRule());
  lexer.addRule(new Parser::IdentifierTokenRule(Parser::Expr::T_IDENTIFIER));
  lexer.addRule(new Parser::IntegerTokenRule(Parser::Expr::T_INTEGER));
  lexer.addRule(new Parser::FloatTokenRule(Parser::Expr::T_FLOAT));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_COMMA, ","));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_PLUS, "+"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_MINUS, "-"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_TIMES, "*"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_POWER, "**"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_POWER, "^"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_DIVIDE, "/"));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_LPAR, "("));
  lexer.addRule(new Parser::KeyWordTokenRule(Parser::Expr::T_RPAR, ")"));
  lexer.addIgnoredToken(Parser::Expr::T_WHITESPACE);

  Parser::ConcreteSyntaxTree cst;
  Parser::Expr::ExpressionGrammar::get()->parse(lexer, cst);
  return Parser::Expr::Assembler(context, lexer).processExpression(cst[0]);
}


void
PlotFormulaParser::serialize(GiNaC::ex formula, std::ostream &stream, const Context &context) {
  iNA::Parser::Expr::serializeExpression(formula, stream, context);
}
