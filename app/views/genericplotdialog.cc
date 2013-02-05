#include "genericplotdialog.hh"
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
GenericGraphConfig::GenericGraphConfig(
    Table *table, PlotType type, const QString &label, size_t mean_column, size_t var_column)
  : _table(table), _context(table), _type(type), _y_expression(mean_column), _var_expression(var_column), _label(label)
{
  // pass...
}

GenericGraphConfig::GenericGraphConfig(const GenericGraphConfig &other)
  : _table(other._table), _context(other._context), _type(other._type),
    _x_expression(other._x_expression), _y_expression(other._y_expression),
    _var_expression(other._var_expression), _label(other._label)
{
  // pass...
}


GenericGraphConfig::PlotType
GenericGraphConfig::plotType() const {
  return _type;
}

void
GenericGraphConfig::setPlotType(PlotType type) {
  _type = type;
}


GiNaC::ex
GenericGraphConfig::xColumn() const {
  return _x_expression;
}

QString
GenericGraphConfig::xColumnString() {
  std::stringstream buffer;
  PlotFormulaParser::serialize(_x_expression, buffer, _context);
  return QString(buffer.str().c_str());
}

bool
GenericGraphConfig::setXColumn(const QString &formula) {
  if (! PlotFormulaParser::check(formula, _context)) { return false; }
  _x_expression = PlotFormulaParser::parse(formula, _context);
  return true;
}


GiNaC::ex
GenericGraphConfig::meanYColumn() const {
  return _y_expression;
}

QString
GenericGraphConfig::meanYColumnString() {
  std::stringstream buffer;
  PlotFormulaParser::serialize(_y_expression, buffer, _context);
  return QString(buffer.str().c_str());
}

bool
GenericGraphConfig::setMeanYColumn(const QString &formula) {
  if (! PlotFormulaParser::check(formula, _context)) { return false; }
  _y_expression = PlotFormulaParser::parse(formula, _context);
  return true;
}

GiNaC::ex
GenericGraphConfig::varYColumn() const {
  return _var_expression;
}

QString
GenericGraphConfig::varYColumnString() {
  std::stringstream buffer;
  PlotFormulaParser::serialize(_var_expression, buffer, _context);
  return QString(buffer.str().c_str());
}

bool
GenericGraphConfig::setVarYColumn(const QString &formula) {
  if (! PlotFormulaParser::check(formula, _context)) { return false; }
  _var_expression = PlotFormulaParser::parse(formula, _context);
  return true;
}

const QString &
GenericGraphConfig::label() const {
  return _label;
}

void
GenericGraphConfig::setLabel(const QString &label) {
  _label = label;
}

Table *
GenericGraphConfig::table() {
  return _table;
}

Plot::Graph *
GenericGraphConfig::create(const Plot::GraphStyle &style)
{
  if (LINE_GRAPH == _type) {
    Plot::LineGraph *graph = new Plot::LineGraph(style);
    for (size_t i=0; i<_table->getNumRows(); i++) {
      graph->addPoint(_evalX(i), _evalY(i));
    }
    return graph;
  }

  Plot::VarianceLineGraph *graph = new Plot::VarianceLineGraph(style);
  for (size_t i=0; i<_table->getNumRows(); i++) {
    graph->addPoint(_evalX(i), _evalY(i), std::sqrt(evalYVar(i)));
  }
  return graph;
}

double
GenericGraphConfig::_evalX(size_t row) {
  return _context(row, _x_expression);
}

double
GenericGraphConfig::_evalY(size_t row) {
  return _context(row, _y_expression);
}

double
GenericGraphConfig::evalYVar(size_t row) {
  return _context(row, _var_expression);
}



/* ******************************************************************************************** *
 * Implementation of TimeSeriesGraphList
 * ******************************************************************************************** */
GenericGraphList::GenericGraphList(QObject *parent)
  : QAbstractListModel(parent), _graphs()
{
  // pass..
}

int
GenericGraphList::rowCount(const QModelIndex &parent) const {
  return _graphs.size();
}

QVariant
GenericGraphList::data(const QModelIndex &index, int role) const {
  if (Qt::DisplayRole != role) { return QVariant(); }
  if (index.row() >= _graphs.size()) { return QVariant(); }
  return _graphs.at(index.row()).label();
}

void
GenericGraphList::addGraph(const GenericGraphConfig &graph) {
  beginInsertRows(QModelIndex(), _graphs.size(), _graphs.size());
  _graphs.append(graph);
  endInsertRows();
}

GenericGraphConfig &
GenericGraphList::graph(int idx) {
  return _graphs[idx];
}

void
GenericGraphList::removeGraph(int idx) {
  if (idx < 0 || idx >= _graphs.size()) { return; }
  beginRemoveRows(QModelIndex(), idx, idx);
  _graphs.removeAt(idx);
  endRemoveRows();
}

