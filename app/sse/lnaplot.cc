#include "lnaplot.hh"
#include "lnatask.hh"
#include "../plot/configuration.hh"
#include "../models/timeseries.hh"
#include <QVector>


Plot::PlotConfig *
createLNATimeSeriesPlotConfig(QStringList &selected_species, LNATask *task)
{
  Table *series = task->getTimeSeries();
  Plot::PlotConfig *config = new Plot::PlotConfig(*series);

  // Get species unit
  std::stringstream unit_str;
  task->getSpeciesUnit().dump(unit_str, true);
  QString species_unit(unit_str.str().c_str());
  // Get time unit
  unit_str.str("");
  task->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());

  // Set axis labels with units:
  config->setTile(QObject::tr("Mean concentrations (RE & LNA)"));
  config->setXLabel(QObject::tr("time [%1]").arg(time_unit));
  if (task->getSpeciesUnit().isConcentrationUnit()) {
    config->setYLabel(QObject::tr("concentrations [%1]").arg(species_unit));
  } else {
    config->setYLabel(QObject::tr("amount [%1]").arg(species_unit));
  }

  iNA::Ast::Model *model = task->getConfig().getModel();
  size_t Ntot = model->numSpecies();
  size_t Nsel = selected_species.size();

  // Create a graph for each colum in time-series:
  for (size_t i=0; i<Nsel; i++) {
    size_t species_idx = model->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = 1+species_idx;
    size_t var_idx  = 1+Ntot; // offset, first cov column
    var_idx  += species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;
    Plot::VarianceLineGraphConfig *var_config = new Plot::VarianceLineGraphConfig(series, i);
    var_config->setLabel(series->getColumnName(mean_idx));
    var_config->setXExpression("$0");
    var_config->setYExpression(QString("$%1").arg(mean_idx));
    var_config->setVarExpression(QString("$%1").arg(var_idx));
    config->addGraph(var_config);
  }

  // Force y plot-range to be [0, AUTO]:
  config->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  config->setYRange(Plot::Range(0, 1));

  return config;
}


Plot::PlotConfig *
createLNACorrelationPlotConfig(QStringList &selected_species, LNATask *task)
{
  Table *data = task->getTimeSeries();
  Plot::PlotConfig *config = new Plot::PlotConfig(*data);

  // Get time unit
  std::stringstream unit_str;
  task->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());
  config->setTile(QObject::tr("Correlation Coefficients (LNA)"));
  config->setXLabel(QObject::tr("time [%1]").arg(time_unit));
  config->setYLabel(QObject::tr("correlation coefficient"));

  iNA::Ast::Model *model= task->getConfig().getModel();
  size_t Nsel = selected_species.size();
  size_t Ntot = model->numSpecies();

  // Allocate a graph for each colum in time-series:
  size_t offset=1+Ntot, graph_idx = 0;
  for (size_t i=0; i<Nsel; i++) {
    iNA::Ast::Species *species_i = model->getSpecies(selected_species.at(i).toStdString());
    size_t species_idx_i = model->getSpeciesIdx(species_i);
    QString species_name_i = data->getColumnName(1+species_idx_i);
    size_t var_idx_i = offset + species_idx_i*(Ntot+1) - (species_idx_i*(species_idx_i+1))/2;

    for (size_t j=i+1; j<Nsel; j++, graph_idx++) {
      iNA::Ast::Species *species_j = model->getSpecies(selected_species.at(j).toStdString());
      size_t species_idx_j = model->getSpeciesIdx(species_j);
      QString species_name_j = data->getColumnName(1+species_idx_j);
      size_t var_idx_j = offset + species_idx_j*(Ntot+1) - (species_idx_j*(species_idx_j+1))/2;
      size_t cov_idx_ij = var_idx_i + (std::max(species_idx_i, species_idx_j) - std::min(species_idx_i, species_idx_j));

      Plot::LineGraphConfig *corr_config = new Plot::LineGraphConfig(data, graph_idx);
      corr_config->setLabel(QObject::tr("corr(%1, %2)").arg(species_name_i).arg(species_name_j));
      corr_config->setXExpression("$0");
      corr_config->setYExpression(
            QString("$%1/sqrt($%2*$%3)").arg(cov_idx_ij).arg(var_idx_i).arg(var_idx_j));
      config->addGraph(corr_config);
    }
  }

  // Force y plot-range to be [-1, 1]:
  config->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::FIXED));
  config->setYRange(Plot::Range(-1.1, 1.1));

  return config;
}
