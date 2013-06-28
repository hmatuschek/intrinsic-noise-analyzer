#include "ssaparamscanplot.hh"
#include <QVector>
#include "ssaparamscantask.hh"
#include "../models/timeseries.hh"
#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../plot/figure.hh"
#include "../plot/configuration.hh"
#include <math.h>
#include <libina/trafo/constantfolder.hh>


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
  // Assemble a constant folder, that excludes the paramter we scan over:
  GiNaC::exmap excludeFromICFold;
  GiNaC::ex parameterSymbol = task->getConfig().getParameter().getSymbol();
  excludeFromICFold[parameterSymbol] = 0;
  iNA::Trafo::InitialValueFolder IC(*model, iNA::Trafo::Filter::ALL, excludeFromICFold);

  // Allocate a graph for each selected species
  size_t offset = 1; // skip parameter column
  for (size_t i=0; i<Nsel; i++) {
    size_t species_idx = model->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset + Ntot + species_idx + (species_idx*(species_idx+1))/2;
    Plot::LineGraphConfig *fano_config = new Plot::LineGraphConfig(config->data(), i);

    // Check if we can evaluate the volume, fold all params except the one we scan over:
    GiNaC::ex compVol = IC.evaluate(model->getSpecies(species_idx)->getCompartment()->getValue());
    // substitute parameter symbol by the column-symbol of that parameter (if present)
    compVol = compVol.subs(parameterSymbol == fano_config->columnSymbol(0));

    // Needs multiplier to obtain correct nondimensional quantity
    GiNaC::ex multiplier = compVol;
    multiplier *= model->getSpeciesUnit().getMultiplier()*std::pow(10.,model->getSpeciesUnit().getScale());
    // Multiply by Avogadro's number if defined in mole
    if (model->getSubstanceUnit().isVariantOf(iNA::Ast::ScaledBaseUnit::MOLE)) {
      multiplier *= iNA::constants::AVOGADRO;
    }

    // Assemble formula for Fano factor.
    GiNaC::ex yexp =
        multiplier * fano_config->columnSymbol(var_idx) / fano_config->columnSymbol(mean_idx);

    // Assemble graph:
    try {
      fano_config->setLabel(data.getColumnName(mean_idx));
      fano_config->setXExpression(fano_config->columnSymbol(0));
      fano_config->setYExpression(yexp);
      config->addGraph(fano_config);
    } catch (iNA::Exception &err) {
      iNA::Utils::Message msg = LOG_MESSAGE(iNA::Utils::Message::WARN);
      msg << "Can not assemble plot formula for Fano factor of species "
          << model->getSpecies(species_idx)->getLabel()
          << "F(" << yexp <<"): " << err.what();
      iNA::Utils::Logger::get().log(msg);
      delete fano_config; continue;
    }
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
