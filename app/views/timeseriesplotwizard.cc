#include "timeseriesplotwizard.hh"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>


/* ******************************************************************************************** *
 * Implementation of TimeSeriesPlotDialog
 * ******************************************************************************************** */
TimeSeriesPlotDialog::TimeSeriesPlotDialog(Table *table, QWidget *parent)
  : QDialog(parent), _data(table)
{
  // Assemble view:
  _plot = 0;
  _plotview = new Plot::Canvas();
  _graph_list = new QListWidget();
  _add_graph  = new QPushButton(tr("+"));
  _rem_graph  = new QPushButton(tr("-"));

  QHBoxLayout *layout = new QHBoxLayout();
  QVBoxLayout *side_box = new QVBoxLayout();
  QHBoxLayout *button_box = new QHBoxLayout();

  button_box->addWidget(_add_graph);
  button_box->addWidget(_rem_graph);
  side_box->addWidget(_graph_list);
  side_box->addLayout(button_box);
  layout->addWidget(_plotview);
  layout->addLayout(side_box);

  setLayout(layout);
}


void
TimeSeriesPlotDialog::onAddGraph()
{

}

void
TimeSeriesPlotDialog::onRemoveGraph()
{
}




/* ******************************************************************************************** *
 * Implementation of NewTimeSeriesGraphDialog
 * ******************************************************************************************** */
NewTimeSeriesGraphDialog::NewTimeSeriesGraphDialog(Table *table, QWidget *parent)
  : QDialog(parent), _data(table)
{
  _formula = new QLineEdit();
  _select_column = new QPushButton();
  QHBoxLayout *layout = new QHBoxLayout();
  layout->addWidget(new QLabel("x(t)="));
  layout->addWidget(_formula);
  layout->addWidget(_select_column);
  setLayout(layout);
}


void
NewTimeSeriesGraphDialog::onSelectColumn()
{
  // pass...
}
