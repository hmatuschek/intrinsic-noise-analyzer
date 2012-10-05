#ifndef __INA_APP_VIEWS_TIMESERIESPLOTWIZARD_HH__
#define __INA_APP_VIEWS_TIMESERIESPLOTWIZARD_HH__

#include <QDialog>
#include <QListView>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QAbstractListModel>
#include <QLineEdit>

#include "../timeseries.hh"
#include "../plot/canvas.hh"
#include "../plot/figure.hh"
#include "../models/plotformulaparser.hh"


class TimeSeriesGraphConfig
{
public:
  typedef enum {
    LINE_GRAPH,
    VARIANCE_GRAPH
  } PlotType;

protected:
  Table *_table;
  PlotFormulaParser::Context _context;
  PlotType _type;
  GiNaC::ex _mean_expression;
  GiNaC::ex _var_expression;
  QString _label;

public:
  TimeSeriesGraphConfig(Table *table, PlotType type, size_t mean_column, size_t var_column=0);
  TimeSeriesGraphConfig(const TimeSeriesGraphConfig &other);

  inline void setPlotType(PlotType type) { _type = type; }
  inline bool setMeanColumn(const QString &formula) {
    if (! PlotFormulaParser::check(formula, _context)) { return false; }
    _mean_expression = PlotFormulaParser::parse(formula, _context);
    return true;
  }
  inline bool setVarColumn(const QString &formula) {
    if (! PlotFormulaParser::check(formula, _context)) { return false; }
    _var_expression = PlotFormulaParser::parse(formula, _context);
    return true;
  }
  inline void setLabel(const QString &label) { _label = label; }
  inline PlotType plotType() const { return _type; }
  inline GiNaC::ex meanColumn() const { return _mean_expression; }
  inline GiNaC::ex varColumn() const { return _var_expression; }
  inline const QString &label() const { return _label; }
  inline const Table *table() const { return _table; }

  Plot::Graph *create(const Plot::GraphStyle &style);

  double evalMean(size_t row);
  double evalVar(size_t row);
};



class TimeSeriesGraphList : public QAbstractListModel
{
  Q_OBJECT

public:
  TimeSeriesGraphList(QObject *parent=0);

  virtual int rowCount(const QModelIndex &parent) const;
  virtual QVariant data(const QModelIndex &index, int role) const;

  TimeSeriesGraphConfig &graph(int idx);
  void addGraph(const TimeSeriesGraphConfig &graph);

private:
  QList<TimeSeriesGraphConfig> _graphs;
};


/** A simple interactive plot-dialog to assemble new plots. */
class TimeSeriesPlotDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TimeSeriesPlotDialog(Table *table, QWidget *parent = 0);
  
private slots:
  void onRemoveGraph();
  void onAddGraph();
  void onUpdatePlot();

private:
  Table *_data;
  TimeSeriesGraphList _graphs;
  Plot::Canvas *_plotview;
  Plot::Figure *_plot;
  QListView *_graph_list;
  QPushButton *_add_graph;
  QPushButton *_rem_graph;
};


class TimeSeriesFormulaEditor : public QWidget
{
  Q_OBJECT

public:
  explicit TimeSeriesFormulaEditor(Table *table, QWidget *parent=0);

  inline QString getFormula() const { return _formula->text(); }

private slots:
  void showPopUp();
  void onColumnSelected(QModelIndex index);

private:
  Table *_table;
  QLineEdit *_formula;
  QCompleter *_columns;
  QPushButton *_select_column;
};



/** A simple dialog to create a new graph for a time series plot. */
class NewTimeSeriesGraphDialog : public QDialog
{
  Q_OBJECT

public:
  explicit NewTimeSeriesGraphDialog(Table *table, QWidget *parent=0);

  inline const TimeSeriesGraphConfig &getConfig() const { return _config; }

private slots:
  void onPlotTypeSelect(int index);
  void checkInputAndExit();

private:
  Table *_data;
  TimeSeriesGraphConfig _config;
  QComboBox *_plot_type;
  QLineEdit *_label;
  TimeSeriesFormulaEditor *_formula_mean;
  TimeSeriesFormulaEditor *_formula_var;
};

#endif // __INA_APP_VIEWS_TIMESERIESPLOTWIZARD_HH__
