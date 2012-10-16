#include "ssaplot.hh"
#include "../plot/graph.hh"
#include "../plot/variancelinegraph.hh"



/* ******************************************************************************************** *
 * SSA Timeseries Plot
 * ******************************************************************************************** */
SSAPlot::SSAPlot(const QStringList &selected_species, SSATask *task, QObject *parent)
  : LinePlot("Stochastic Simulation Algorithm", parent)
{
  size_t Ntot = task->getModel()->numSpecies();
  size_t Nsel = selected_species.size();

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

  size_t offset = 1;
  for (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = task->getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;
    addVarianceGraph(task->getTimeSeries().getColumn(0),
                     task->getTimeSeries().getColumn(mean_idx),
                     task->getTimeSeries().getColumn(var_idx),
                     task->getTimeSeries().getColumnName(mean_idx));
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->getAxis()->setXRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setXRange(0, 1);

  updateAxes();
}



/* ******************************************************************************************** *
 * SSA Correlation Plot
 * ******************************************************************************************** */
SSACorrelationPlot::SSACorrelationPlot(const QStringList &selected_species, SSATask *task, QObject *parent)
  : LinePlot("Correlation Coefficients (SSA)", parent)
{
  // Serialize unit of species:
  std::stringstream unit_str;
  task->getModel()->getTimeUnit().dump(unit_str,true);
  QString time_unit  = unit_str.str().c_str();

  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("correlation coefficient"));

  size_t Ntot = task->numSpecies();
  size_t Nsel = selected_species.size();

  // Allocate a graph for each colum in time-series:
  size_t offset = 1;
  for (size_t i=0; i<Nsel; i++) {
    // Get species index for i-th selected species
    size_t species_idx_i = task->getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    // Get its variance columne
    size_t var_idx_i = offset+Ntot+species_idx_i*(Ntot+1)-(species_idx_i*(species_idx_i+1))/2;
    // Get its name
    QString species_name_i = task->getModel()->getSpecies(species_idx_i)->getIdentifier().c_str();
    if (task->getModel()->getSpecies(species_idx_i)->hasName()) {
      species_name_i = task->getModel()->getSpecies(species_idx_i)->getName().c_str();
    }

    for (size_t j=i+1; j<Nsel; j++) {
      // Get species index for the j-th selected species
      size_t species_idx_j = task->getModel()->getSpeciesIdx(selected_species.at(j).toStdString());
      // Its variance index
      size_t var_idx_j = offset+Ntot+species_idx_j*(Ntot+1)-(species_idx_j*(species_idx_j+1))/2;
      // Its name
      QString species_name_j = task->getModel()->getSpecies(species_idx_j)->getIdentifier().c_str();
      if (task->getModel()->getSpecies(species_idx_j)->hasName()) {
        species_name_j = task->getModel()->getSpecies(species_idx_j)->getName().c_str();
      }
      // The index for the covariance of i & j:
      size_t cov_index_ij = std::min(var_idx_i, var_idx_j) +
          (std::max(species_idx_i, species_idx_j) - std::min(species_idx_i, species_idx_j));
      // Calc correlation coefficients
      Eigen::VectorXd var_i = task->getTimeSeries().getColumn(var_idx_i);
      Eigen::VectorXd var_j = task->getTimeSeries().getColumn(var_idx_j);
      Eigen::VectorXd cov_ij = task->getTimeSeries().getColumn(cov_index_ij);
      Eigen::VectorXd corr = cov_ij.array()/(var_i.array()*var_j.array()).sqrt();
      // Add correlation function plot
      addLineGraph(task->getTimeSeries().getColumn(0), corr,
                   QString("corr(%1, %2)").arg(species_name_i).arg(species_name_j));
    }
  }

  updateAxes();
}
