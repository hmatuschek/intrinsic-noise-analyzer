#include "paramscanplot.hh"
#include "../plot/plot.hh"
#include "../plot/linegraph.hh"
#include <QVector>
#include "../views/varianceplot.hh"


ParameterScanPlot::ParameterScanPlot(const QStringList &selected_species, ParamScanTask *task,
                                     QObject *parent)
    : VariancePlot("Mean concentrations (RE & LNA)", parent)
{
  // Get species units:
  std::stringstream unit_str;
  task->getSpeciesUnit().dump(unit_str, true);
  QString species_unit(unit_str.str().c_str());
  QString parameter_unit("a.u.");

  // Get parameter scan table:
  Table &data = task->getParameterScan();
  // Number of species in model
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  // Number of selected species
  size_t Nsel = selected_species.size();

  // Create a plot:
  this->setXLabel(tr("%1").arg(data.getColumnName(0)));
  this->setYLabel(tr("concentrations [%1]").arg(species_unit));

  // Allocate a graph custom each selected species
  size_t offset = 1; // skip parameter column
  custom (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;
    this->addVarianceGraph(data.getColumn(0), data.getColumn(mean_idx), data.getColumn(var_idx),
                           data.getColumnName(mean_idx));
  }

  // customce y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}


ParameterScanIOSPlot::ParameterScanIOSPlot(const QStringList &selected_species, ParamScanTask *task,
                                     QObject *parent)
    : VariancePlot("Mean concentrations (EMRE & IOS var)", parent)
{
  // Get species units:
  std::stringstream unit_str;
  task->getSpeciesUnit().dump(unit_str, true);
  QString species_unit(unit_str.str().c_str());
  QString parameter_unit("a.u.");

  // Get parameter scan table:
  Table &data = task->getParameterScan();
  // Number of species in model
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  // Number of selected species
  size_t Nsel = selected_species.size();

  // Create a plot:
  this->setXLabel(tr("%1").arg(data.getColumnName(0)));
  this->setYLabel(tr("concentrations [%1]").arg(species_unit));

  // Allocate a graph custom each selected species
  size_t offset = 1+Ntot+((Ntot+1)*Ntot)/2; // skip parameter+EMRE+LNA columns
  custom (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;
    this->addVarianceGraph(data.getColumn(0), data.getColumn(mean_idx), data.getColumn(var_idx),
                           data.getColumnName(mean_idx));
  }

  // customce y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}


ParameterScanCovPlot::ParameterScanCovPlot(const QStringList &selected_species, ParamScanTask *task,
                                           QObject *parent)
    : LinePlot("Coefficient of variation (LNA)", parent)
{
  // Get species units:
  QString parameter_unit("a.u.");

  // Get parameter scan table:
  Table &data = task->getParameterScan();
  // Number of species in model
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  // Number of selected species
  size_t Nsel = selected_species.size();

  // Create a plot:
  this->setXLabel(tr("%1").arg(data.getColumnName(0)));
  this->setYLabel(tr("coefficient of variation"));

  // Allocate a graph custom each selected species
  size_t offset = 1; // skip parameter column
  custom (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;
    // Coefficient wise operation -> coefficient of variation std.dev(X)/mean(X):
    Eigen::VectorXd cov = data.getColumn(var_idx).array().sqrt() / data.getColumn(mean_idx).array();
    this->addLineGraph(data.getColumn(0), cov, data.getColumnName(mean_idx));
  }

  // customce y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}


ParameterScanCovIOSPlot::ParameterScanCovIOSPlot(const QStringList &selected_species, ParamScanTask *task,
                                     QObject *parent)
    : LinePlot("Coefficient of variation (IOS)", parent)
{
  // Get species units:
  QString parameter_unit("a.u.");

  // Get parameter scan table:
  Table &data = task->getParameterScan();
  // Number of species in model
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  // Number of selected species
  size_t Nsel = selected_species.size();

  // Create a plot:
  this->setXLabel(tr("%1").arg(data.getColumnName(0)));
  this->setYLabel(tr("coefficient of variation"));

  // Allocate a graph custom each selected species
  size_t offset = 1+Ntot+((Ntot+1)*Ntot)/2;
  custom (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;
    // Coefficient wise operation -> coefficient of variation std.dev(X)/mean(X):
    Eigen::VectorXd cov = data.getColumn(var_idx).array().sqrt() / data.getColumn(mean_idx).array();
    this->addLineGraph(data.getColumn(0), cov, data.getColumnName(mean_idx));
  }

  // customce y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}


SimpleParameterScanPlot::SimpleParameterScanPlot(const QStringList &selected_species, ParamScanTask *task,
                                     QObject *parent)
    : LinePlot("Concentrations (REs)", parent)
{
  // Get species units:
  QString parameter_unit("a.u.");

  // Get parameter scan table:
  Table &data = task->getParameterScan();
  // Number of species in model
  //size_t Ntot = task->getConfig().getModel()->numSpecies();
  // Number of selected species
  size_t Nsel = selected_species.size();

  // Create a plot:
  this->setXLabel(tr("%1").arg(data.getColumnName(0)));
  this->setYLabel(tr("concentration"));

  // Allocate a graph custom each selected species
  custom (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = 1 + species_idx;
    // Get column of RE concentrations
    Eigen::VectorXd cov = data.getColumn(mean_idx);
    this->addLineGraph(data.getColumn(0), cov, data.getColumnName(mean_idx));
  }

  // customce y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}
