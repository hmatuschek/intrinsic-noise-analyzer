#include "paramscanplot.hh"
#include <QVector>
#include "paramscantask.hh"
#include "../plot/plot.hh"
#include "../plot/configuration.hh"
#include "../models/timeseries.hh"


Plot::PlotConfig *
createParameterScanREPlotConfig(const QStringList &selected_species, ParamScanTask *task)
{
  // Get parameter scan table:
  Table &data = task->getParameterScan();
  Plot::PlotConfig *config = new Plot::PlotConfig(data);

  // Number of selected species
  size_t Nsel = selected_species.size();

  config->setTile(QObject::tr("Concentrations (REs)"));
  config->setXLabel(QObject::tr("%1").arg(data.getColumnName(0)));
  config->setYLabel(QObject::tr("concentration"));

  // Allocate a graph for each selected species
  for (size_t i=0; i<Nsel; i++) {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = 1 + species_idx;

    Plot::LineGraphConfig *mean_config = new Plot::LineGraphConfig(config->data(), i);
    mean_config->setLabel(data.getColumnName(mean_idx));
    mean_config->setXExpression("$0");
    mean_config->setYExpression(QString("$%1").arg(mean_idx));
    config->addGraph(mean_config);
  }

  // Force y plot-range to be [0, AUTO]:
  config->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  config->setYRange(Plot::Range(0, 1));

  return config;
}


