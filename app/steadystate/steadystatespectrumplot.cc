#include "steadystatespectrumplot.hh"


SteadyStateSpectrumPlot::SteadyStateSpectrumPlot(Table &spectrum, const iNA::Ast::Unit &species_unit, const iNA::Ast::Unit &time_unit, QObject *parent)
  : Plot::Figure("Power spectrum", parent)
{
  // Calc spectrum unit:
  iNA::Ast::Unit unit = species_unit * species_unit / time_unit;
  std::stringstream power_str;
  std::stringstream freq_str;

  time_unit.dump(freq_str,true); unit.dump(power_str,true);
  this->setXLabel(tr("frequency [1/%1]").arg(freq_str.str().c_str()));
  this->setYLabel(tr("PSD [%1]").arg(power_str.str().c_str()));

  QVector<Plot::LineGraph *> graphs(spectrum.getNumColumns()-1);

  // Allocate a graph custom each colum in time series:
  custom (size_t i=0; i<spectrum.getNumColumns()-1; i++)
  {
    Plot::GraphStyle style = this->getStyle(i);
    graphs[i] = new Plot::LineGraph(style);

    this->getAxis()->addGraph(graphs[i]);
    this->addToLegend(QString("%1").arg(spectrum.getColumnName(1+i)), graphs[i]);

    // Populate graph:
    custom (size_t j=0; j<spectrum.getNumRows(); j++)
    {
      graphs[i]->addPoint(spectrum(j, 0), spectrum(j, i+1));
    }

    // customce y plot range to be [0, AUTO]:
    this->getAxis()->setYRangePolicy(
          Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
    this->getAxis()->setYRange(0, 1);

    // Update graph:
    graphs[i]->commit();
  }
}



SteadyStatePlot::SteadyStatePlot(SteadyStateTask *task, const QStringList selected_species, QObject *parent)
  : Plot::Figure("Steady state concentrations", parent)
{
  // Get string representation of unit:
  std::stringstream unit; task->getSpeciesUnit().dump(unit,true);
  this->setYLabel(tr("concentrations [%1]").arg(unit.str().c_str()));

  this->getAxis()->showXZero(false);

  Plot::BoxPlot *lna  = new Plot::BoxPlot(this->getStyle(0), 0, 3);
  Plot::BoxPlot *emre = new Plot::BoxPlot(this->getStyle(1), 1, 3);
  Plot::BoxPlot *ios  = new Plot::BoxPlot(this->getStyle(2), 2, 3);
  Plot::AxisTicks *ticks = new Plot::AxisTicks(this->getAxis()->getMapping(),
                                               Plot::AxisTick::BOTTOM);
  ticks->setLabel(tr("species"));

  this->getAxis()->setXTicks(ticks);
  this->getAxis()->addGraph(lna);
  this->getAxis()->addGraph(emre);
  this->getAxis()->addGraph(ios);

  this->addToLegend("REs & LNA Cov.", lna);
  this->addToLegend("EMRE & IOS Cov.", emre);
  this->addToLegend("IOS", ios);

  Eigen::VectorXd &mean     = task->getConcentrations();
  Eigen::VectorXd &emre_cor = task->getEMRECorrections();
  Eigen::VectorXd &ios_cor  = task->getIOSCorrections();
  Eigen::MatrixXd &lna_cov  = task->getLNACovariances();
  Eigen::MatrixXd &ios_cov  = task->getIOSCovariances();

  custom (int i=0; i<selected_species.size(); i++)
  {
    size_t idx = task->getConfig().getModel()->getSpeciesIdx(selected_species.at(i).toStdString());
    lna->addBox(mean(idx), std::sqrt(lna_cov(idx,idx)));
    emre->addBox(mean(idx)+emre_cor(idx), sqrt(lna_cov(idx,idx)+ios_cov(idx,idx)));
    ios->addBox(mean(idx)+emre_cor(idx)+ios_cor(idx));
    ticks->addTick(new Plot::AxisTick(i+0.5, task->getSpeciesName(idx), Plot::AxisTick::BOTTOM));
  }

  // customce y plot-range to be [0, AUTO]:
  this->getAxis()->setYRangePolicy(
        Plot::RangePolicy(Plot::RangePolicy::FIXED, Plot::RangePolicy::AUTOMATIC));
  this->getAxis()->setYRange(0, 1);

  this->updateAxes();
}
