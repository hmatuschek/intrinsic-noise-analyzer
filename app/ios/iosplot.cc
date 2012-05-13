#include "iosplot.hh"
#include "../plot/plot.hh"
#include "../plot/linegraph.hh"
#include <QVector>


IOSLNATimeSeriesPlot::IOSLNATimeSeriesPlot(size_t num_species, Table *series,
                                     const QString &species_unit, const QString &time_unit,
                                     QObject *parent)
  : VariancePlot("Mean concentrations (RE & LNA)", parent)
{
  // Create a plot:
  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("concentrations [%1]").arg(species_unit));

  size_t off_mean = 1;
  size_t off_cov  = 1+num_species;
  for (size_t i=0; i<num_species; i++) {
    this->addVarianceGraph(series->getColumn(0), series->getColumn(i+off_mean),
                           series->getColumn(off_cov), series->getColumnName(i+off_mean));
    off_cov += num_species-i;
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);
  this->updateAxes();
}




IOSEMRETimeSeriesPlot::IOSEMRETimeSeriesPlot(size_t num_species, Table *series,
                                             const QString &species_unit, const QString &time_unit,
                                             QObject *parent)
  : VariancePlot("Mean concentrations (EMRE & IOS)", parent)
{
  // Create a plot:
  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("concentrations [%1]").arg(species_unit));

  size_t offset_mean = 1+num_species+(num_species*(num_species+1))/2;
  size_t offset_cov  = offset_mean+num_species;
  for (size_t i=0; i<num_species; i++)
  {
    this->addVarianceGraph(series->getColumn(0), series->getColumn(i+offset_mean),
                           series->getColumn(offset_cov), series->getColumnName(i+offset_mean));
    offset_cov += num_species-i;
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);
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



/* ********************************************************************************************* *
 * Implementation of correlation plot.
 * ********************************************************************************************* */
IOSLNACorrelationPlot::IOSLNACorrelationPlot(size_t num_species, Table *data,
                                             const QString &time_unit,
                                             QObject *parent)
  : Figure("Correlation Coefficients (LNA)", parent)
{
  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("correlation coefficient"));

  size_t N_cov = (num_species*(num_species-1))/2;
  QVector<Plot::LineGraph *> graphs(N_cov);

  // Allocate a graph for each colum in time-series:
  size_t graph_idx = 0;
  size_t column_idx = 1+num_species;
  Eigen::MatrixXi index_table(num_species, num_species);
  for (size_t i=0; i<num_species; i++)
  {
    index_table(i,i) = column_idx; column_idx++;
    for (size_t j=i+1; j<num_species; j++, graph_idx++, column_idx++)
    {
      Plot::GraphStyle style = this->getStyle(graph_idx);
      graphs[graph_idx] = new Plot::LineGraph(style);
      this->axis->addGraph(graphs[graph_idx]);
      this->addToLegend(
            QString("corr(%1, %2)").arg(data->getColumnName(1+i)).arg(data->getColumnName(1+j)),
            graphs[graph_idx]);
      index_table(i,j) = index_table(j,i) = column_idx;
    }
  }

  // Do not plot all
  int idx_incr ;
  if (0 == (idx_incr = data->getNumRows()/100))
  {
    idx_incr = 1;
  }

  // Plot time-series:
  for (size_t k=0; k<data->getNumRows(); k+=idx_incr)
  {
    size_t idx = 0;
    for (size_t i=0; i<num_species; i++)
    {
      for (size_t j=i+1; j<num_species; j++, idx++)
      {
        double x = (*data)(k, 0);
        double y = 0.0;

        if ((0.0 != (*data)(k, index_table(i,i))) && (0.0 != (*data)(k, index_table(j,j))))
        {
          y = (*data)(k, index_table(i,j)) /
              (std::sqrt((*data)(k, index_table(i,i))) *
               std::sqrt((*data)(k, index_table(j,j))));
        }

        graphs[idx]->addPoint(x, y);
      }
    }
  }

  // Force y plot-range to be [-1, 1]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::FIXED));
  this->getAxis()->setYRange(-1.1, 1.1);

  // Finally commit changes:
  for (size_t i=0; i<N_cov; i++)
  {
    graphs[i]->commit();
  }

  this->updateAxes();
}



/* ********************************************************************************************* *
 * Implementation of correlation plot.
 * ********************************************************************************************* */
IOSEMRECorrelationPlot::IOSEMRECorrelationPlot(size_t num_species, Table *data,
                                               const QString &time_unit,
                                               QObject *parent)
  : Figure("Correlation Coefficients (EMRE+IOS)", parent)
{
  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("correlation coefficient"));

  size_t N_cov = (num_species*(num_species-1))/2;
  QVector<Plot::LineGraph *> graphs(N_cov);

  // Allocate a graph for each colum in time-series:
  size_t graph_idx = 0;
  size_t column_idx = 1+2*num_species+(num_species*(num_species+1))/2;
  Eigen::MatrixXi index_table(num_species, num_species);
  for (size_t i=0; i<num_species; i++)
  {
    index_table(i,i) = column_idx; column_idx++;
    for (size_t j=i+1; j<num_species; j++, graph_idx++, column_idx++)
    {
      Plot::GraphStyle style = this->getStyle(graph_idx);
      graphs[graph_idx] = new Plot::LineGraph(style);
      this->axis->addGraph(graphs[graph_idx]);
      this->addToLegend(
            QString("corr(%1, %2)").arg(data->getColumnName(1+i)).arg(data->getColumnName(1+j)),
            graphs[graph_idx]);
      index_table(i,j) = index_table(j,i) = column_idx;
    }
  }

  // Do not plot all
  int idx_incr ;
  if (0 == (idx_incr = data->getNumRows()/100))
  {
    idx_incr = 1;
  }

  // Plot time-series:
  for (size_t k=0; k<data->getNumRows(); k+=idx_incr)
  {
    size_t idx = 0;
    for (size_t i=0; i<num_species; i++)
    {
      for (size_t j=i+1; j<num_species; j++, idx++)
      {
        double x = (*data)(k, 0);
        double y = 0.0;

        if ((0.0 != (*data)(k, index_table(i,i))) && (0.0 != (*data)(k, index_table(j,j))))
        {
          y = (*data)(k, index_table(i,j)) /
              (std::sqrt((*data)(k, index_table(i,i))) *
               std::sqrt((*data)(k, index_table(j,j))));
        }

        graphs[idx]->addPoint(x, y);
      }
    }
  }

  // Force y plot-range to be [-1, 1]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::FIXED));
  this->getAxis()->setYRange(-1.1, 1.1);

  // Finally commit changes:
  for (size_t i=0; i<N_cov; i++)
  {
    graphs[i]->commit();
  }

  this->updateAxes();
}
