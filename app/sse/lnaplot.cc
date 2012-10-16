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
  : Figure("Correlation Coefficients (LNA)", parent)
{
  // Get time unit
  std::stringstream unit_str;
  task->getTimeUnit().dump(unit_str, true);
  QString time_unit(unit_str.str().c_str());
  this->setXLabel(tr("time [%1]").arg(time_unit));
  this->setYLabel(tr("correlation coefficient"));

  size_t num_species = selected_species.size();
  size_t N_cov = (num_species*(num_species-1))/2;
  Table *data = task->getTimeSeries();
  iNA::Ast::Model *model= task->getConfig().getModel();
  QVector<Plot::LineGraph *> graphs(N_cov);

  // Allocate a graph for each colum in time-series:
  Eigen::MatrixXi index_table(num_species, num_species); size_t graph_idx=0;
  for (size_t i=0; i<num_species; i++)
  {
    iNA::Ast::Species *species_i = model->getSpecies(selected_species.at(i).toStdString());
    QString species_name_i = species_i->getIdentifier().c_str();
    if (species_i->hasName()) { species_name_i = species_i->getName().c_str(); }

    size_t species_idx = model->getSpeciesIdx(species_i);
    index_table(i,i) = 1+model->numSpecies(); // offset to first cov column
    index_table(i,i) += species_idx*(model->numSpecies()+1) - (species_idx*(species_idx+1))/2;
    for (size_t j=i+1; j<num_species; j++, graph_idx++)
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
  if (0 == (idx_incr = data->getNumRows()/100)) { idx_incr = 1; }

  // Plot time-series:
  for (size_t k=0; k<data->getNumRows(); k+=idx_incr) {
    size_t idx = 0;
    for (size_t i=0; i<num_species; i++) {
      for (size_t j=i+1; j<num_species; j++, idx++) {
        double x = (*data)(k, 0);
        double y = 0.0;
        if ((0.0 != (*data)(k, index_table(i,i))) && (0.0 != (*data)(k, index_table(j,j)))) {
          y = (*data)(k, index_table(i,j)) /
              (std::sqrt((*data)(k, index_table(i,i))) *
               std::sqrt((*data)(k, index_table(j,j))));
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
  for (size_t i=0; i<N_cov; i++) {
    graphs[i]->commit();
  }

  this->updateAxes();
}