void
GenericGraphList::updateGraph(int idx, const GenericGraphConfig &graph) {
  if (idx < 0 || idx >= _graphs.size()) { return; }
  _graphs.replace(idx, graph);
  emit dataChanged(index(idx), index(idx));
}


/* ******************************************************************************************** *
 * Implementation of TimeSeriesPlotDialog
 * ******************************************************************************************** */
GenericPlotDialog::GenericPlotDialog(Table *table, QWidget *parent)
  : QDialog(parent), _data(table), _graphs(), _figure_title("New Plot"), _x_label(), _y_label()
{
  setWindowTitle("Plot-o-mat");

  // Assemble view:
  _plot = new Plot::Figure();
  _plotview = new Plot::Canvas();
  _plotview->setMinimumSize(320,240);
  _plotview->setPlot(_plot);
  _plotview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  _graph_list = new QListView();
  //_graph_list->setMaximumWidth(160);
  _graph_list->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  _graph_list->setModel(&_graphs);

  _add_graph  = new QPushButton(tr("+"));
  _add_graph->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  _rem_graph  = new QPushButton(tr("-"));
  _rem_graph->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  _edit_labels = new QPushButton(tr("Edit labels"));
  _edit_labels->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

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
  button_box->addWidget(_edit_labels);
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
  QObject::connect(_edit_labels, SIGNAL(clicked()), this, SLOT(onEditLabels()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(onAccepted()));
  QObject::connect(_graph_list, SIGNAL(doubleClicked(QModelIndex)),
                   this, SLOT(onEditGraph(QModelIndex)));
}


size_t
GenericPlotDialog::numGraphs() const {
  return _graphs.rowCount(QModelIndex());
}

GenericGraphConfig &
GenericPlotDialog::graph(size_t i) {
  return _graphs.graph(i);
}

QString
GenericPlotDialog::figureTitle() const {
  return _figure_title;
}

QString
GenericPlotDialog::xLabel() const {
  return _x_label;
}

QString
GenericPlotDialog::yLabel() const {
  return _y_label;
}


void
GenericPlotDialog::onAddGraph() {
  GenericGraphDialog add_graph_dialog(_data);
  if (QDialog::Rejected == add_graph_dialog.exec()) { return; }
  _graphs.addGraph(add_graph_dialog.getConfig());
  _stack->setCurrentIndex(0);
  onUpdatePlot();
}


void
GenericPlotDialog::onEditLabels() {
  GenericPlotLabelDialog label_dialog(_figure_title, _x_label, _y_label);
  if (QDialog::Accepted != label_dialog.exec()) { return; }
  _figure_title = label_dialog.figureTitle();
  _x_label = label_dialog.xLabel();
  _y_label = label_dialog.yLabel();
  onUpdatePlot();
}


void
GenericPlotDialog::onEditGraph(const QModelIndex &index) {
  // get graph config and edit it:
  GenericGraphDialog graph_dialog(_graphs.graph(index.row()));
  if (QDialog::Rejected == graph_dialog.exec()) { return; }
  _graphs.updateGraph(index.row(), graph_dialog.getConfig());
  onUpdatePlot();
}

void
GenericPlotDialog::onRemoveGraph() {
  if (! _graph_list->selectionModel()->hasSelection()) { return; }
  QModelIndexList selected_items =_graph_list->selectionModel()->selectedIndexes();
  if (1 != selected_items.count()) { return; }

  _graphs.removeGraph(selected_items.at(0).row());
  if (0 == _graphs.rowCount(QModelIndex())) { _stack->setCurrentIndex(1); return; }
  onUpdatePlot();
}

void
GenericPlotDialog::onUpdatePlot()
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

  // Set labels:
  _plot->setTitle(_figure_title);
  _plot->setXLabel(_x_label);
  _plot->setYLabel(_y_label);

  // Add plot and update figure
  _plotview->setPlot(_plot);
  _plot->updateAxes();
  _plotview->update();
}

void
GenericPlotDialog::onAccepted() {
  if (0 == _graphs.rowCount(QModelIndex())) { return; }
  accept();
}



/* ******************************************************************************************** *
 * Implementation of TimeSeriesFormulaEditor
 * ******************************************************************************************** */
GenericPlotFormulaEditor::GenericPlotFormulaEditor(Table *table, QWidget *parent)
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
GenericPlotFormulaEditor::onColumnSelected(QModelIndex index) {
  QAbstractProxyModel *proxy = static_cast<QAbstractProxyModel *>(_columns->completionModel());
  index = proxy->mapToSource(index);
  QString formula = _formula->text();
  int pos = _formula->cursorPosition();
  formula.insert(pos, QString("$%1").arg(index.row()));
  _formula->setText(formula);
}

