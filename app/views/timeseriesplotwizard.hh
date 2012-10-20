#ifndef __INA_APP_VIEWS_TIMESERIESPLOTWIZARD_HH__
#define __INA_APP_VIEWS_TIMESERIESPLOTWIZARD_HH__

#include <QDialog>
#include <QListView>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QAbstractListModel>
#include <QLineEdit>
#include <QStackedWidget>

#include "../timeseries.hh"
#include "../plot/canvas.hh"
#include "../plot/figure.hh"
#include "../models/plotformulaparser.hh"


/** Implements the configuration of a time-series graph. */
class TimeSeriesGraphConfig
{
public:
  /** Specifies which plot types are supported. */
  typedef enum {
    LINE_GRAPH,
    VARIANCE_GRAPH
  } PlotType;

protected:
  /** Holds the data table. */
  Table *_table;
  /** Holds the symbol -> column translation, also implements expression evaluation. */
  PlotFormulaParser::Context _context;
  /** Holds the type of the graph. */
  PlotType _type;
  /** The mean value expression. */
  GiNaC::ex _mean_expression;
  /** The variance expression if needed. */
  GiNaC::ex _var_expression;
  /** Holds the graph label. */
  QString _label;

public:
  TimeSeriesGraphConfig(Table *table, PlotType type, const QString &label, size_t mean_column, size_t var_column=0);
  TimeSeriesGraphConfig(const TimeSeriesGraphConfig &other);

  /** Returns the current plot type. */
  PlotType plotType() const;
  /** Resets the current plot type. */
  void setPlotType(PlotType type);

  /** Returns the current plot formula for the mean. */
  GiNaC::ex meanColumn() const;
  /** Returns the current plot formula as a string. */
  QString meanColumnString();
  /** Resets the plot formula for the mean, returns false if the formula is not valid. */
  bool setMeanColumn(const QString &formula);

  /** Returns the current plot formula for the variance (if set). */
  GiNaC::ex varColumn() const;
  /** Returns the current plot formula as a string. */
  QString varColumnString();
  /** Resets the current plot formula for the variance, returns false if the given formular
   * is not valid. */
  bool setVarColumn(const QString &formula);

  /** Returns the graph label. */
  const QString &label() const;
  /** Resets the graph label. */
  void setLabel(const QString &label);

  /** Returns the data table. */
  Table *table();

  /** Creates a new graph from this configuration. */
  Plot::Graph *create(const Plot::GraphStyle &style);

  /** Evaluates the mean expression for the given row of the table. */
  double evalMean(size_t row);
  /** Evaluates the variance expression for the given row of the table. */
  double evalVar(size_t row);
};



/** Implements a simple list model of graphs of a plot figure. */
class TimeSeriesGraphList : public QAbstractListModel
{
  Q_OBJECT

public:
  /** Constructor. */
  TimeSeriesGraphList(QObject *parent=0);

  /** Returns the number of graphs. */
  virtual int rowCount(const QModelIndex &parent) const;
  /** Returns the graph label for the list index. */
  virtual QVariant data(const QModelIndex &index, int role) const;
  /** Returns the graph config for the given graph. */
  TimeSeriesGraphConfig &graph(int idx);
  /** Adds a graph config to the list. */
  void addGraph(const TimeSeriesGraphConfig &graph);
  /** Removes a graph from the list. */
  void removeGraph(int idx);
  /** Update graph at index. */
  void updateGraph(int idx, const TimeSeriesGraphConfig &graph);

private:
  /** The list of graphs. */
  QList<TimeSeriesGraphConfig> _graphs;
};



/** A simple interactive plot-dialog to assemble new plots. */
class TimeSeriesPlotDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor, table species the data table to create graphs for. */
  explicit TimeSeriesPlotDialog(Table *table, QWidget *parent = 0);

  /** Returns the number of currently configured graphs. */
  size_t numGraphs() const;
  /** Returns the i-th graph config. */
  TimeSeriesGraphConfig &graph(size_t i);
  /** Returns the figure title. */
  QString figureTitle() const;
  /** Returns the x-label. */
  QString xLabel() const;
  /** Returns the y-label. */
  QString yLabel() const;