Plot::PlotConfig *
createParameterScanLNAPlotConfig(const QStringList &selected_species, ParamScanTask *task)
{
  // Get parameter scan table:
  Table &data = task->getParameterScan();
  Plot::PlotConfig *config = new Plot::PlotConfig(data);

  // Get species units:
  std::stringstream unit_str;
  task->getSpeciesUnit().dump(unit_str, true);
  QString species_unit(unit_str.str().c_str());

  // Number of species in model & number of selected species:
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  size_t Nsel = selected_species.size();

  // Create a plot:
  config->setTile(QObject::tr("Mean concentrations (RE & LNA)"));
  config->setXLabel(QObject::tr("%1").arg(data.getColumnName(0)));
  config->setYLabel(QObject::tr("concentrations [%1]").arg(species_unit));

  // Allocate a graph for each selected species
  size_t offset = 1; // skip parameter column
  for (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;

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


Plot::PlotConfig *
createParameterScanIOSPlotConfig(const QStringList &selected_species, ParamScanTask *task)
{
  // Get parameter scan table:
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
  config->setTile(QObject::tr("Mean concentrations (EMRE & IOS var)"));
  config->setXLabel(QObject::tr("%1").arg(data.getColumnName(0)));
  config->setYLabel(QObject::tr("concentrations [%1]").arg(species_unit));

  // Allocate a graph for each selected species
  size_t offset = 1+Ntot+((Ntot+1)*Ntot)/2; // skip parameter+EMRE+LNA columns
  for (size_t i=0; i<Nsel; i++) {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;

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


Plot::PlotConfig *
createParameterScanLNACVPlotConfig(const QStringList &selected_species, ParamScanTask *task)
{
  // Get parameter scan table:
  Table &data = task->getParameterScan();
  Plot::PlotConfig *config = new Plot::PlotConfig(data);

  // Number of species in model
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  // Number of selected species
  size_t Nsel = selected_species.size();

  // Create a plot:
  config->setTile(QObject::tr("Coefficient of variation (LNA)"));
  config->setXLabel(QObject::tr("%1").arg(data.getColumnName(0)));
  config->setYLabel(QObject::tr("$C_V$"));

  // Allocate a graph for each selected species
  size_t offset = 1; // skip parameter column
  for (size_t i=0; i<Nsel; i++)
  {
    iNA::Ast::Species *species = task->getConfig().getModel()->getSpecies(selected_species.at(i).toStdString());
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(species);
    std::string species_name = species->getIdentifier();
    if (species->hasName()) { species_name = species->getName(); }

    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;
    Plot::LineGraphConfig *cv_config = new Plot::LineGraphConfig(config->data(), i);
    cv_config->setLabel(QString("LNA(%1)").arg(species_name.c_str()));
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


Plot::PlotConfig *
createParameterScanLNAFanoPlotConfig(const QStringList &selected_species, ParamScanTask *task)
{
  // Get parameter scan table etc...
  Table &data = task->getParameterScan();
  iNA::Ast::Model *model = task->getConfig().getModel();
  Plot::PlotConfig *config = new Plot::PlotConfig(data);
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  size_t Nsel = selected_species.size();

  config->setTile(QObject::tr("Fano Factor (LNA)"));
  config->setXLabel(QObject::tr("%1").arg(data.getColumnName(0)));
  config->setYLabel(QObject::tr("$F_Fano$"));

  // Assemble a constant folder, that excludes the paramter we scan over:
  GiNaC::exmap excludeFromICFold;
  GiNaC::ex parameterSymbol = task->getConfig().getParameter().getSymbol();
  excludeFromICFold[parameterSymbol] = 0;
  iNA::Trafo::InitialValueFolder IC(*model, iNA::Trafo::Filter::ALL, excludeFromICFold);

  // Allocate a graph for each selected species
  size_t offset = 1; // skip parameter column
  for (size_t i=0; i<Nsel; i++) {
    // Get some indices:
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;

    Plot::LineGraphConfig *fano_config = new Plot::LineGraphConfig(config->data(), i);

    // Check if we can evaluate the volume, fold all params except the one we scan over:
    GiNaC::ex compVol = IC.apply(model->getSpecies(species_idx)->getCompartment()->getValue());
    // substitute parameter symbol by the column-symbol of that parameter (if present)
    compVol = compVol.subs(parameterSymbol == fano_config->columnSymbol(0));

    // Needs multiplier to obtain correct nondimensional quantity
    GiNaC::ex multiplier = compVol;
    multiplier *= model->getSpeciesUnit().getMultiplier()*std::pow(10.,model->getSpeciesUnit().getScale());
    // Multiply by Avogadro's number if defined in mole
    if (model->getSubstanceUnit().isVariantOf(iNA::Ast::Unit::MOLE)) {
      multiplier *= iNA::constants::AVOGADRO;
    }
    // Assemble formula for Fano factor.
    GiNaC::ex yexp =
        multiplier * fano_config->columnSymbol(var_idx) / fano_config->columnSymbol(mean_idx);

    // Coefficient wise operation -> coefficient of variation variance(X)/mean(X):
    try {
      fano_config->setLabel(QString("LNA(%1)").arg(model->getSpecies(species_idx)->getLabel().c_str()));
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

Plot::PlotConfig *
createParameterScanIOSCVPlotConfig(const QStringList &selected_species, ParamScanTask *task)
{
  // Get parameter scan table:
  Table &data = task->getParameterScan();
  Plot::PlotConfig *config = new Plot::PlotConfig(data);

  // Number of species in model
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  size_t Nsel = selected_species.size();

  // Create a plot:
  config->setTile(QObject::tr("Coefficient of variation (IOS)"));
  config->setXLabel(QObject::tr("%1").arg(data.getColumnName(0)));
  config->setYLabel(QObject::tr("$C_V$"));

  // Allocate a graph for each selected species
  size_t offset = 1+Ntot+((Ntot+1)*Ntot)/2;
  for (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;

    Plot::LineGraphConfig *cv_config = new Plot::LineGraphConfig(config->data(), i);
    cv_config->setLabel(QString("IOS(%1)").arg(task->getConfig().getModel()->getSpecies(species_idx)->getLabel().c_str()));
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

Plot::PlotConfig *
createParameterScanIOSFanoPlotConfig(const QStringList &selected_species, ParamScanTask *task)
{
  // Get parameter scan table:
  Table &data = task->getParameterScan();
  Plot::PlotConfig *config = new Plot::PlotConfig(data);

  // Number of species in model
  size_t Ntot = task->getConfig().getModel()->numSpecies();
  // Number of selected species
  size_t Nsel = selected_species.size();

  // Create a plot:
  config->setTile(QObject::tr("Fano Factor (IOS)"));
  config->setXLabel(QObject::tr("%1").arg(data.getColumnName(0)));
  config->setYLabel(QObject::tr("$F_Fano$"));

  iNA::Ast::Model *model = task->getConfig().getModel();

  // Assemble a constant folder, that excludes the paramter we scan over:
  GiNaC::exmap excludeFromICFold;
  GiNaC::ex parameterSymbol = task->getConfig().getParameter().getSymbol();
  excludeFromICFold[parameterSymbol] = 0;
  iNA::Trafo::InitialValueFolder IC(*model, iNA::Trafo::Filter::ALL, excludeFromICFold);

  // Allocate a graph for each selected species
  size_t offset = 1+Ntot+((Ntot+1)*Ntot)/2; // skip parameter column, RE and LNA columns
  for (size_t i=0; i<Nsel; i++) {
    size_t species_idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ntot + species_idx*(Ntot+1) - (species_idx*(species_idx+1))/2;

    Plot::LineGraphConfig *fano_config = new Plot::LineGraphConfig(config->data(), i);

    // Check if we can evaluate the volume, fold all params except the one we scan over:
    GiNaC::ex compVol = IC.apply(model->getSpecies(species_idx)->getCompartment()->getValue());
    // substitute parameter symbol by the column-symbol of that parameter (if present)
    compVol = compVol.subs(parameterSymbol == fano_config->columnSymbol(0));

    // Needs multiplier to obtain correct nondimensional quantity
    GiNaC::ex multiplier = compVol;
    multiplier *= model->getSpeciesUnit().getMultiplier()*std::pow(10.,model->getSpeciesUnit().getScale());
    // Multiply by Avogadro's number if defined in mole
    if (model->getSubstanceUnit().isVariantOf(iNA::Ast::Unit::MOLE)) {
      multiplier *= iNA::constants::AVOGADRO;
    }
    // Assemble formula for Fano factor.
    GiNaC::ex yexp =
        multiplier * fano_config->columnSymbol(var_idx) / fano_config->columnSymbol(mean_idx);

    try {
      // Coefficient wise operation -> coefficient of variation variance(X)/mean(X):
      fano_config->setLabel(QString("IOS(%1)").arg(task->getConfig().getModel()->getSpecies(species_idx)->getLabel().c_str()));
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
