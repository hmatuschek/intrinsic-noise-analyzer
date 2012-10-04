#ifndef __INA_APP_VIEWS_TIMESERIESPLOTWIZARD_HH__
#define __INA_APP_VIEWS_TIMESERIESPLOTWIZARD_HH__

#include <QDialog>
#include <QListWidget>
#include <QPushButton>

#include "../timeseries.hh"
#include "../plot/canvas.hh"
#include "../plot/figure.hh"


/** A simple interactive plot-dialog to assemble new plots. */
class TimeSeriesPlotDialog : public QDialog
{
  Q_OBJECT

public:
  explicit TimeSeriesPlotDialog(Table *table, QWidget *parent = 0);
  
private slots:
  void onRemoveGraph();
  void onAddGraph();

private:
  Table *_data;
  Plot::Canvas *_plotview;
  Plot::Figure *_plot;
  QListWidget *_graph_list;
  QPushButton *_add_graph;
  QPushButton *_rem_graph;
};


/** A simple dialog to create a new graph for a time series plot. */
class NewTimeSeriesGraphDialog : public QDialog
{
  Q_OBJECT

public:
  explicit NewTimeSeriesGraphDialog(Table *table, QWidget *parent=0);

private slots:
  void onSelectColumn();

private:
  Table *_data;
  QLineEdit *_formula;
  QPushButton *_select_column;
};

#endif // __INA_APP_VIEWS_TIMESERIESPLOTWIZARD_HH__