private slots:
  /** Callback to remove a selected graph from the graph list. */
  void onRemoveGraph();
  /** Callback to create a new graph. */
  void onAddGraph();
  /** Callback to edit figure labels & title. */
  void onEditLabels();
  /** Callbeck to edit a existing graph. */
  void onEditGraph(const QModelIndex &index);
  /** Updates the plot-preview window. */
  void onUpdatePlot();
  /** Checks if the configuration is valid. */
  void onAccepted();

private:
  /** Holds the selected data-table for the plots. */
  Table *_data;
  /** Holds a ListModel of the graphs. */
  TimeSeriesGraphList _graphs;
  /** A stack widget that shows either the plot-preview or a label. */
  QStackedWidget *_stack;
  /** The preview plot view. */
  Plot::Canvas *_plotview;
  /** The plot current instance. */
  Plot::Figure *_plot;
  /** List view for the graph list. */
  QListView *_graph_list;
  /** Add graph button. */
  QPushButton *_add_graph;
  /** Remove graph button. */
  QPushButton *_rem_graph;
  /** Edit figure labels. */
  QPushButton *_edit_labels;
  /** Figure title. */
  QString _figure_title;
  /** X label. */
  QString _x_label;
  /** Y label. */
  QString _y_label;

};



/** A simple line editor widget to edit plot formulas, provides a button to select a certain
 * column of the table. */
class TimeSeriesFormulaEditor : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit TimeSeriesFormulaEditor(Table *table, QWidget *parent=0);
  /** Returns the formula as text. */
  inline QString getFormula() const { return _formula->text(); }
  /** Resets the formula. */
  inline void setFormula(const QString &formula) const { return _formula->setText(formula); }

private slots:
  /** Assembles a list of column names ad shows them as a completer. */
  void showPopUp();
  /** Callback for "select column" button. */
  void onColumnSelected(QModelIndex index);

private:
  /** Holds the data table. */
  Table *_table;
  /** The line edit for the formula. */
  QLineEdit *_formula;
  /** The column-id completer. */
  QCompleter *_columns;
  /** The "select column" button, starts the completer. */
  QPushButton *_select_column;
};



/** A simple dialog to create a new graph for a time series plot. */
class TimeSeriesGraphDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor for a new graph. */
  explicit TimeSeriesGraphDialog(Table *table, QWidget *parent=0);
  /** Constructor to edit an existing graph. */
  explicit TimeSeriesGraphDialog(TimeSeriesGraphConfig &config, QWidget *parent=0);

  /** Returns the graph config. */
  inline const TimeSeriesGraphConfig &getConfig() const { return _config; }

private slots:
  /** Enables the variance formula editor if a variance plot type is selected. */
  void onPlotTypeSelect(int index);
  /** Validates the plot formulas and calls accepted() if they are valid. */
  void checkInputAndExit();

private:
  /** Internal used post-constructor to setup the GUI elements of the dialog. */
  void __initGUI();

private:
  /** Holds the data table. */
  Table *_data;
  /** Holds the configuration of the graph. */
  TimeSeriesGraphConfig _config;
  /** plot type selector. */
  QComboBox *_plot_type;
  /** Holds the label editor. */
  QLineEdit *_label;
  /** Holds the formula editor for the mean formula. */
  TimeSeriesFormulaEditor *_formula_mean;
  /** Holds the formula editor for the variance formula. */
  TimeSeriesFormulaEditor *_formula_var;
};



class TimeSeriesLabelDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TimeSeriesLabelDialog(const QString &title, const QString &x_label, const QString &y_label, QWidget *parent=0);

  QString figureTitle() const;
  QString xLabel() const;
  QString yLabel() const;

private:
  QLineEdit *_figureTitle;
  QLineEdit *_xLabel;
  QLineEdit *_yLabel;
};


#endif // __INA_APP_VIEWS_TIMESERIESPLOTWIZARD_HH__
