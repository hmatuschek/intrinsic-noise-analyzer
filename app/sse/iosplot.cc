#include "iosplot.hh"
#include "../plot/plot.hh"
#include "../plot/linegraph.hh"
#include <QVector>


IOSEMRETimeSeriesPlot::IOSEMRETimeSeriesPlot(const QStringList &selected_species,
                                             IOSTask *task, QObject *parent)
  : VariancePlot("Mean concentrations (EMRE & IOS Var.)", parent)
{
  // Get time & species units and assemble axis labels
  std::stringstream unit_str;
  task->getSpeciesUnit().dump(unit_str, true);
  QString species_unit(unit_str.str().c_str());
  unit_str.str("");
  task->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());
  setXLabel(tr("time [%1]").arg(time_unit));
  if (task->getSpeciesUnit().isConcentrationUnit()) {
    setYLabel(tr("concentrations [%1]").arg(species_unit));
  } else {
    setYLabel(tr("amount [%1]").arg(species_unit));
  }

  // Determine some numbers
  iNA::Ast::Model *model = task->getConfig().getModel();
  size_t Nss = selected_species.size();
  size_t Ns = model->numSpecies();    // Number of species in model
  size_t offset = 1+Ns+(Ns*(Ns+1))/2; // skip time, RE & LNA
  Table *series = task->getTimeSeries();

  // Assemble plots
  for (size_t i=0; i<Nss; i++) {
    size_t species_idx = model->getSpeciesIdx(selected_species.at(i).toStdString());
    size_t mean_idx = offset + species_idx;
    size_t var_idx  = offset+Ns + species_idx*(Ns+1) - (species_idx*(species_idx+1))/2;
    this->addVarianceGraph(series->getColumn(0), series->getColumn(mean_idx),
                           series->getColumn(var_idx), series->getColumnName(mean_idx));
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);
  this->updateAxes();
}



IOSEMREComparePlot::IOSEMREComparePlot(const QStringList &selected_species, IOSTask *task,
                                       QObject *parent)
  : LinePlot("Mean concentrations (RE, EMRE & IOS)", parent)
{
  // Get time & species units and assemble axis labels
  std::stringstream unit_str;
  task->getSpeciesUnit().dump(unit_str, true);
  QString species_unit(unit_str.str().c_str());
  unit_str.str("");
  task->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());
  setXLabel(tr("time [%1]").arg(time_unit));
  if (task->getSpeciesUnit().isConcentrationUnit()) {
    setYLabel(tr("concentrations [%1]").arg(species_unit));
  } else {
    setYLabel(tr("amount [%1]").arg(species_unit));
  }

  iNA::Ast::Model *model = task->getConfig().getModel();
  Table *series = task->getTimeSeries();
  size_t Ntot  = model->numSpecies();
  size_t Nsel = selected_species.size();
  size_t off_re   = 1;
  size_t off_emre = 1 + Ntot + (Ntot*(Ntot+1))/2;
  size_t off_ios  = off_emre + Ntot + (Ntot*(Ntot+1))/2;

  QVector<Plot::LineGraph *> re_graphs(Nsel);
  QVector<Plot::LineGraph *> emre_graphs(Nsel);
  QVector<Plot::LineGraph *> ios_graphs(Nsel);

  // Allocate a graph for each colum in time-series:
  for (size_t i=0; i<Nsel; i++)
  {
    size_t species_idx = model->getSpeciesIdx(selected_species.at(i).toStdString());

    Plot::GraphStyle style = this->getStyle(i);
    ios_graphs[i]  = new Plot::LineGraph(style);

    QColor line_color = style.getLineColor(); line_color.setAlpha(128);
    style.setLineColor(line_color);
    emre_graphs[i] = new Plot::LineGraph(style);

    line_color.setAlpha(64);
    style.setLineColor(line_color);
    re_graphs[i] = new Plot::LineGraph(style);

    this->axis->addGraph(ios_graphs[i]);
    this->axis->addGraph(emre_graphs[i]);
    this->axis->addGraph(re_graphs[i]);

    this->addToLegend(series->getColumnName(species_idx+off_ios),  ios_graphs[i]);
    this->addToLegend(series->getColumnName(species_idx+off_emre), emre_graphs[i]);
    this->addToLegend(series->getColumnName(species_idx+off_re), re_graphs[i]);
  }

  // Do not plot all
  int idx_incr = 0;
  if (0 == (idx_incr = series->getNumRows()/100)) {
    idx_incr = 1;
  }

  // Plot time-series:
  for (size_t j=0; j<series->getNumRows(); j+=idx_incr) {
    for (size_t i=0; i<Nsel; i++) {
      size_t species_idx = model->getSpeciesIdx(selected_species.at(i).toStdString());
      re_graphs[i]->addPoint((*series)(j,0), (*series)(j, species_idx+off_re));
      emre_graphs[i]->addPoint((*series)(j, 0), (*series)(j, species_idx+off_emre));
      ios_graphs[i]->addPoint((*series)(j, 0), (*series)(j, species_idx+off_ios));
    }
  }

  // Force y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  for (size_t i=0; i<Nsel; i++) {
    re_graphs[i]->commit();
    emre_graphs[i]->commit();
    ios_graphs[i]->commit();
  }

  this->updateAxes();
}



/* ********************************************************************************************* *
 * Implementation of correlation plot.
 * ********************************************************************************************* */
IOSEMRECorrelationPlot::IOSEMRECorrelationPlot(const QStringList &selected_species, IOSTask *task, QObject *parent)
  : Figure("Correlation Coefficients (IOS)", parent)
{
  std::stringstream unit_str;
  task->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());
  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("correlation coefficient"));

  iNA::Ast::Model *model = task->getConfig().getModel();
  Table *series = task->getTimeSeries();
  size_t Nss = selected_species.size();
  size_t Ns = model->numSpecies();
  size_t Ncov = (Nss*(Nss-1))/2;
  QVector<Plot::LineGraph *> graphs(Ncov);

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

      Plot::GraphStyle style = this->getStyle(graph_idx);
      graphs[graph_idx] = new Plot::LineGraph(style);
      this->axis->addGraph(graphs[graph_idx]);
      this->addToLegend(
            QString("corr(%1, %2)").arg(species_name_i).arg(species_name_j), graphs[graph_idx]);
    }
  }

  // Do not plot all
  int idx_incr ;
  if (0 == (idx_incr = series->getNumRows()/100)) { idx_incr = 1; }

  // Plot time-series:
  for (size_t k=0; k<series->getNumRows(); k+=idx_incr) {
    size_t idx = 0;
    for (size_t i=0; i<Nss; i++) {
      for (size_t j=i+1; j<Nss; j++, idx++) {
        double x = (*series)(k, 0);
        double y = 0.0;
        if ((0.0 != (*series)(k, index_table(i,i))) && (0.0 != (*series)(k, index_table(j,j)))) {
          y = (*series)(k, index_table(i,j)) /
              (std::sqrt((*series)(k, index_table(i,i))) *
               std::sqrt((*series)(k, index_table(j,j))));
        }
        graphs[idx]->addPoint(x, y);
      }
    }
  }

  // Force y plot-range to be [-1, 1]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::FIXED));
  this->getAxis()->setYRange(-1.1, 1.1);

  // Finally commit changes:
  for (size_t i=0; i<Ncov; i++) {
    graphs[i]->commit();
  }
  this->updateAxes();
}
