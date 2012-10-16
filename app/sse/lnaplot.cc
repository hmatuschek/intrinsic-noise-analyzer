#include "lnaplot.hh"
#include "../plot/plot.hh"
#include "../plot/linegraph.hh"
#include <QVector>


LNATimeSeriesPlot::LNATimeSeriesPlot(QList<QString> &selected_species, LNATask *task, QObject *parent)
  : LinePlot("Mean concentrations (RE & LNA)", parent)
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

  iNA::Ast::Model *model = task->getConfig().getModel();
  Table *series = task->getTimeSeries();
  size_t Ntot = model->numSpecies();
  size_t Nsel = selected_species.size();

  // Create a graph for each colum in time-series:
  for (size_t i=0; i<Nsel; i++) {
    size_t species_idx = model->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = 1+species_idx;
    size_t var_idx  = 1+Ntot; // offset, first cov column
    var_idx  += species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;
    addVarianceGraph(series->getColumn(0), series->getColumn(mean_idx), series->getColumn(var_idx),
                     series->getColumnName(mean_idx));
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}



/* ********************************************************************************************* *
 * Implementation of correlation plot.
 * ********************************************************************************************* */
LNACorrelationPlot::LNACorrelationPlot(QList<QString> &selected_species, LNATask *task, QObject *parent)
  : LinePlot("Correlation Coefficients (LNA)", parent)
{
  // Get time unit
  std::stringstream unit_str;
  task->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());
  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("correlation coefficient"));

  iNA::Ast::Model *model= task->getConfig().getModel();
  Table *data = task->getTimeSeries();
  size_t Nsel = selected_species.size();
  size_t Ntot = model->numSpecies();
  size_t N_cov = (Nsel*(Nsel-1))/2;
  QVector<Plot::LineGraph *> graphs(N_cov);

  // Allocate a graph for each colum in time-series:
  size_t offset=1+Ntot;
  for (size_t i=0; i<Nsel; i++) {
    iNA::Ast::Species *species_i = model->getSpecies(selected_species.at(i).toStdString());
    size_t species_idx_i = model->getSpeciesIdx(species_i);
    QString species_name_i = data->getColumnName(1+species_idx_i);
    size_t var_idx_i = offset + species_idx_i*(Ntot+1) - (species_idx_i*(species_idx_i+1))/2;
    Eigen::VectorXd var_i = data->getColumn(var_idx_i);

    for (size_t j=i+1; j<Nsel; j++)
    {
      iNA::Ast::Species *species_j = model->getSpecies(selected_species.at(i).toStdString());
      size_t species_idx_j = model->getSpeciesIdx(species_j);
      QString species_name_j = data->getColumnName(1+species_idx_j);
      size_t var_idx_j = offset + species_idx_j*(Ntot+1) - (species_idx_j*(species_idx_j+1))/2;
      size_t cov_idx_ij = var_idx_i + (std::max(species_idx_i, species_idx_j) - std::min(species_idx_i, species_idx_j));
      Eigen::VectorXd var_j = data->getColumn(var_idx_j);
      Eigen::VectorXd cov_ij = data->getColumn(cov_idx_ij);
      Eigen::VectorXd corr = cov_ij.array() / (var_i.array()*var_j.array()).sqrt();
      addLineGraph(data->getColumn(0), corr,
                   QString("corr(%1, %2)").arg(species_name_i).arg(species_name_j));
    }
  }

  // Force y plot-range to be [-1, 1]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::FIXED));
  this->getAxis()->setYRange(-1.1, 1.1);


  this->updateAxes();
}
