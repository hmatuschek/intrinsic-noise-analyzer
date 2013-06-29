#ifndef __INA_APP_PLOT_PLOTCONFIGDIALOG_HH__
#define __INA_APP_PLOT_PLOTCONFIGDIALOG_HH__

#include <QDialog>
#include <QAbstractListModel>
#include <QStackedWidget>
#include <QListView>
#include <QComboBox>
#include <QPushButton>
#include <QTimer>


// Forward declarations:
class Table;
namespace Plot {
class PlotConfig;
class AbstractGraphConfig;
class LineGraphConfig;
class VarianceLineGraphConfig;
class Canvas;
class Figure;
}


namespace Plot {

/** Implements a simple list model of graphs of a plot configuration. */
class GraphConfigList : public QAbstractListModel
{
  Q_OBJECT

public:
  /** Constructor. */
  GraphConfigList(PlotConfig *config, QObject *parent=0);

  /** Returns the number of graphs. */
  virtual int rowCount(const QModelIndex &parent) const;
  /** Returns the graph label for the list index. */
  virtual QVariant data(const QModelIndex &index, int role) const;
  /** Returns the graph config for the given graph (weak reference). */
  AbstractGraphConfig *graph(int idx);
  /** Adds a graph config to the list. */
  void addGraph(AbstractGraphConfig *graph);
  /** Removes a graph from the list and frees the graph config instance. */
  void removeGraph(int idx);
  /** Update graph at index, the ownership of the graph config is transferred to the plot config. */
  void updateGraph(int idx, AbstractGraphConfig *graph);

private:
  /** The list of graphs. */
  PlotConfig *_config;
};


/** Implements the plot configuration dialog. This dialog is used to assemble a new or edit an
 * existing plot configuration stored in a @c Plot::PlotConfig instance.
 * @ingroup plot. */
class PlotConfigDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor, table species the data table to create graphs for. */
  explicit PlotConfigDialog(PlotConfig *config, QWidget *parent = 0);

  /** Returns the number of currently configured graphs. */
  size_t numGraphs() const;
  /** Returns the i-th graph config. */
  AbstractGraphConfig *graph(size_t i);
  /** Returns the figure title. */
  const QString &figureTitle() const;
  /** Returns the x-label. */
  const QString &xLabel() const;
  /** Returns the y-label. */
  const QString &yLabel() const;

private slots:
  /** Callback to remove a selected graph from the graph list. */
  void onRemoveGraph();
  /** Callback to create a new line graph. */
  void onAddLineGraph();
  /** Callback to create a new variance line graph. */
  void onAddVarLineGraph();
  /** Callback to edit a existing graph. */
  void onEditGraph(const QModelIndex &index);
  /** Updates the plot-preview window. */
  void onUpdatePlot();
  /** If the plot title is changed. */
  void onTitleChanged(QString title);
  /** If the x label is changed. */
  void onXLabelChanged(QString label);
  /** If the y label is changed. */
  void onYLabelChanged(QString label);
  /** If the min x range policy has changed. */
  void onMinXRangePolicyChanged(bool fixed);
  /** If the min x range has changed. */
  void onMinXRangeChanged(QString value);
  /** If the min x range policy has changed. */
  void onMaxXRangePolicyChanged(bool fixed);
  /** If the max x range has changed. */
  void onMaxXRangeChanged(QString value);
  /** If the min y range policy has changed. */
  void onMinYRangePolicyChanged(bool fixed);
  /** If the min y range has changed. */
  void onMinYRangeChanged(QString value);
  /** If the min y range policy has changed. */
  void onMaxYRangePolicyChanged(bool fixed);
  /** If the max y range has changed. */
  void onMaxYRangeChanged(QString value);
  /** Checks if the configuration is valid. */
  void onAccepted();

private:
  /** Holds a ListModel of the graphs. */
  PlotConfig *_config;
  /** Holds the configured graphs as a list model. */
  GraphConfigList _graph_list;
  /** List view for the graph list. */
  QListView *_graph_list_view;
  /** A stack widget that shows either the plot-preview or a label. */
  QStackedWidget *_stack;
  /** The preview plot view. */
  Plot::Canvas *_plotview;
  /** A Tab view allowing to switch between graph-list and labels&ranges */
  QTabWidget *_tabwidget;
  /** Text field for the x range min. */
  QLineEdit *_minXRange;
  /** Text field for the x range max. */
  QLineEdit *_maxXRange;
  /** Text field for the y range min. */
  QLineEdit *_minYRange;
  /** Text field for the y range max. */
  QLineEdit *_maxYRange;
  /** Replot timer, avoid the immediate replot on changes to the configuration. */
  QTimer _replotTimer;
};


