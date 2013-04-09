#ifndef __INA_APP_MODELS_PLOTFORMULAPARSER_HH__
#define __INA_APP_MODELS_PLOTFORMULAPARSER_HH__

#include <QObject>
#include <QString>
#include <parser/lexer.hh>
#include <parser/parser.hh>
#include <parser/production.hh>
#include "timeseries.hh"


/** Implements a simple parser to parse formula expressions for data tables. */
class PlotFormulaParser : public QObject
{
  Q_OBJECT

public:
  /** Implements the symbol -> column translation for plot formulas. */
  class Context: public iNA::Parser::Expr::Context {
  public:
    /** Constructor. */
    Context(Table *table);
    /** Copy constructor. */
    Context(const Context &other);
    /** Implements the Context interface, resolves the given identifier to a @c GiNaC::symbol */
    virtual GiNaC::symbol resolve(const std::string &identifier);
    /** Implements the Context interface, returns the identifier for the given symbol. */
    virtual std::string identifier(GiNaC::symbol symbol) const;
    /** Resolves a column number to the corresponding GiNaC::symbol. */
    GiNaC::symbol getColumnSymbol(size_t column) const;
    /** Resolves the given symbol to the corresponding column index. */
    size_t getColumnIdx(GiNaC::symbol symbol) const;
    /** Evaluates the given expression (plot formula) for the given row of the table given to the
     * constructor. */
    double operator()(size_t row, GiNaC::ex expression) const;

  private:
    /** Holds the data table. */
    Table *_table;
    /** Holds a vector of GiNaC::symbols for each column of the data table. */
    std::vector<GiNaC::symbol> _symbols;
    /** Maps symbols to column indices. */
    std::map<GiNaC::symbol, size_t, GiNaC::ex_is_less> _symbol_table;
  };

public:
  /** Checks if the given expression is valid in the given context (data table). */
  static bool check(const QString &formula, Context &context);
  /** Parses and returns the expression for the given formula in the given context.
   * You can use the context to evaluate the constructed expression. */
  static GiNaC::ex parse(const QString &formula, Context &context);
  /** Serializes a given plot formula into its textual representation. */
  static void serialize(GiNaC::ex formula, std::ostream &stream, const Context &context);
};

#endif // PLOTFORMULAPARSER_HH
