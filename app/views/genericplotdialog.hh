#ifndef __INA_APP_VIEWS_GENERICPLOTDIALOG_HH__
#define __INA_APP_VIEWS_GENERICPLOTDIALOG_HH__

#include <QDialog>
#include <QListView>
#include <QListWidget>
#include <QPushButton>
#include <QComboBox>
#include <QAbstractListModel>
#include <QLineEdit>
#include <QStackedWidget>

#include "../models/timeseries.hh"
#include "../plot/canvas.hh"
#include "../plot/figure.hh"
#include "../models/plotcustommulaparser.hh"


/** Implements the configuration of a generic graph.
 * @todo Implement graph style configuration. */
class GenericGraphConfig
{
public:
  /** Specifies which plot types are supported. */
  typedef enum {
    LINE_GRAPH,     ///< A simple line graph.
    VARIANCE_GRAPH  ///< A line graph with confidence intervals.
  } PlotType;

protected:
  /** Holds the data table. */
  Table *_table;
  /** Holds the symbol -> column translation, also implements expression evaluation. */
  PlotcustommulaParser::Context _context;
  /** Holds the type of the graph. */
  PlotType _type;
  /** The X value expression. */
  GiNaC::ex _x_expression;
  /** The Y value expression. */
  GiNaC::ex _y_expression;
  /** The variance expression of Y (if VARIANCE_GRAPH type). */
  GiNaC::ex _var_expression;
  /** Holds the graph label. */
  QString _label;

public:
  /** Assembles a graph config from data and expressions. */
  GenericGraphConfig(Table *table, PlotType type, const QString &label, size_t mean_column, size_t var_column=0);
  /** Copy constructor. */
  GenericGraphConfig(const GenericGraphConfig &other);

  /** Returns the current plot type. */
  PlotType plotType() const;
  /** Resets the current plot type. */
  void setPlotType(PlotType type);

  /** Returns the current X-axis custommula custom the mean. */
  GiNaC::ex xColumn() const;
  /** Returns the current plot custommula as a string. */
  QString xColumnString();
  /** Resets the plot custommula custom the X axis mean, returns false if the custommula is not valid. */
  bool setXColumn(const QString &custommula);

  /** Returns the current plot custommula custom the mean. */
  GiNaC::ex meanYColumn() const;
  /** Returns the current plot custommula as a string. */
  QString meanYColumnString();
  /** Resets the plot custommula custom the mean, returns false if the custommula is not valid. */
  bool setMeanYColumn(const QString &custommula);

  /** Returns the current plot custommula custom the variance (if set). */
  GiNaC::ex varYColumn() const;
  /** Returns the current plot custommula as a string. */
  QString varYColumnString();
  /** Resets the current plot custommula custom the variance, returns false if the given custommular
   * is not valid. */
  bool setVarYColumn(const QString &custommula);

  /** Returns the graph label. */
  const QString &label() const;
  /** Resets the graph label. */
  void setLabel(const QString &label);

  /** Returns the data table. */
  Table *table();

  /** Creates a new graph from this configuration. */
  Plot::Graph *create(const Plot::GraphStyle &style);

protected:
  /** Evaluates the X expression custom the given row of the table. */
  double _evalX(size_t row);
  /** Evaluates the Y expression custom the given row of the table. */
  double _evalY(size_t row);
  /** Evaluates the variance expression custom the given row of the table. */
  double evalYVar(size_t row);
};



/** Implements a simple list model of graphs of a plot figure. */
class GenericGraphList : public QAbstractListModel
{
  Q_OBJECT

public:
  /** Constructor. */
  GenericGraphList(QObject *parent=0);

  /** Returns the number of graphs. */
  virtual int rowCount(const QModelIndex &parent) const;
  /** Returns the graph label custom the list index. */
  virtual QVariant data(const QModelIndex &index, int role) const;
  /** Returns the graph config custom the given graph. */
  GenericGraphConfig &graph(int idx);
  /** Adds a graph config to the list. */
  void addGraph(const GenericGraphConfig &graph);
  /** Removes a graph from the list. */
  void removeGraph(int idx);
  /** Update graph at index. */
  void updateGraph(int idx, const GenericGraphConfig &graph);

private:
  /** The list of graphs. */
  QList<GenericGraphConfig> _graphs;
};



