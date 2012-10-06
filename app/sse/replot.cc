#include "replot.hh"
#include "../plot/plot.hh"
#include "../plot/linegraph.hh"
#include <QVector>


RETimeSeriesPlot::RETimeSeriesPlot(QList<QString> &selected_species, RETask *task, QObject *parent)
  : Plot::Figure("Mean concentrations (RE)", parent)
{
  // Get species unit
  std::stringstream unit_str;
  task->getSpeciesUnit().dump(unit_str, true);
  QString species_unit(unit_str.str().c_str());
  // Get time unit
  unit_str.str("");
  task->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());
  // Set axis labels with units:
  setXLabel(tr("time [%1]").arg(time_unit));
  if (task->getSpeciesUnit().isConcentrationUnit()) {
    setYLabel(tr("concentrations [%1]").arg(species_unit));
  } else {
    setYLabel(tr("amount [%1]").arg(species_unit));
  }

  /* Assemble plot. */
  QVector<Plot::LineGraph *> graphs(selected_species.size());

  // Maps each selected species to the column in the analyis data:
  QVector<size_t> mean_index_table(selected_species.size());
  for (int i=0; i<selected_species.size(); i++) {
    size_t idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    mean_index_table[i] = idx+1; // +1 since column 0 is time
  }

  // Allocate a graph for each colum in time-series:
  Table *series = task->getTimeSeries();
  for (int i=0; i<selected_species.size(); i++)
  {
    Plot::GraphStyle style = this->getStyle(i);
    graphs[i] = new Plot::LineGraph(style);
    this->axis->addGraph(graphs[i]);
    this->addToLegend(series->getColumnName(i+1), graphs[i]);
  }

  // Do not plot all
  int idx_incr = 0;
  if (0 == (idx_incr = series->getNumRows()/100)) { idx_incr = 1; }

  // Plot time-series:
  for (size_t j=0; j<series->getNumRows(); j+=idx_incr) {
    for (int i=0; i<selected_species.size(); i++) {
      graphs[i]->addPoint((*series)(j, 0), (*series)(j, mean_index_table[i]));
    }
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  for (int i=0; i<selected_species.size(); i++) {
    graphs[i]->commit();
  }

  this->updateAxes();
}
