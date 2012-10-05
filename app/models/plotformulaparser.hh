#ifndef __INA_APP_MODELS_PLOTFORMULAPARSER_HH__
#define __INA_APP_MODELS_PLOTFORMULAPARSER_HH__

#include <QObject>
#include <QString>
#include <parser/lexer.hh>
#include <parser/parser.hh>
#include <parser/production.hh>
#include "../timeseries.hh"


/** Implements a simple parser to parse formula expressions for data tables. */
class PlotFormulaParser : public QObject
{
  Q_OBJECT

public:
  /** Implements the symbol -> column translation for plot formulas. */
  class Context {
  public:
    /** Constructor. */
    Context(Table *table);
    /** Copy constructor. */
    Context(const Context &other);
    /** Resolves a column number to the corresponding GiNaC::symbol. */
    GiNaC::symbol getColumnSymbol(size_t column);
    /** Evaluates the given expression (plot formula) for the given row of the table given to the
     * constructor. */
    double operator()(size_t row, GiNaC::ex expression);

  private:
    /** Holds the data table. */
    Table *_table;
    /** Holds a vector of GiNaC::symbols for each column of the data table. */
    std::vector<GiNaC::symbol> _symbols;
  };

public:
  /** Checks if the given expression is valid in the given context (data table). */
  static bool check(const QString &formula, Context &context);
  /** Parses and returns the expression for the given formula in the given context.
   * You can use the context to evaluate the constructed expression. */
  static GiNaC::ex parse(const QString &formula, Context &context);
};

#endif // PLOTFORMULAPARSER_HH
