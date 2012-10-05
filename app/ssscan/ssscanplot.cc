#include "ssscanplot.hh"
#include "../plot/plot.hh"
#include "../plot/linegraph.hh"
#include <QVector>


ParameterScanPlot::ParameterScanPlot(size_t num_species, Table *series,
                                     const QString &species_unit, const QString &time_unit,
                                     QObject *parent)
    : Plot::Figure("Mean concentrations (RE & LNA)", parent)
{
  // Create a plot:
  this->setXLabel(tr("%1").arg(series->getColumnName(0)));
  this->setYLabel(tr("concentrations [%1]").arg(species_unit));

  QVector<Plot::VarianceLineGraph *> graphs(num_species);

  // Allocate a graph for each colum in time-series:
  for (size_t i=0; i<num_species; i++)
  {
    Plot::GraphStyle style = this->getStyle(i);
    graphs[i] = new Plot::VarianceLineGraph(style);
    this->axis->addGraph(graphs[i]);
    this->addToLegend(series->getColumnName(i+1), graphs[i]);
  }

  // Do not plot all
  int idx_incr = 0;
  if (0 == (idx_incr = series->getNumRows()/100))
  {
    idx_incr = 1;
  }

  // Plot time-series:
  for (size_t j=0; j<series->getNumRows(); j+=idx_incr)
  {
    size_t N = graphs.size();
    size_t offset = N+1; size_t increment = N;
    for (size_t i=0; i<num_species; i++)
    {
      graphs[i]->addPoint((*series)(j, 0), (*series)(j, 1+i), std::sqrt((*series)(j, offset)));
      offset += increment; increment -= 1;
    }
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  for (size_t i=0; i<num_species; i++)
  {
    graphs[i]->commit();
  }

  this->updateAxes();
}


ParameterScanIOSPlot::ParameterScanIOSPlot(size_t num_species, Table *series,
                                     const QString &species_unit, const QString &time_unit,
                                     QObject *parent)
    : Plot::Figure("Mean concentrations (EMRE & IOS var)", parent)
{
  // Create a plot:
  this->setXLabel(tr("%1").arg(series->getColumnName(0)));
  this->setYLabel(tr("concentrations [%1]").arg(species_unit));

  QVector<Plot::VarianceLineGraph *> graphs(num_species);

  size_t IOSoffset = num_species+num_species*(num_species+1)/2;

  // Allocate a graph for each colum in time-series:
  for (size_t i=0; i<num_species; i++)
  {
    Plot::GraphStyle style = this->getStyle(i);
    graphs[i] = new Plot::VarianceLineGraph(style);
    this->axis->addGraph(graphs[i]);
    this->addToLegend(series->getColumnName(i+1+IOSoffset), graphs[i]);
  }

  // Do not plot all
  int idx_incr = 0;
  if (0 == (idx_incr = series->getNumRows()/100))
  {
    idx_incr = 1;
  }

  // Plot time-series:
  for (size_t j=0; j<series->getNumRows(); j+=idx_incr)
  {
    size_t N = graphs.size();
    size_t offset = N+1; size_t increment = N;
    for (size_t i=0; i<num_species; i++)
    {
      graphs[i]->addPoint((*series)(j, 0), (*series)(j, 1+i+IOSoffset), std::sqrt((*series)(j, offset)));
      offset += increment; increment -= 1;
    }
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  for (size_t i=0; i<num_species; i++)
  {
    graphs[i]->commit();
  }

  this->updateAxes();
}


ParameterScanCovPlot::ParameterScanCovPlot(size_t num_species, Table *series,
                                     const QString &species_unit, const QString &time_unit,
                                     QObject *parent)
    : Plot::Figure("Coefficient of variation (EMRE & IOS var)", parent)
{
  // Create a plot:
  this->setXLabel(tr("%1").arg(series->getColumnName(0)));
  this->setYLabel(tr("concentrations [%1]").arg(species_unit));

  QVector<Plot::LineGraph *> graphs(num_species);

  size_t IOSoffset = 0;

  // Allocate a graph for each colum in time-series:
  for (size_t i=0; i<num_species; i++)
  {
    Plot::GraphStyle style = this->getStyle(i);
    graphs[i] = new Plot::LineGraph(style);
    this->axis->addGraph(graphs[i]);
    this->addToLegend(series->getColumnName(i+1+IOSoffset), graphs[i]);
  }

  // Do not plot all
  int idx_incr = 0;
  if (0 == (idx_incr = series->getNumRows()/100))
  {
    idx_incr = 1;
  }

  // Plot time-series:
  for (size_t j=0; j<series->getNumRows(); j+=idx_incr)
  {
    size_t N = graphs.size();
    size_t offset = N+1; size_t increment = N;
    for (size_t i=0; i<num_species; i++)
    {
      graphs[i]->addPoint((*series)(j, 0), std::sqrt((*series)(j, offset)/(*series)(j, 1+i+IOSoffset)));
      offset += increment; increment -= 1;
    }
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  for (size_t i=0; i<num_species; i++)
  {
    graphs[i]->commit();
  }

  this->updateAxes();
}


ParameterScanCovIOSPlot::ParameterScanCovIOSPlot(size_t num_species, Table *series,
                                     const QString &species_unit, const QString &time_unit,
                                     QObject *parent)
    : Plot::Figure("Coeff of variation (EMRE & IOS var)", parent)
{

  // Create a plot:
  this->setXLabel(tr("%1").arg(series->getColumnName(0)));
  this->setYLabel(tr("concentrations [%1]").arg(species_unit));

  QVector<Plot::LineGraph *> graphs(num_species);

  size_t IOSoffset = num_species+num_species*(num_species+1)/2;

  // Allocate a graph for each colum in time-series:
  for (size_t i=0; i<num_species; i++)
  {
    Plot::GraphStyle style = this->getStyle(i);
    graphs[i] = new Plot::LineGraph(style);
    this->axis->addGraph(graphs[i]);
    this->addToLegend(series->getColumnName(i+1+IOSoffset), graphs[i]);
  }

  // Do not plot all
  int idx_incr = 0;
  if (0 == (idx_incr = series->getNumRows()/100))
  {
    idx_incr = 1;
  }

  // Plot time-series:
  for (size_t j=0; j<series->getNumRows(); j+=idx_incr)
  {
    size_t N = graphs.size();
    size_t offset = N+1; size_t increment = N;
    for (size_t i=0; i<num_species; i++)
    {
      graphs[i]->addPoint((*series)(j, 0), std::sqrt((*series)(j, offset)/(*series)(j, 1+i+IOSoffset)));
      offset += increment; increment -= 1;
    }
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  for (size_t i=0; i<num_species; i++)
  {
    graphs[i]->commit();
  }

  this->updateAxes();

}
