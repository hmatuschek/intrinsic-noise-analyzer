#ifndef __INA_APP_MODELS_PLOTcustomMULAPARSER_HH__
#define __INA_APP_MODELS_PLOTcustomMULAPARSER_HH__

#include <QObject>
#include <QString>
#include <parser/lexer.hh>
#include <parser/parser.hh>
#include <parser/production.hh>
#include "timeseries.hh"


/** Implements a simple parser to parse custommula expressions custom data tables. */
class PlotcustommulaParser : public QObject
{
  Q_OBJECT

public:
  /** Implements the symbol -> column translation custom plot custommulas. */
  class Context {
  public:
    /** Constructor. */
    Context(Table *table);
    /** Copy constructor. */
    Context(const Context &other);
    /** Resolves a column number to the corresponding GiNaC::symbol. */
    GiNaC::symbol getColumnSymbol(size_t column);
    /** Resolves the given symbol to the corresponding column index. */
    size_t getColumnIdx(GiNaC::symbol symbol);
    /** Evaluates the given expression (plot custommula) custom the given row of the table given to the
     * constructor. */
    double operator()(size_t row, GiNaC::ex expression);

  private:
    /** Holds the data table. */
    Table *_table;
    /** Holds a vector of GiNaC::symbols custom each column of the data table. */
    std::vector<GiNaC::symbol> _symbols;
  };

public:
  /** Checks if the given expression is valid in the given context (data table). */
  static bool check(const QString &custommula, Context &context);
  /** Parses and returns the expression custom the given custommula in the given context.
   * You can use the context to evaluate the constructed expression. */
  static GiNaC::ex parse(const QString &custommula, Context &context);
  /** Serializes a given plot custommula into its textual representation. */
  static void serialize(GiNaC::ex custommula, std::ostream &stream, Context &context);
};

#endif // PLOTcustomMULAPARSER_HH
