#include "ssaparamscanplot.hh"
#include "../plot/plot.hh"
#include "../plot/linegraph.hh"
#include <QVector>
#include "../views/varianceplot.hh"
#include <math.h>


SSAParameterScanPlot::SSAParameterScanPlot(const QStringList &selected_species, SSAParamScanTask *task,
                                     QObject *parent)
    : LinePlot("Mean concentrations (SSA)", parent)
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

  // Allocate a graph for each selected species
  size_t offset = 1; // skip parameter column
  for (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset + Ntot + species_idx + (species_idx*(species_idx+1))/2;
    this->addVarianceGraph(data.getColumn(0), data.getColumn(mean_idx), data.getColumn(var_idx),
                           data.getColumnName(mean_idx));
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}


SSAParameterScanCVPlot::SSAParameterScanCVPlot(const QStringList &selected_species, SSAParamScanTask *task,
                                           QObject *parent)
    : LinePlot("Coefficient of variation (SSA)", parent)
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
  this->setYLabel(tr("$C_V$"));

  // Allocate a graph for each selected species
  size_t offset = 1; // skip parameter column
  for (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset + Ntot + species_idx + (species_idx*(species_idx+1))/2;
    // Coefficient wise operation -> coefficient of variation std.dev(X)/mean(X):
    Eigen::VectorXd cov = data.getColumn(var_idx).array().sqrt() / data.getColumn(mean_idx).array();
    this->addLineGraph(data.getColumn(0), cov, data.getColumnName(mean_idx));
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}

SSAParameterScanFanoPlot::SSAParameterScanFanoPlot(const QStringList &selected_species, SSAParamScanTask *task,
                                           QObject *parent)
    : LinePlot("Fano Factor (SSA)", parent)
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
  this->setYLabel(tr("$F_Fano$"));

  iNA::Ast::Model *model = task->getConfig().getModel();

  // Allocate a graph for each selected species
  size_t offset = 1; // skip parameter column
  for (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = model->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset + Ntot + species_idx + (species_idx*(species_idx+1))/2;

    // Check if we can evaluate the volume
    if(!GiNaC::is_a<GiNaC::numeric>(model->getSpecies(species_idx)->getCompartment()->getValue())) continue;

    // Needs multiplier to obtain correct nondimensional quantity
    double multiplier = Eigen::ex2double(model->getSpecies(species_idx)->getCompartment()->getValue());
    multiplier *= Eigen::ex2double(model->getSpeciesUnit().getMultiplier()*std::pow(10.,model->getSpeciesUnit().getScale()));
    // Multiply by Avogadro's number if defined in mole
    if (model->getSubstanceUnit().isVariantOf(iNA::Ast::ScaledBaseUnit::MOLE)) multiplier *= iNA::constants::AVOGADRO;

    // Coefficient wise operation -> coefficient of variation variance(X)/mean(X):
    Eigen::VectorXd fano =  multiplier*( data.getColumn(var_idx).array() / data.getColumn(mean_idx).array() );
    this->addLineGraph(data.getColumn(0), fano, data.getColumnName(mean_idx));
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}


