#include "ssaplot.hh"
#include "../plot/graph.hh"
#include "../plot/variancelinegraph.hh"



/* ******************************************************************************************** *
 * SSA Timeseries Plot
 * ******************************************************************************************** */
SSAPlot::SSAPlot(SSATask *task, QObject *parent)
  : Plot::Figure("Stochastic Simulation Algorithm", parent)
{
  size_t N = task->numSpecies();
  QVector<Plot::VarianceLineGraph *> graphs(N);

  // Serialize unit of species:
  std::stringstream unit_str;
  task->getModel()->getConcentrationUnit().dump(unit_str,true);
  QString concentration_unit = unit_str.str().c_str();

  unit_str.str("");
  task->getModel()->getTimeUnit().dump(unit_str,true);
  QString time_unit  = unit_str.str().c_str();

  // Set axis labels:
  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("concentrations [%1]").arg(concentration_unit));

  for (size_t i=0; i<N; i++)
  {
    Plot::GraphStyle style = this->getStyle(i);
    graphs[i] = new Plot::VarianceLineGraph(style);
    this->getAxis()->addGraph(graphs[i]);
    this->addToLegend(task->getTimeSeries().getColumnName(i+1), graphs[i]);
  }

  // Plot time-series:
  for (size_t j=0; j<task->getTimeSeries().getNumRows(); j++)
  {
    size_t offset = N+1; size_t increment = N;
    for (size_t i=0; i<N; i++)
    {
      graphs[i]->addPoint(task->getTimeSeries().matrix()(j, 0),
                          task->getTimeSeries().matrix()(j, 1+i),
                          std::sqrt(std::abs(task->getTimeSeries().matrix()(j, offset))));
      offset += increment; increment -= 1;
    }
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->getAxis()->setXRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setXRange(0, 1);

  for (size_t i=0; i<N; i++)
  {
    graphs[i]->commit();
  }
}



/* ******************************************************************************************** *
 * SSA Correlation Plot
 * ******************************************************************************************** */
SSACorrelationPlot::SSACorrelationPlot(SSATask *task, QObject *parent)
  : Plot::Figure("Correlation Coefficients (SSA)", parent)
{
  // Serialize unit of species:
  std::stringstream unit_str;
  task->getModel()->getTimeUnit().dump(unit_str,true);
  QString time_unit  = unit_str.str().c_str();

  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("correlation coefficient"));

  Table &data = task->getTimeSeries();
  size_t num_species = task->numSpecies();
  size_t N_cov = num_species*(num_species-1)/2;
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
            QString("corr(%1, %2)").arg(data.getColumnName(1+i)).arg(data.getColumnName(1+j)),
            graphs[graph_idx]);
      index_table(i,j) = index_table(j,i) = column_idx;
    }
  }

  // Plot time-series:
  for (size_t k=0; k<data.getNumRows(); k++)
  {
    size_t idx = 0;
    for (size_t i=0; i<num_species; i++)
    {
      for (size_t j=i+1; j<num_species; j++, idx++)
      {
        double x = data(k, 0);
        double y = data(k, index_table(i,j)) /
            (std::sqrt(data(k, index_table(i,i))) *
             std::sqrt(data(k, index_table(j,j))));
        graphs[idx]->addPoint(x, y);
      }
    }
  }

  // Force y plot-range to be [-1, 1]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::FIXED));
  this->getAxis()->setYRange(-1.1, 1.1);

  this->getAxis()->setXRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setXRange(0, 1);

  // Finally commit changes:
  for (size_t i=0; i<N_cov; i++)
  {
    graphs[i]->commit();
  }

  this->updateAxes();
}
