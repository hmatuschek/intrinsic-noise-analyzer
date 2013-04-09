#include "ssaplot.hh"
#include "ssatask.hh"
#include "../plot/graph.hh"
#include "../plot/configuration.hh"



Plot::PlotConfig *
createSSAPlotConfig(const QStringList &selected_species, SSATask *task)
{
  Table &series = task->getTimeSeries();
  Plot::PlotConfig *config = new Plot::PlotConfig(series);

  size_t Ntot = task->getModel()->numSpecies();
  size_t Nsel = selected_species.size();

  // Serialize unit of species:
  std::stringstream unit_str;
  task->getModel()->getConcentrationUnit().dump(unit_str,true);
  QString concentration_unit = unit_str.str().c_str(); unit_str.str("");
  task->getModel()->getTimeUnit().dump(unit_str,true);
  QString time_unit  = unit_str.str().c_str();

  // Set axis labels:
  config->setTile(QObject::tr("Stochastic Simulation Algorithm"));
  config->setXLabel(QObject::tr("time [%1]").arg(time_unit));
  config->setYLabel(QObject::tr("concentrations [%1]").arg(concentration_unit));

  size_t offset = 1;
  for (size_t i=0; i<Nsel; i++) {
    size_t species_idx = task->getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;
    Plot::VarianceLineGraphConfig *var_config = new Plot::VarianceLineGraphConfig(config->data(), i);
    var_config->setLabel(task->getTimeSeries().getColumnName(mean_idx));
    var_config->setXExpression("$0");
    var_config->setYExpression(QString("$%1").arg(mean_idx));
    var_config->setVarExpression(QString("$%1").arg(var_idx));
    config->addGraph(var_config);
  }

  // Force y plot-range to be [0, AUTO]:
  config->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  config->setYRange(Plot::Range(0, 1));

  config->setXRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  config->setXRange(Plot::Range(0, 1));

  return config;
}


Plot::PlotConfig *
createSSACorrelationPlotConfig(const QStringList &selected_species, SSATask *task)
{
  Table &series = task->getTimeSeries();
  Plot::PlotConfig *config = new Plot::PlotConfig(series);

  // Serialize unit of species:
  std::stringstream unit_str;
  task->getModel()->getTimeUnit().dump(unit_str,true);
  QString time_unit  = unit_str.str().c_str();

  config->setTile("Correlation Coefficients (SSA)");
  config->setXLabel(QObject::tr("time [%1]").arg(time_unit));
  config->setYLabel(QObject::tr("correlation coefficient"));

  size_t Ntot = task->getModel()->numSpecies();
  size_t Nsel = selected_species.size();

  // Allocate a graph for each colum in time-series:
  size_t offset = 1, graph_idx=0;
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

    for (size_t j=i+1; j<Nsel; j++, graph_idx++) {
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

      Plot::LineGraphConfig *cor_config = new Plot::LineGraphConfig(config->data(), graph_idx);
      cor_config->setLabel(QObject::tr("corr(%1, %2)").arg(species_name_i).arg(species_name_j));
      cor_config->setXExpression("$0");
      cor_config->setYExpression(
            QString("$%1/sqrt($%2*$%3)").arg(cov_index_ij).arg(var_idx_i).arg(var_idx_j));
      config->addGraph(cor_config);
    }
  }

  // Force y plot-range to be [0, AUTO]:
  config->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::FIXED));
  config->setYRange(Plot::Range(-1.1, 1.1));

  return config;
}
