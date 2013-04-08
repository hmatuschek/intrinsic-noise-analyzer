#include "plotconfigdialog.hh"
#include "configuration.hh"
#include "canvas.hh"
#include "figure.hh"

#include <QDialogButtonBox>
#include <QAbstractProxyModel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QCompleter>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

using namespace Plot;


/* ****************************************************************************************** *
 * Implementation of GraphConfigList
 * ****************************************************************************************** */
GraphConfigList::GraphConfigList(PlotConfig *config, QObject *parent)
  : QAbstractListModel(parent), _config(config)
{
  // pass...
}

int
GraphConfigList::rowCount(const QModelIndex &parent) const {
  return _config->numGraphs();
}

QVariant
GraphConfigList::data(const QModelIndex &index, int role) const {
  if (int(Qt::DisplayRole) != role) { return QVariant(); }
  if (index.row() >= int(_config->numGraphs())) { return QVariant(); }
  return _config->graph(index.row())->label();
}

AbstractGraphConfig *
GraphConfigList::graph(int idx) {
  if (idx >= int(_config->numGraphs())) { return 0; }
  return _config->graph(idx);
}

void
GraphConfigList::addGraph(AbstractGraphConfig *graph) {
  _config->addGraph(graph);
}

void
GraphConfigList::removeGraph(int idx) {
  _config->removeGraph(idx);
}

void
GraphConfigList::updateGraph(int idx, AbstractGraphConfig *graph) {
  _config->replaceGraph(idx, graph);
}



/* ****************************************************************************************** *
 * Implementation of PlotConfigDialog
 * ****************************************************************************************** */