SSASimpleParameterScanPlot::SSASimpleParameterScanPlot(const QStringList &selected_species, SSAParamScanTask *task,
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

  // Allocate a graph for each selected species
  for (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = 1 + species_idx;
    // Get column of RE concentrations
    Eigen::VectorXd cov = data.getColumn(mean_idx);
    this->addLineGraph(data.getColumn(0), cov, data.getColumnName(mean_idx));
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}



Plot::PlotConfig *
createSSAParameterScanPlotConfig(const QStringList &selected_species, SSAParamScanTask *task)
{
  // Get parameter scan table & create config
  Table &data = task->getParameterScan();
  Plot::PlotConfig *config = new Plot::PlotConfig(data);

  // Get species units:
  std::stringstream unit_str;
  task->getSpeciesUnit().dump(unit_str, true);
  QString species_unit(unit_str.str().c_str());

  // Number of species in model
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  // Number of selected species
  size_t Nsel = selected_species.size();

  // Create a plot:
  config->setTile(QObject::tr("Mean concentrations (SSA)"));
  config->setXLabel(QObject::tr("%1").arg(data.getColumnName(0)));
  config->setYLabel(QObject::tr("concentrations [%1]").arg(species_unit));

  // Allocate a graph for each selected species
  size_t offset = 1; // skip parameter column
  for (size_t i=0; i<Nsel; i++) {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset + Ntot + species_idx + (species_idx*(species_idx+1))/2;
    Plot::VarianceLineGraphConfig *var_config = new Plot::VarianceLineGraphConfig(config->data(), i);
    var_config->setLabel(data.getColumnName(mean_idx));
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


Plot::Figure *
createSSAParameterScanPlot(const QStringList &selected_species, SSAParamScanTask *task) {
  Plot::PlotConfig *config = createSSAParameterScanPlotConfig(selected_species, task);
  return config->createFigure();
}


Plot::PlotConfig *
createSSAParameterScanCVPlotConfig(const QStringList &selected_species, SSAParamScanTask *task)
{
  // Get parameter scan table and config
  Table &data = task->getParameterScan();
  Plot::PlotConfig *config = new Plot::PlotConfig(data);

  // Number of species in model
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  // Number of selected species
  size_t Nsel = selected_species.size();

  // Set labels
  config->setTile(QObject::tr("Coefficient of variation (SSA)"));
  config->setXLabel(QObject::tr("%1").arg(data.getColumnName(0)));
  config->setYLabel(QObject::tr("$C_V$"));

  // Allocate a graph for each selected species
  size_t offset = 1; // skip parameter column
  for (size_t i=0; i<Nsel; i++) {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset + Ntot + species_idx + (species_idx*(species_idx+1))/2;

    // Coefficient wise operation -> coefficient of variation std.dev(X)/mean(X):
    Plot::LineGraphConfig *cv_config = new Plot::LineGraphConfig(config->data(), i);
    cv_config->setLabel(data.getColumnName(mean_idx));
    cv_config->setXExpression("$0");
    cv_config->setYExpression(QString("sqrt($%1)/$%2").arg(var_idx).arg(mean_idx));
    config->addGraph(cv_config);
  }

  // Force y plot-range to be [0, AUTO]:
  config->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  config->setYRange(Plot::Range(0, 1));
  return config;
}


Plot::Figure *
createSSAParameterScanCVPlot(const QStringList &selected_species, SSAParamScanTask *task) {
  Plot::PlotConfig *config = createSSAParameterScanCVPlotConfig(selected_species, task);
  return config->createFigure();
}


Plot::PlotConfig *
createSSAParameterScanFanoPlotConfig(const QStringList &selected_species, SSAParamScanTask *task)
{
  // Get parameter scan table & config
  Table &data = task->getParameterScan();
  Plot::PlotConfig *config = new Plot::PlotConfig(data);

  // Number of species in model
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  // Number of selected species
  size_t Nsel = selected_species.size();
  // Set labels
  config->setTile(QObject::tr("Fano Factor (SSA)"));
  config->setXLabel(QObject::tr("%1").arg(data.getColumnName(0)));
  config->setYLabel(QObject::tr("$F_Fano$"));

  iNA::Ast::Model *model = task->getConfig().getModel();

  // Allocate a graph for each selected species
  size_t offset = 1; // skip parameter column
  for (size_t i=0; i<Nsel; i++) {
    size_t species_idx = model->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset + Ntot + species_idx + (species_idx*(species_idx+1))/2;

    // Check if we can evaluate the volume
    if(!GiNaC::is_a<GiNaC::numeric>(model->getSpecies(species_idx)->getCompartment()->getValue())) continue;
    // Needs multiplier to obtain correct nondimensional quantity
    double multiplier = Eigen::ex2double(model->getSpecies(species_idx)->getCompartment()->getValue());
    multiplier *= Eigen::ex2double(model->getSpeciesUnit().getMultiplier()*std::pow(10.,model->getSpeciesUnit().getScale()));
    // Multiply by Avogadro's number if defined in mole
    if (model->getSubstanceUnit().isVariantOf(iNA::Ast::ScaledBaseUnit::MOLE)) multiplier *= iNA::constants::AVOGADRO;
    // Assemble graph:
    Plot::LineGraphConfig *fano_config = new Plot::LineGraphConfig(config->data(), i);
    fano_config->setLabel(data.getColumnName(mean_idx));
    fano_config->setXExpression("$0");
    fano_config->setYExpression(QString("%1*$%2/$%3").arg(multiplier).arg(var_idx).arg(mean_idx));
    config->addGraph(fano_config);
  }

  // Force y plot-range to be [0, AUTO]:
  config->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  config->setYRange(Plot::Range(0, 1));
  return config;
}


Plot::Figure *
createSSAParameterScanFanoPlot(const QStringList &selected_species, SSAParamScanTask *task) {
  Plot::PlotConfig *config = createSSAParameterScanFanoPlotConfig(selected_species, task);
  return config->createFigure();
}
