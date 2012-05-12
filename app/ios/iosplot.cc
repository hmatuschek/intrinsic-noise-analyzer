#include "iosplot.hh"
#include "../plot/plot.hh"
#include "../plot/linegraph.hh"
#include <QVector>


IOSLNATimeSeriesPlot::IOSLNATimeSeriesPlot(size_t num_species, Table *series,
                                     const QString &species_unit, const QString &time_unit,
                                     QObject *parent)
  : Plot::Figure("Mean concentrations (RE & LNA)", parent)
{
  // Create a plot:
  this->setXLabel(tr("time [%1]").arg(time_unit));
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




IOSEMRETimeSeriesPlot::IOSEMRETimeSeriesPlot(size_t num_species, Table *series,
                                             const QString &species_unit, const QString &time_unit,
                                             QObject *parent)
  : Plot::Figure("Mean concentrations (EMRE & IOS)", parent)
{
  // Create a plot:
  this->setXLabel(tr("time [%1]").arg(time_unit));
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
    size_t offset_mean = 1+N+(N*(N+1))/2;
    size_t offset_cov  = offset_mean+N; size_t increment = N;
    for (size_t i=0; i<num_species; i++)
    {
      graphs[i]->addPoint((*series)(j, 0), (*series)(j, 1+i), sqrt((*series)(j, offset_cov)));
      offset_cov += increment; increment -= 1;
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



IOSEMREComparePlot::IOSEMREComparePlot(size_t num_species, Table *series,
                                       const QString &species_unit, const QString &time_unit,
                                       QObject *parent)
  : Plot::Figure("Mean concentrations (EMRE & IOS-EMRE)", parent)
{
  // Create a plot:
  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("concentrations [%1]").arg(species_unit));

  QVector<Plot::LineGraph *> emre_graphs(num_species);
  QVector<Plot::LineGraph *> ios_graphs(num_species);

  size_t off_emre = 1 + num_species + (num_species*(num_species+1))/2;
  size_t off_ios  = off_emre + num_species + (num_species*(num_species+1))/2;

  // Allocate a graph for each colum in time-series:
  for (size_t i=0; i<num_species; i++)
  {
    Plot::GraphStyle style = this->getStyle(i);
    ios_graphs[i]  = new Plot::LineGraph(style);

    QColor line_color = style.getLineColor(); line_color.setAlpha(128);
    style.setLineColor(line_color);
    emre_graphs[i] = new Plot::LineGraph(style);

    this->axis->addGraph(emre_graphs[i]);
    this->axis->addGraph(ios_graphs[i]);
    this->addToLegend(series->getColumnName(i+off_emre), emre_graphs[i]);
    this->addToLegend(series->getColumnName(i+off_ios),  ios_graphs[i]);
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
    for (size_t i=0; i<num_species; i++)
    {
      emre_graphs[i]->addPoint((*series)(j, 0), (*series)(j, i+off_emre));
      ios_graphs[i]->addPoint((*series)(j, 0), (*series)(j, i+off_ios));
    }
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  for (size_t i=0; i<num_species; i++)
  {
    emre_graphs[i]->commit();
    ios_graphs[i]->commit();
  }

  this->updateAxes();
}