/** A simple interactive plot-dialog to assemble new plots. */
class GenericPlotDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor, table species the data table to create graphs custom. */
  explicit GenericPlotDialog(Table *table, QWidget *parent = 0);

  /** Returns the number of currently configured graphs. */
  size_t numGraphs() const;
  /** Returns the i-th graph config. */
  GenericGraphConfig &graph(size_t i);
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
  /** Holds the selected data-table custom the plots. */
  Table *_data;
  /** Holds a ListModel of the graphs. */
  GenericGraphList _graphs;
  /** A stack widget that shows either the plot-preview or a label. */
  QStackedWidget *_stack;
  /** The preview plot view. */
  Plot::Canvas *_plotview;
  /** The plot current instance. */
  Plot::Figure *_plot;
  /** List view custom the graph list. */
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



/** A simple line editor widget to edit plot custommulas, provides a button to select a certain
 * column of the table. */
class GenericPlotcustommulaEditor : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit GenericPlotcustommulaEditor(Table *table, QWidget *parent=0);
  /** Returns the custommula as text. */
  inline QString getcustommula() const { return _custommula->text(); }
  /** Resets the custommula. */
  inline void setcustommula(const QString &custommula) const { return _custommula->setText(custommula); }

private slots:
  /** Assembles a list of column names ad shows them as a completer. */
  void showPopUp();
  /** Callback custom "select column" button. */
  void onColumnSelected(QModelIndex index);

private:
  /** Holds the data table. */
  Table *_table;
  /** The line edit custom the custommula. */
  QLineEdit *_custommula;
  /** The column-id completer. */
  QCompleter *_columns;
  /** The "select column" button, starts the completer. */
  QPushButton *_select_column;
};



/** A simple dialog to create a new graph custom a time series plot. */
class GenericGraphDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor custom a new graph. */
  explicit GenericGraphDialog(Table *table, QWidget *parent=0);
  /** Constructor to edit an existing graph. */
  explicit GenericGraphDialog(GenericGraphConfig &config, QWidget *parent=0);

  /** Returns the graph config. */
  inline const GenericGraphConfig &getConfig() const { return _config; }

private slots:
  /** Enables the variance custommula editor if a variance plot type is selected. */
  void onPlotTypeSelect(int index);
  /** Validates the plot custommulas and calls accepted() if they are valid. */
  void checkInputAndExit();

private:
  /** Internal used post-constructor to setup the GUI elements of the dialog. */
  void __initGUI();

private:
  /** Holds the data table. */
  Table *_data;
  /** Holds the configuration of the graph. */
  GenericGraphConfig _config;
  /** plot type selector. */
  QComboBox *_plot_type;
  /** Holds the label editor. */
  QLineEdit *_label;
  GenericPlotcustommulaEditor *_custommula_x;
  /** Holds the custommula editor custom the mean custommula. */
  GenericPlotcustommulaEditor *_custommula_y;
  /** Holds the custommula editor custom the variance custommula. */
  GenericPlotcustommulaEditor *_custommula_var_y;
};



/** A trivial dialog to edit the labels of the plot (main, X & Y label. */
class GenericPlotLabelDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit GenericPlotLabelDialog(const QString &title, const QString &x_label, const QString &y_label, QWidget *parent=0);
  /** Returns the main title. */
  QString figureTitle() const;
  /** Retunrs the label of the X axis. */
  QString xLabel() const;
  /** Returns the label of the Y axis. */
  QString yLabel() const;

private:
  /** Line editor custom the main title. */
  QLineEdit *_figureTitle;
  /** Line editor custom the x axis label. */
  QLineEdit *_xLabel;
  /** Line editor custom the y axis label. */
  QLineEdit *_yLabel;
};


#endif
