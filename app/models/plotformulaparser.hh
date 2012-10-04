#ifndef __INA_APP_MODELS_PLOTFORMULAPARSER_HH__
#define __INA_APP_MODELS_PLOTFORMULAPARSER_HH__

#include <QObject>
#include <QString>
#include <parser/lexer.hh>
#include <parser/parser.hh>
#include <parser/production.hh>
#include "../timeseries.hh"


class PlotFormulaParser : public QObject
{
  Q_OBJECT

public:
  class Context {
  public:
    Context(Table *table);
    GiNaC::symbol getColumnSymbol(size_t column);

    double operator()(size_t row, GiNaC::ex expression);

  private:
    Table *_table;
    std::vector<GiNaC::symbol> _symbols;
  };

public:
  static bool check(const QString &formula, Context &context);
  static GiNaC::ex parse(const QString &formula, Context &context);
};

#endif // PLOTFORMULAPARSER_HH