PlotConfigDialog::PlotConfigDialog(Table *table, PlotConfig *config, QWidget *parent)
  : QDialog(parent), _data(table), _config(config), _graph_list(_config)
{
  setWindowTitle("Plot-o-mat");

  // Assemble view:
  _plot = new Plot::Figure();
  _plotview = new Plot::Canvas();
  _plotview->setMinimumSize(320,240);
  _plotview->setPlot(_plot);
  _plotview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  _graph_list_view = new QListView();
  _graph_list_view->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
  _graph_list_view->setModel(&_graph_list);

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
  side_box->addWidget(_graph_list_view);
  side_box->addLayout(button_box);
  main_layout->addWidget(_stack);
  main_layout->addLayout(side_box);
  layout->addLayout(main_layout);
  layout->addWidget(buttons);
  setLayout(layout);

  QObject::connect(_add_graph, SIGNAL(clicked()), this, SLOT(onAddLineGraph()));
  QObject::connect(_rem_graph, SIGNAL(clicked()), this, SLOT(onRemoveGraph()));
  QObject::connect(_edit_labels, SIGNAL(clicked()), this, SLOT(onEditLabels()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(onAccepted()));
  QObject::connect(_graph_list_view, SIGNAL(doubleClicked(QModelIndex)),
                   this, SLOT(onEditGraph(QModelIndex)));
}


size_t
PlotConfigDialog::numGraphs() const {
  return _graph_list.rowCount(QModelIndex());
}

AbstractGraphConfig *
PlotConfigDialog::graph(size_t i) {
  return _graph_list.graph(i);
}

const QString &
PlotConfigDialog::figureTitle() const {
  return _config->title();
}

const QString &
PlotConfigDialog::xLabel() const {
  return _config->xLabel();
}

const QString &
PlotConfigDialog::yLabel() const {
  return _config->yLabel();
}


void
PlotConfigDialog::onAddLineGraph() {
  LineGraphConfig *graph_config = new LineGraphConfig(*_data, numGraphs());
  LineGraphDialog add_graph_dialog(graph_config);
  if (QDialog::Rejected == add_graph_dialog.exec()) { delete graph_config; return; }
  _config->addGraph(graph_config);
  _stack->setCurrentIndex(0);
  onUpdatePlot();
}

void
PlotConfigDialog::onAddVarLineGraph() {
  VarianceLineGraphConfig *graph_config = new VarianceLineGraphConfig(*_data, numGraphs());
  VarianceLineGraphDialog add_graph_dialog(graph_config);
  if (QDialog::Rejected == add_graph_dialog.exec()) { delete graph_config; return; }
  _config->addGraph(graph_config);
  _stack->setCurrentIndex(0);
  onUpdatePlot();
}

void
PlotConfigDialog::onEditLabels() {
  /*PlotConfigLabelDialog label_dialog(_config);
  if (QDialog::Accepted != label_dialog.exec()) { return; }
  onUpdatePlot();*/
}

void
PlotConfigDialog::onEditGraph(const QModelIndex &index) {
  // get graph config and edit it:
  AbstractGraphConfig *graph_config = _config->graph(index.row());
  // Dispatch by type:
  if (LineGraphConfig *line_config = dynamic_cast<LineGraphConfig *>(graph_config)) {
    LineGraphDialog graph_dialog(line_config);
    if (QDialog::Rejected == graph_dialog.exec()) { return; }
  } else if (VarianceLineGraphConfig *var_config = dynamic_cast<VarianceLineGraphConfig *>(graph_config)) {
    VarianceLineGraphDialog graph_dialog(var_config);
    if (QDialog::Rejected == graph_dialog.exec()) { return; }
  }
  onUpdatePlot();
}

void
PlotConfigDialog::onRemoveGraph() {
  if (! _graph_list_view->selectionModel()->hasSelection()) { return; }
  QModelIndexList selected_items =_graph_list_view->selectionModel()->selectedIndexes();
  if (1 != selected_items.count()) { return; }

  _config->removeGraph(selected_items.at(0).row());
  if (0 == _graph_list.rowCount(QModelIndex())) { _stack->setCurrentIndex(1); return; }
  onUpdatePlot();
}

void
PlotConfigDialog::onUpdatePlot()
{
  // Free old plot
  if (0 != _plot) { delete _plot; }

  // Assemble new plot & add plot
  _plot = _config->createFigure();
  _plotview->setPlot(_plot);
  _plot->updateAxes();
  _plotview->update();
}

void
PlotConfigDialog::onAccepted() {
  if (0 == _graph_list.rowCount(QModelIndex())) { return; }
  accept();
}


/* ******************************************************************************************** *
 * Implementation of PlotFormulaEditor
 * ******************************************************************************************** */
LinePlotFormulaEditor::LinePlotFormulaEditor(LineGraphConfig *config, QWidget *parent)
  : QWidget(parent), _config(config)
{
  _formula = new QLineEdit("");
  _columns = new QCompleter(_config->columnNames());
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

QString
LinePlotFormulaEditor::getFormula() const {
  return _formula->text();
}

void
LinePlotFormulaEditor::setFormula(const QString &formula) const {
  return _formula->setText(formula);
}

void
LinePlotFormulaEditor::onColumnSelected(QModelIndex index) {
  QAbstractProxyModel *proxy = static_cast<QAbstractProxyModel *>(_columns->completionModel());
  index = proxy->mapToSource(index);
  QString formula = _formula->text();
  int pos = _formula->cursorPosition();
  formula.insert(pos, QString("$%1").arg(index.row()));
  _formula->setText(formula);
}

void
LinePlotFormulaEditor::showPopUp() {
  _columns->setCompletionPrefix("");
  _columns->complete();
}



/* ******************************************************************************************** *
 * Implementation of LineGraphDialog
 * ******************************************************************************************** */
LineGraphDialog::LineGraphDialog(LineGraphConfig *config, QWidget *parent)
  : QDialog(parent), _config(config)
{
  // Setup GUI
  __initGUI();
}


void
LineGraphDialog::__initGUI()
{
  setWindowTitle("Graph-o-mat");

  _label = new QLineEdit(_config->label());
  _formula_x = new LinePlotFormulaEditor(_config);
  _formula_x->setFormula(_config->xExpression());
  _formula_y = new LinePlotFormulaEditor(_config);
  _formula_y->setFormula(_config->yExpression());

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  QFormLayout *grid = new QFormLayout();
  grid->addRow(tr("Label"), _label);
  grid->addRow(tr("X="), _formula_x);
  grid->addRow(tr("Y="), _formula_y);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(grid);
  layout->addWidget(buttons);

  setLayout(layout);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(checkInputAndExit()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void
LineGraphDialog::checkInputAndExit()
{
  // Configure plot label
  _config->setLabel(_label->text());

  // Check formulas:
  if (! _config->checkExpression(_formula_x->getFormula())) { return; }
  if (! _config->checkExpression(_formula_y->getFormula())) { return; }

  // Set formulas:
  _config->setXExpression(_formula_x->getFormula());
  _config->setYExpression(_formula_y->getFormula());

  accept();
}



/* ******************************************************************************************** *
 * Implementation of VarianceLineGraphDialog
 * ******************************************************************************************** */
VarianceLineGraphDialog::VarianceLineGraphDialog(VarianceLineGraphConfig *config, QWidget *parent)
  : QDialog(parent), _config(config)
{
  // Setup GUI
  __initGUI();
}


void
VarianceLineGraphDialog::__initGUI()
{
  setWindowTitle("Graph-o-mat");

  _label = new QLineEdit(_config->label());
  _formula_x = new LinePlotFormulaEditor(_config);
  _formula_x->setFormula(_config->xExpression());
  _formula_y = new LinePlotFormulaEditor(_config);
  _formula_y->setFormula(_config->yExpression());
  _formula_var = new LinePlotFormulaEditor(_config);
  _formula_var->setFormula(_config->varExpression());

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  QFormLayout *grid = new QFormLayout();
  grid->addRow(tr("Label"), _label);
  grid->addRow(tr("X="), _formula_x);
  grid->addRow(tr("Y="), _formula_y);
  grid->addRow(tr("var(Y)="), _formula_var);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(grid);
  layout->addWidget(buttons);

  setLayout(layout);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(checkInputAndExit()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}

void
VarianceLineGraphDialog::checkInputAndExit()
{
  // Configure plot label
  _config->setLabel(_label->text());

  // Check formulas:
  if (! _config->checkExpression(_formula_x->getFormula())) { return; }
  if (! _config->checkExpression(_formula_y->getFormula())) { return; }
  if (! _config->checkExpression(_formula_var->getFormula())) { return; }

  // Set formulas:
  _config->setXExpression(_formula_x->getFormula());
  _config->setYExpression(_formula_y->getFormula());
  _config->setVarExpression(_formula_var->getFormula());

  accept();
}
