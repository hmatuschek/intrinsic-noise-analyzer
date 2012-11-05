#include "replot.hh"
#include "../plot/plot.hh"
#include "../plot/linegraph.hh"
#include <QVector>


RETimeSeriesPlot::RETimeSeriesPlot(QList<QString> &selected_species, RETask *task, QObject *parent)
  : LinePlot("Mean concentrations (RE)", parent)
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
  Table *series = task->getTimeSeries();
  for (int i=0; i<selected_species.size(); i++) {
    iNA::Ast::Species *species =
        task->getConfig().getModel()->getSpecies(selected_species.at(i).toStdString());
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(species);
    addLineGraph(series->getColumn(0), series->getColumn(1+species_idx),
                 series->getColumnName(1+species_idx));
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}