void
GenericPlotFormulaEditor::showPopUp() {
  _columns->setCompletionPrefix("");
  _columns->complete();
}



/* ******************************************************************************************** *
 * Implementation of TimeSeriesGraphDialog
 * ******************************************************************************************** */
GenericGraphDialog::GenericGraphDialog(Table *table, QWidget *parent)
  : QDialog(parent), _data(table), _config(table, GenericGraphConfig::LINE_GRAPH, "graph", 0, 0)
{
  // Setup GUI
  __initGUI();
}

GenericGraphDialog::GenericGraphDialog(GenericGraphConfig &config, QWidget *parent)
  : QDialog(parent), _data(config.table()), _config(config)
{
  // Setup GUI
  __initGUI();
}


void
GenericGraphDialog::__initGUI()
{
  setWindowTitle("Graph-o-mat");

  _plot_type    = new QComboBox();
  _plot_type->addItem(tr("Line plot"));
  _plot_type->addItem(tr("Variance plot"));
  if (GenericGraphConfig::LINE_GRAPH == _config.plotType()) {
    _plot_type->setCurrentIndex(0);
  } else {
    _plot_type->setCurrentIndex(1);
  }

  _label = new QLineEdit(_config.label());
  _formula_x = new GenericPlotFormulaEditor(_data);
  if ("0" == _config.xColumnString()) { _formula_x->setFormula("$0"); }
  else { _formula_x->setFormula(_config.xColumnString()); }
  _formula_y = new GenericPlotFormulaEditor(_data);
  _formula_y->setFormula(_config.meanYColumnString());
  _formula_var_y  = new GenericPlotFormulaEditor(_data);
  if (GenericGraphConfig::VARIANCE_GRAPH == _config.plotType()) {
    _formula_var_y->setFormula(_config.varYColumnString());
    _formula_var_y->setEnabled(true);
  } else {
    _formula_var_y->setEnabled(false);
  }

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  QFormLayout *grid = new QFormLayout();
  grid->addRow(tr("Plot type"), _plot_type);
  grid->addRow(tr("Label"), _label);
  grid->addRow(tr("X="), _formula_x);
  grid->addRow(tr("Y="), _formula_y);
  grid->addRow(tr("var(Y)="), _formula_var_y);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(grid);
  layout->addWidget(buttons);

  setLayout(layout);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(checkInputAndExit()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(_plot_type, SIGNAL(currentIndexChanged(int)), this, SLOT(onPlotTypeSelect(int)));
}


void
GenericGraphDialog::onPlotTypeSelect(int index)
{
  if (0 == index) {
    _formula_var_y->setEnabled(false);
    _config.setPlotType(GenericGraphConfig::LINE_GRAPH);
  } else {
    _formula_var_y->setEnabled(true);
    _config.setPlotType(GenericGraphConfig::VARIANCE_GRAPH);
  }
}


void
GenericGraphDialog::checkInputAndExit()
{
  // Configure plot label
  _config.setLabel(_label->text());

  // Get X formula
  if (! _config.setXColumn(_formula_x->getFormula())) { return; }

  // Get Y formula
  if (! _config.setMeanYColumn(_formula_y->getFormula())) { return; }

  // Get var formula if needed.
  if (GenericGraphConfig::VARIANCE_GRAPH == _config.plotType()) {
    if (! _config.setVarYColumn(_formula_var_y->getFormula())) { return; }
  }

  accept();
}



/* ******************************************************************************************** *
 * Implementation of TimeSeriesLabelDialog
 * ******************************************************************************************** */
GenericPlotLabelDialog::GenericPlotLabelDialog(
    const QString &title, const QString &x_label, const QString &y_label, QWidget *parent)
  : QDialog(parent)
{
  setWindowTitle("Plot label");

  _figureTitle = new QLineEdit(title);
  _xLabel = new QLineEdit(x_label);
  _yLabel = new QLineEdit(y_label);

  QFormLayout *label_layout = new QFormLayout();
  label_layout->addRow(tr("Title"), _figureTitle);
  label_layout->addRow(tr("X Label"), _xLabel);
  label_layout->addRow(tr("Y Label"), _yLabel);

  QDialogButtonBox *button_box = new QDialogButtonBox(
        QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(label_layout);
  layout->addWidget(button_box);
  setLayout(layout);

  QObject::connect(button_box, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(button_box, SIGNAL(rejected()), this, SLOT(reject()));
}


QString
GenericPlotLabelDialog::figureTitle() const  {
  return _figureTitle->text();
}

QString
GenericPlotLabelDialog::xLabel() const {
  return _xLabel->text();
}

QString
GenericPlotLabelDialog::yLabel() const {
  return _yLabel->text();
}
