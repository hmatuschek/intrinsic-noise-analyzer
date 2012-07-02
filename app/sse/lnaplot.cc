#include "lnaplot.hh"
#include "../plot/plot.hh"
#include "../plot/linegraph.hh"
#include <QVector>


LNATimeSeriesPlot::LNATimeSeriesPlot(size_t num_species, Table *series,
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




/* ********************************************************************************************* *
 * Implementation of EMRE time-series plot.
 * ********************************************************************************************* */
EMRETimeSeriesPlot::EMRETimeSeriesPlot(size_t num_species, Table *series,
                                       const QString &species_unit, const QString &time_unit,
                                       QObject *parent)
  : Plot::Figure("Mean concentrations (EMRE & RE)", parent)
{
  // Create a plot:
  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("concentrations [%1]").arg(species_unit));

  QVector<Plot::LineGraph *> lna_graphs(num_species);
  QVector<Plot::LineGraph *> emre_graphs(num_species);

  size_t lna_offset  = 1;
  size_t emre_offset = 1+num_species+(num_species*(num_species+1))/2;

  // Allocate a graph for each colum in time-series:
  for (size_t i=0; i<num_species; i++)
  {
    Plot::GraphStyle style = this->getStyle(i);
    emre_graphs[i] = new Plot::LineGraph(style);

    QColor lcolor = style.getLineColor();
    lcolor.setAlpha(128); style.setLineColor(lcolor);
    lna_graphs[i] = new Plot::LineGraph(style);

    this->axis->addGraph(lna_graphs[i]);
    this->axis->addGraph(emre_graphs[i]);

    this->addToLegend(series->getColumnName(lna_offset+i), lna_graphs[i]);
    this->addToLegend(series->getColumnName(emre_offset+i), emre_graphs[i]);
  }

  // Do not plot all
  int idx_incr ;
  if (0 == (idx_incr = series->getNumRows()/100))
  {
    idx_incr = 1;
  }

  // Plot time-series:
  Eigen::VectorXd  lna_vals(2);
  for (size_t j=0; j<series->getNumRows(); j+=idx_incr)
  {
    size_t N = lna_graphs.size();
    size_t offset = N+1; size_t increment = N;
    for (size_t i=0; i<num_species; i++)
    {
      lna_vals(0) = (*series)(j, 0);
      lna_vals(1) = (*series)(j, 1+i);
      offset += increment; increment -= 1;
      lna_graphs[i]->addPoint(lna_vals);
    }
  }

  // Plot time-series:
  Eigen::VectorXd  emre_vals(2);
  for (size_t j=0; j<series->getNumRows(); j+=idx_incr)
  {
    for (size_t i=0; i<num_species; i++)
    {
      emre_vals(0) = (*series)(j, 0);
      emre_vals(1) = (*series)(j, emre_offset+i);
      emre_graphs[i]->addPoint(emre_vals);
    }
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  for (size_t i=0; i<num_species; i++)
  {
    lna_graphs[i]->commit();
    emre_graphs[i]->commit();
  }

  this->updateAxes();
}



/* ********************************************************************************************* *
 * Implementation of correlation plot.
 * ********************************************************************************************* */
LNACorrelationPlot::LNACorrelationPlot(LNATask *task,
                                       const QString &time_unit,
                                       QObject *parent)
  : Figure("Correlation Coefficients (LNA)", parent)
{
  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("correlation coefficient"));

  size_t num_species = task->getSelectedSpecies().size();
  size_t N_cov = (num_species*(num_species-1))/2;
  Table *data = task->getTimeSeries();
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
            QString("corr(%1, %2)").arg(task->getSpeciesName(i)).arg(task->getSpeciesName(j)),
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
 * Implementation of vadility plot:
 * ********************************************************************************************* */
LNAVadilityPlot::LNAVadilityPlot(size_t num_species, Table *data, QObject *parent)
  : Figure("Vadility Plot", parent)
{
  this->setXLabel("time");
  this->setYLabel("probability of vadility");
  this->getAxis()->showXZero(true);

  size_t probIdx = 1+2*num_species+num_species*(num_species+1)/2;

  // Allocate graph:
  Plot::GraphStyle style = this->getStyle(0);
  Plot::LineGraph *graph = new Plot::LineGraph(style);
  this->axis->addGraph(graph);

  // Plot time-series:
  for (size_t k=0; k<data->getNumRows(); k++)
  {
    double x = (*data)(k, 0);
    double y = (*data)(k, probIdx);
    graph->addPoint(x, y);
  }

  // Force y plot-range to be [0, 1]
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::FIXED));
  this->getAxis()->setYRange(0, 1);

  // Commit changes:
  graph->commit();

  this->updateAxes();
}
