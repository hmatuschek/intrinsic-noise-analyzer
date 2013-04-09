#include "iosplot.hh"
#include "iostask.hh"
#include "../plot/plot.hh"
#include "../plot/graph.hh"
#include "../plot/linegraph.hh"
#include "../models/timeseries.hh"
#include "../views/varianceplot.hh"
#include <QVector>


Plot::PlotConfig *
createIOSEMRETimeSeriesPlotConfig(const QStringList &selected_species, IOSTask *task)
{
  Table *series = task->getTimeSeries();
  Plot::PlotConfig *config = new Plot::PlotConfig(*series);

  // Get time & species units and assemble axis labels
  std::stringstream unit_str;
  task->getSpeciesUnit().dump(unit_str, true);
  QString species_unit(unit_str.str().c_str());
  unit_str.str("");
  task->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());
  config->setTile(QObject::tr("Mean concentrations (EMRE & IOS Var.)"));
  config->setXLabel(QObject::tr("time [%1]").arg(time_unit));
  if (task->getSpeciesUnit().isConcentrationUnit()) {
    config->setYLabel(QObject::tr("concentrations [%1]").arg(species_unit));
  } else {
    config->setYLabel(QObject::tr("amount [%1]").arg(species_unit));
  }

  // Determine some numbers
  iNA::Ast::Model *model = task->getConfig().getModel();
  size_t Nss = selected_species.size();
  size_t Ns = model->numSpecies();    // Number of species in model
  size_t offset = 1+Ns+(Ns*(Ns+1))/2; // skip time, RE & LNA

  // Assemble plots
  for (size_t i=0; i<Nss; i++) {
    size_t species_idx = model->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ns + species_idx*(Ns+1) - (species_idx*(species_idx+1))/2;
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
createIOSEMREComparePlotConfig(const QStringList &selected_species, IOSTask *task)
{
  Table *series = task->getTimeSeries();
  Plot::PlotConfig *config = new Plot::PlotConfig(*series);

  // Get time & species units and assemble axis labels
  std::stringstream unit_str;
  task->getSpeciesUnit().dump(unit_str, true);
  QString species_unit(unit_str.str().c_str());
  unit_str.str("");
  task->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());
  config->setTile(QObject::tr("Mean concentrations (RE, EMRE & IOS)"));
  config->setXLabel(QObject::tr("time [%1]").arg(time_unit));
  if (task->getSpeciesUnit().isConcentrationUnit()) {
    config->setYLabel(QObject::tr("concentrations [%1]").arg(species_unit));
  } else {
    config->setYLabel(QObject::tr("amount [%1]").arg(species_unit));
  }

  iNA::Ast::Model *model = task->getConfig().getModel();
  size_t Ntot  = model->numSpecies();
  size_t Nsel = selected_species.size();
  size_t off_re   = 1;
  size_t off_emre = 1 + Ntot + (Ntot*(Ntot+1))/2;
  size_t off_ios  = off_emre + Ntot + (Ntot*(Ntot+1))/2;

  // Allocate a graph for each colum in time-series:
  for (size_t i=0; i<Nsel; i++)
  {
    Plot::LineGraphConfig *ios_graph = new Plot::LineGraphConfig(series, i);
    Plot::LineGraphConfig *emre_graph = new Plot::LineGraphConfig(series, i);
    Plot::LineGraphConfig *re_graph = new Plot::LineGraphConfig(series, i);
    QColor line_color = ios_graph->lineColor();
    line_color.setAlpha(128); emre_graph->setLineColor(line_color);
    line_color.setAlpha(64);  re_graph->setLineColor(line_color);

    size_t species_idx = model->getSpeciesIdx(selected_species.at(i).toStdString());
    ios_graph->setLabel(series->getColumnName(off_ios+species_idx));
    ios_graph->setXExpression("$0");
    ios_graph->setYExpression(QString("$%1").arg(off_ios+species_idx));
    config->addGraph(ios_graph);
    emre_graph->setLabel(series->getColumnName(off_emre+species_idx));
    emre_graph->setXExpression("$0");
    emre_graph->setYExpression(QString("$%1").arg(off_emre+species_idx));
    config->addGraph(emre_graph);
    re_graph->setLabel(series->getColumnName(off_re+species_idx));
    re_graph->setXExpression("$0");
    re_graph->setYExpression(QString("$%1").arg(off_re+species_idx));
    config->addGraph(re_graph);
  }

  // Force y plot-range to be [0, AUTO]:
  config->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  config->setYRange(Plot::Range(0, 1));

  return config;
}


Plot::PlotConfig *
createIOSEMRECorrelationPlotConfig(const QStringList &selected_species, IOSTask *task)
{
  Table *series = task->getTimeSeries();
  Plot::PlotConfig *config = new Plot::PlotConfig(*series);

  std::stringstream unit_str;
  task->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());
  config->setTile(QObject::tr("Correlation Coefficients (IOS)"));
  config->setXLabel(QObject::tr("time [%1]").arg(time_unit));
  config->setYLabel(QObject::tr("correlation coefficient"));

  iNA::Ast::Model *model = task->getConfig().getModel();
  size_t Nss = selected_species.size();
  size_t Ns = model->numSpecies();

  // Allocate a graph for each colum in time-series:
  Eigen::MatrixXi index_table(Nss, Nss); size_t graph_idx=0;
  for (size_t i=0; i<Nss; i++)
  {
    iNA::Ast::Species *species_i = model->getSpecies(selected_species.at(i).toStdString());
    QString species_name_i = species_i->getIdentifier().c_str();
    if (species_i->hasName()) { species_name_i = species_i->getName().c_str(); }

    size_t species_idx = model->getSpeciesIdx(species_i);
    index_table(i,i) = 1+2*Ns+(Ns*(Ns+1))/2; // offset to first cov column
    index_table(i,i) += species_idx*(model->numSpecies()+1) - (species_idx*(species_idx+1))/2;
    for (size_t j=i+1; j<Nss; j++, graph_idx++)
    {
      index_table(i,j) = index_table(j,i) = index_table(i,i)+j-i;

      iNA::Ast::Species *species_j = model->getSpecies(selected_species.at(j).toStdString());
      QString species_name_j = species_j->getIdentifier().c_str();
      if (species_j->hasName()) { species_name_j = species_j->getName().c_str(); }

      Plot::LineGraphConfig *cor_config = new Plot::LineGraphConfig(series, graph_idx);
      cor_config->setLabel(QString("corr(%1, %2)").arg(species_name_i).arg(species_name_j));
      cor_config->setXExpression("$0");
      cor_config->setYExpression(
            QString("$%1/sqrt($%2*$%3)").arg(index_table(i,j)).arg(index_table(i,i)).arg(index_table(j,j)));
      config->addGraph(cor_config);
    }
  }

  // Force y plot-range to be [-1, 1]:
  config->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::FIXED));
  config->setYRange(Plot::Range(-1.1, 1.1));

  return config;
}
