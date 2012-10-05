#include "timeseriesplotwizard.hh"
#include "../plot/linegraph.hh"
#include "../plot/variancelinegraph.hh"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QComboBox>
#include <QCompleter>
#include <QAbstractProxyModel>



/* ******************************************************************************************** *
 * Implementation of TimeSeriesGraphConfig
 * ******************************************************************************************** */
TimeSeriesGraphConfig::TimeSeriesGraphConfig(
    Table *table, PlotType type, size_t mean_column, size_t var_column)
  : _table(table), _context(table), _type(type), _mean_expression(mean_column), _var_expression(var_column), _label("Graph")
{
  // pass...
}

TimeSeriesGraphConfig::TimeSeriesGraphConfig(const TimeSeriesGraphConfig &other)
  : _table(other._table), _context(other._context), _type(other._type),
    _mean_expression(other._mean_expression), _var_expression(other._var_expression), _label(other._label)
{
  // pass...
}


TimeSeriesGraphConfig::PlotType
TimeSeriesGraphConfig::plotType() const {
  return _type;
}

void
TimeSeriesGraphConfig::setPlotType(PlotType type) {
  _type = type;
}

GiNaC::ex
TimeSeriesGraphConfig::meanColumn() const {
  return _mean_expression;
}

bool
TimeSeriesGraphConfig::setMeanColumn(const QString &formula) {
  if (! PlotFormulaParser::check(formula, _context)) { return false; }
  _mean_expression = PlotFormulaParser::parse(formula, _context);
  return true;
}

GiNaC::ex
TimeSeriesGraphConfig::varColumn() const {
  return _var_expression;
}

bool
TimeSeriesGraphConfig::setVarColumn(const QString &formula) {
  if (! PlotFormulaParser::check(formula, _context)) { return false; }
  _var_expression = PlotFormulaParser::parse(formula, _context);
  return true;
}

const QString &
TimeSeriesGraphConfig::label() const {
  return _label;
}

void
TimeSeriesGraphConfig::setLabel(const QString &label) {
  _label = label;
}

const Table *
TimeSeriesGraphConfig::table() const {
  return _table;
}

Plot::Graph *
TimeSeriesGraphConfig::create(const Plot::GraphStyle &style)
{
  if (LINE_GRAPH == _type) {
    Plot::LineGraph *graph = new Plot::LineGraph(style);
    for (size_t i=0; i<_table->getNumRows(); i++) {
      graph->addPoint((*_table)(i, 0), evalMean(i));
    }
    return graph;
  }

  Plot::VarianceLineGraph *graph = new Plot::VarianceLineGraph(style);
  for (size_t i=0; i<_table->getNumRows(); i++) {
    graph->addPoint((*_table)(i, 0), evalMean(i), std::sqrt(evalVar(i)));
  }
  return graph;
}

double
TimeSeriesGraphConfig::evalMean(size_t row) {
  return _context(row, _mean_expression);
}

double
TimeSeriesGraphConfig::evalVar(size_t row) {
  return _context(row, _var_expression);
}



/* ******************************************************************************************** *
 * Implementation of TimeSeriesGraphList
 * ******************************************************************************************** */
TimeSeriesGraphList::TimeSeriesGraphList(QObject *parent)
  : QAbstractListModel(parent), _graphs()
{
  // pass..
}

int
TimeSeriesGraphList::rowCount(const QModelIndex &parent) const {
  return _graphs.size();
}

QVariant
TimeSeriesGraphList::data(const QModelIndex &index, int role) const {
  if (Qt::DisplayRole != role) { return QVariant(); }
  if (index.row() >= _graphs.size()) { return QVariant(); }
  return _graphs.at(index.row()).label();
}

void
TimeSeriesGraphList::addGraph(const TimeSeriesGraphConfig &graph) {
  beginInsertRows(QModelIndex(), _graphs.size(), _graphs.size());
  _graphs.append(graph);
  endInsertRows();
}

TimeSeriesGraphConfig &
TimeSeriesGraphList::graph(int idx) {
  return _graphs[idx];
}

