#include "varianceplot.hh"

VariancePlot::VariancePlot(const QString &title, QObject *parent)
  : Plot::Figure(title, parent)
{
  // Pass...
}


void
VariancePlot::addVarianceGraph(const Eigen::VectorXd &x, const Eigen::VectorXd &mean,
                               const Eigen::VectorXd &var, const QString &label)
{
  // Determins style and create graph:
  Plot::GraphStyle style = this->getStyle(this->_axis->getNumGraphs());
  Plot::VarianceLineGraph *graph = new Plot::VarianceLineGraph(style);
  this->_axis->addGraph(graph);

  // Add graph to legent if label is given:
  if(0 != label.size()) {
    this->addToLegend(label, graph);
  }

  // Plot data...
  // Do not plot all point, limited to 100 points:
  int idx_incr = 0;
  if (0 == (idx_incr = x.rows()/100)) {
    idx_incr = 1;
  }
  // Plot...
  for (size_t i=0; i<(size_t)x.rows(); i+=idx_incr) {
    graph->addPoint(x(i), mean(i), std::sqrt(var(i)));
  }
  graph->commit();
}




LinePlot::LinePlot(const QString &title, QObject *parent)
  : Plot::Figure(title, parent)
{
  // Pass...
}


void
LinePlot::addVarianceGraph(const Eigen::VectorXd &x, const Eigen::VectorXd &mean,
                               const Eigen::VectorXd &var, const QString &label)
{
  // Determins style and create graph:
  Plot::GraphStyle style = this->getStyle(this->_axis->getNumGraphs());
  Plot::VarianceLineGraph *graph = new Plot::VarianceLineGraph(style);
  this->_axis->addGraph(graph);

  // Add graph to legent if label is given:
  if(0 != label.size()) {
    this->addToLegend(label, graph);
  }

  // Plot data...
  // Do not plot all point, limited to 100 points:
  int idx_incr = 0;
  if (0 == (idx_incr = x.rows()/100)) {
    idx_incr = 1;
  }
  // Plot...
  for (size_t i=0; i<(size_t)x.rows(); i+=idx_incr) {
    graph->addPoint(x(i), mean(i), std::sqrt(var(i)));
  }
  graph->commit();
}


void
LinePlot::addLineGraph(const Eigen::VectorXd &x, const Eigen::VectorXd &mean, const QString &label)
{
  // Determins style and create graph:
  Plot::GraphStyle style = this->getStyle(this->_axis->getNumGraphs());
  Plot::LineGraph *graph = new Plot::LineGraph(style);
  this->_axis->addGraph(graph);

  // Add graph to legent if label is given:
  if(0 != label.size()) {
    this->addToLegend(label, graph);
  }

  // Plot data...
  // Do not plot all point, limited to 100 points:
  int idx_incr = 0;
  if (0 == (idx_incr = x.rows()/100)) {
    idx_incr = 1;
  }
  // Plot...
  for (size_t i=0; i<(size_t)x.rows(); i+=idx_incr) {
    graph->addPoint(x(i), mean(i));
  }
  graph->commit();
}