/** A simple line editor widget to edit plot formulas, provides a button to select a certain
 * column of the table. */
class LinePlotFormulaEditor : public QWidget
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit LinePlotFormulaEditor(LineGraphConfig *config, QWidget *parent=0);
  /** Returns the formula as text. */
  QString getFormula() const;
  /** Resets the formula. */
  void setFormula(const QString &formula) const;

private slots:
  /** Assembles a list of column names ad shows them as a completer. */
  void showPopUp();
  /** Callback for "select column" button. */
  void onColumnSelected(QModelIndex index);

private:
  LineGraphConfig *_config;
  /** The line edit for the formula. */
  QLineEdit *_formula;
  /** The column-id completer. */
  QCompleter *_columns;
  /** The "select column" button, starts the completer. */
  QPushButton *_select_column;
};


/** A trivial button that shows a colored field and displays a QColorDialog on
 * click. */
class ColorButton: public QPushButton
{
  Q_OBJECT

public:
  /** Shows the given color. */
  explicit ColorButton(const QColor &color, QWidget *parent=0);
  /** Returns the currently selected color. */
  const QColor &selectedColor() const;

signals:
  /** Gets emitted if a new color was selected. */
  void colorSelected(QColor);

private slots:
  /** Internal callback to show color picker dialog. */
  void onSelectColor();

protected:
  /** Draws the button content. */
  virtual void paintEvent(QPaintEvent *event);

private:
  /** The currently selected color. */
  QColor _color;
};


/** A simple dialog to create a new line graph from the given config. */
class LineGraphDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor to edit an existing line graph. */
  explicit LineGraphDialog(LineGraphConfig *config, QWidget *parent=0);

private slots:
  /** Validates the plot formulas and calls accepted() if they are valid. */
  void checkInputAndExit();
  /** If the color select button was pressed. */
  void onSelectColor(QColor color);

private:
  /** Internal used post-constructor to setup the GUI elements of the dialog. */
  void __initGUI();

private:
  /** Holds the configuration of the graph. */
  LineGraphConfig *_config;
  /** Holds the label editor. */
  QLineEdit *_label;
  /** Hold the formula editor for the X values. */
  LinePlotFormulaEditor *_formula_x;
  /** Holds the formula editor for the mean formula. */
  LinePlotFormulaEditor *_formula_y;
  /** Holds the line color selection button. */
  ColorButton *_line_color;
};


/** A simple dialog to create a new variance line graph from the given config. */
class VarianceLineGraphDialog : public QDialog
{
  Q_OBJECT

public:
  /** Constructor to edit an existing var line graph. */
  explicit VarianceLineGraphDialog(VarianceLineGraphConfig *config, QWidget *parent=0);

private slots:
  /** Validates the plot formulas and calls accepted() if they are valid. */
  void checkInputAndExit();
  /** Gets called if a line color is selected. */
  void onSelectLineColor(QColor color);
  /** Gets called if a fill color is selected. */
  void onSelectFillColor(QColor color);

private:
  /** Internal used post-constructor to setup the GUI elements of the dialog. */
  void __initGUI();

private:
  /** Holds the configuration of the graph. */
  VarianceLineGraphConfig *_config;
  /** Holds the label editor. */
  QLineEdit *_label;
  /** Hold the formula editor for the X values. */
  LinePlotFormulaEditor *_formula_x;
  /** Holds the formula editor for the mean formula. */
  LinePlotFormulaEditor *_formula_y;
  /** Holds the formula editor for the var formula. */
  LinePlotFormulaEditor *_formula_var;
};


}

#endif // __INA_APP_PLOT_PLOTCONFIGDIALOG_HH__