void
TimeSeriesGraphList::removeGraph(int idx) {
  if (idx < 0 || idx >= _graphs.size()) { return; }
  beginRemoveRows(QModelIndex(), idx, idx);
  _graphs.removeAt(idx);
  endRemoveRows();
}


/* ******************************************************************************************** *
 * Implementation of TimeSeriesPlotDialog
 * ******************************************************************************************** */
TimeSeriesPlotDialog::TimeSeriesPlotDialog(Table *table, QWidget *parent)
  : QDialog(parent), _data(table), _graphs()
{
  setWindowTitle("Plot-o-mat");

  // Assemble view:
  _plot = new Plot::Figure();
  _plotview = new Plot::Canvas();
  _plotview->setMinimumSize(320,240);
  _plotview->setPlot(_plot);
  _plotview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
  _graph_list = new QListView();
  _graph_list->setMaximumWidth(160);
  _graph_list->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
  _graph_list->setModel(&_graphs);
  _add_graph  = new QPushButton(tr("+"));
  _rem_graph  = new QPushButton(tr("-"));

  _stack = new QStackedWidget();
  _stack->addWidget(_plotview);
  _stack->addWidget(new QLabel(tr("Add a graph to the plot by pressing '+'")));
  _stack->setCurrentIndex(1);

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

  QVBoxLayout *layout = new QVBoxLayout();
  QHBoxLayout *main_layout = new QHBoxLayout();
  QVBoxLayout *side_box = new QVBoxLayout();
  QHBoxLayout *button_box = new QHBoxLayout();

  button_box->setMargin(0);
  button_box->addWidget(_add_graph);
  button_box->addWidget(_rem_graph);
  side_box->setMargin(0); side_box->setSpacing(0);
  side_box->addWidget(_graph_list);
  side_box->addLayout(button_box);
  main_layout->addWidget(_stack);
  main_layout->addLayout(side_box);
  layout->addLayout(main_layout);
  layout->addWidget(buttons);
  setLayout(layout);

  QObject::connect(_add_graph, SIGNAL(clicked()), this, SLOT(onAddGraph()));
  QObject::connect(_rem_graph, SIGNAL(clicked()), this, SLOT(onRemoveGraph()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(onAccepted()));
}


size_t
TimeSeriesPlotDialog::numGraphs() const {
  return _graphs.rowCount(QModelIndex());
}

TimeSeriesGraphConfig &
TimeSeriesPlotDialog::graph(size_t i) {
  return _graphs.graph(i);
}


void
TimeSeriesPlotDialog::onAddGraph() {
  NewTimeSeriesGraphDialog add_graph_dialog(_data);
  if (QDialog::Rejected == add_graph_dialog.exec()) { return; }
  _graphs.addGraph(add_graph_dialog.getConfig());
  _stack->setCurrentIndex(0);
  onUpdatePlot();
}

void
TimeSeriesPlotDialog::onRemoveGraph() {
  if (! _graph_list->selectionModel()->hasSelection()) { return; }
  QModelIndexList selected_items =_graph_list->selectionModel()->selectedIndexes();
  if (1 != selected_items.count()) { return; }

  _graphs.removeGraph(selected_items.at(0).row());
  if (0 == _graphs.rowCount(QModelIndex())) { _stack->setCurrentIndex(1); return; }
  onUpdatePlot();
}

void
TimeSeriesPlotDialog::onUpdatePlot()
{
  // Free old plot
  if (0 != _plot) { delete _plot; }

  // Assemble new plot
  _plot = new Plot::Figure();
  for (int i=0; i<_graphs.rowCount(QModelIndex()); i++) {
    Plot::Graph *graph = _graphs.graph(i).create(_plot->getStyle(i));
    _plot->getAxis()->addGraph(graph);
    _plot->addToLegend(_graphs.graph(i).label(), graph);
  }

  // Add plot and update figure
  _plotview->setPlot(_plot);
  _plot->updateAxes();
  _plotview->update();
}

void
TimeSeriesPlotDialog::onAccepted() {
  if (0 == _graphs.rowCount(QModelIndex())) { return; }
  accept();
}



/* ******************************************************************************************** *
 * Implementation of TimeSeriesFormulaEditor
 * ******************************************************************************************** */
TimeSeriesFormulaEditor::TimeSeriesFormulaEditor(Table *table, QWidget *parent)
  : QWidget(parent), _table(table)
{
  _formula = new QLineEdit("");
  QStringList columns;
  for (size_t i=0; i<_table->getNumColumns(); i++) { columns.append(_table->getColumnName(i)); }
  _columns = new QCompleter(columns);
  _columns->setWidget(_formula);
  _columns->setCompletionMode(QCompleter::PopupCompletion);
  _select_column = new QPushButton("+");

  QHBoxLayout *layout = new QHBoxLayout();
  layout->setMargin(0); layout->setSpacing(0);
  layout->addWidget(_formula);
  layout->addWidget(_select_column);
  setLayout(layout);

  QObject::connect(_select_column, SIGNAL(clicked()), this, SLOT(showPopUp()));
  QObject::connect(_columns, SIGNAL(activated(QModelIndex)), this, SLOT(onColumnSelected(QModelIndex)));
}


void
TimeSeriesFormulaEditor::onColumnSelected(QModelIndex index) {
  QAbstractProxyModel *proxy = static_cast<QAbstractProxyModel *>(_columns->completionModel());
  index = proxy->mapToSource(index);
  QString formula = _formula->text();
  formula.append(QString("$%1").arg(index.row()));
  _formula->setText(formula);
}

void
TimeSeriesFormulaEditor::showPopUp() {
  _columns->setCompletionPrefix("");
  _columns->complete();
}


/* ******************************************************************************************** *
 * Implementation of NewTimeSeriesGraphDialog
 * ******************************************************************************************** */
NewTimeSeriesGraphDialog::NewTimeSeriesGraphDialog(Table *table, QWidget *parent)
  : QDialog(parent), _data(table), _config(table, TimeSeriesGraphConfig::LINE_GRAPH, 0, 0)
{
  setWindowTitle("Graph-o-mat");

  _plot_type    = new QComboBox();
  _plot_type->addItem(tr("Line plot"));
  _plot_type->addItem(tr("Variance plot"));
  _plot_type->setCurrentIndex(0);

  _label = new QLineEdit("graph");
  _formula_mean = new TimeSeriesFormulaEditor(_data);
  _formula_var  = new TimeSeriesFormulaEditor(_data);
  _formula_var->setEnabled(false);

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  QFormLayout *grid = new QFormLayout();
  grid->addRow(tr("Plot type"), _plot_type);
  grid->addRow(tr("Label"), _label);
  grid->addRow(tr("x(t)="), _formula_mean);
  grid->addRow(tr("var(x(t))="), _formula_var);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(grid);
  layout->addWidget(buttons);

  setLayout(layout);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(checkInputAndExit()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(_plot_type, SIGNAL(currentIndexChanged(int)), this, SLOT(onPlotTypeSelect(int)));
}


void
NewTimeSeriesGraphDialog::onPlotTypeSelect(int index)
{
  if (0 == index) {
    _formula_var->setEnabled(false);
    _config.setPlotType(TimeSeriesGraphConfig::LINE_GRAPH);
  } else {
    _formula_var->setEnabled(true);
    _config.setPlotType(TimeSeriesGraphConfig::VARIANCE_GRAPH);
  }
}


void
NewTimeSeriesGraphDialog::checkInputAndExit()
{
  // Configure plot label
  _config.setLabel(_label->text());

  // Get mean formular
  if (! _config.setMeanColumn(_formula_mean->getFormula())) { return; }

  // Get var formula if needed.
  if (TimeSeriesGraphConfig::VARIANCE_GRAPH == _config.plotType()) {
    if (! _config.setVarColumn(_formula_mean->getFormula())) { return; }
  }

  accept();
}
