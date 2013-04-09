#include "plotconfigdialog.hh"
#include "configuration.hh"
#include "canvas.hh"
#include "figure.hh"
#include "plotrangedialog.hh"

#include <QDialogButtonBox>
#include <QAbstractProxyModel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QCompleter>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMenu>
#include <QColorDialog>
#include <QTabWidget>
#include <QPaintEvent>

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
  beginInsertRows(QModelIndex(), _config->numGraphs(), _config->numGraphs());
  _config->addGraph(graph);
  endInsertRows();
}

void
GraphConfigList::removeGraph(int idx) {
  beginRemoveRows(QModelIndex(), idx, idx);
  _config->removeGraph(idx);
  endRemoveRows();
}

void
GraphConfigList::updateGraph(int idx, AbstractGraphConfig *graph) {
  beginResetModel();
  _config->replaceGraph(idx, graph);
  endResetModel();
}



/* ****************************************************************************************** *
 * Implementation of PlotConfigDialog
 * ****************************************************************************************** */
PlotConfigDialog::PlotConfigDialog(PlotConfig *config, QWidget *parent)
  : QDialog(parent), _config(config), _graph_list(_config), _replotTimer()
{
  setWindowTitle("Plot-o-mat");

  // Configure replot-timer:
  _replotTimer.setSingleShot(true);

  // Assemble preview:
  Plot::Figure *plot = _config->createFigure();
  _plotview = new Plot::Canvas();
  _plotview->setMinimumSize(320,240);
  _plotview->setPlot(plot);
  _plotview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

  _tabwidget = new QTabWidget();

  { // Assemble "Graphs" page:
    _graph_list_view = new QListView();
    _graph_list_view->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    _graph_list_view->setModel(&_graph_list);

    QPushButton *add_graph  = new QPushButton(tr("+"));
    add_graph->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    QMenu *add_graph_menu = new QMenu();
    QAction *add_line_graph = add_graph_menu->addAction(tr("Add line graph"));
    QAction *add_var_graph = add_graph_menu->addAction(tr("Add variance graph"));
    add_graph->setMenu(add_graph_menu);

    QPushButton *rem_graph  = new QPushButton(tr("-"));
    rem_graph->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    QVBoxLayout *side_box = new QVBoxLayout();
    QHBoxLayout *button_box = new QHBoxLayout();
    button_box->setMargin(0);
    button_box->addWidget(add_graph);
    button_box->addWidget(rem_graph);
    side_box->setMargin(0); side_box->setSpacing(0);
    side_box->addWidget(_graph_list_view);
    side_box->addLayout(button_box);

    QObject::connect(add_line_graph, SIGNAL(triggered()), this, SLOT(onAddLineGraph()));
    QObject::connect(add_var_graph, SIGNAL(triggered()), this, SLOT(onAddVarLineGraph()));
    QObject::connect(rem_graph, SIGNAL(clicked()), this, SLOT(onRemoveGraph()));

    QWidget *graph_tab = new QWidget(); graph_tab->setLayout(side_box);
    _tabwidget->addTab(graph_tab, tr("Graphs"));
  }

  { // Assemble Lables page:
    QFormLayout *layout = new QFormLayout();

    QLineEdit *title = new QLineEdit(_config->title());
    layout->addRow(tr("Title"), title);
    QLineEdit *xlabel = new QLineEdit(_config->xLabel());
    layout->addRow(tr("X Label"), xlabel);
    QLineEdit *ylabel = new QLineEdit(_config->yLabel());
    layout->addRow(tr("Y Label"), ylabel);

    QHBoxLayout *min_xrange_layout = new QHBoxLayout();
    QCheckBox *min_xrange_policy = new QCheckBox(); _minXRange  = new QLineEdit();
    if (RangePolicy::FIXED == _config->xRangePolicy().getMinPolicy()) {
      min_xrange_policy->setChecked(true); _minXRange->setEnabled(true);
    } else {
      min_xrange_policy->setChecked(false); _minXRange->setEnabled(false);
    }
    _minXRange->setValidator(new QDoubleValidator());
    _minXRange->setText(QString("%1").arg(_config->xRange().min()));
    min_xrange_layout->addWidget(min_xrange_policy);
    min_xrange_layout->addWidget(_minXRange);
    layout->addRow(tr("min. X"), min_xrange_layout);

    QHBoxLayout *max_xrange_layout = new QHBoxLayout();
    QCheckBox *max_xrange_policy = new QCheckBox(); _maxXRange  = new QLineEdit();
    if (RangePolicy::FIXED == _config->xRangePolicy().getMaxPolicy()) {
      max_xrange_policy->setChecked(true); _maxXRange->setEnabled(true);
    } else {
      max_xrange_policy->setChecked(false); _maxXRange->setEnabled(false);
    }
    _maxXRange->setValidator(new QDoubleValidator());
    _maxXRange->setText(QString("%1").arg(_config->xRange().max()));
    max_xrange_layout->addWidget(max_xrange_policy);
    max_xrange_layout->addWidget(_maxXRange);
    layout->addRow(tr("max. X"), max_xrange_layout);

    QHBoxLayout *min_yrange_layout = new QHBoxLayout();
    QCheckBox *min_yrange_policy = new QCheckBox(); _minYRange  = new QLineEdit();
    if (RangePolicy::FIXED == _config->yRangePolicy().getMinPolicy()) {
      min_yrange_policy->setChecked(true); _minYRange->setEnabled(true);
    } else {
      min_yrange_policy->setChecked(false); _minYRange->setEnabled(false);
    }
    _minYRange->setValidator(new QDoubleValidator());
    _minYRange->setText(QString("%1").arg(_config->yRange().min()));
    min_yrange_layout->addWidget(min_yrange_policy);
    min_yrange_layout->addWidget(_minYRange);
    layout->addRow(tr("min. Y"), min_yrange_layout);

    QHBoxLayout *max_yrange_layout = new QHBoxLayout();
    QCheckBox *max_yrange_policy = new QCheckBox(); _maxYRange  = new QLineEdit();
    if (RangePolicy::FIXED == _config->yRangePolicy().getMaxPolicy()) {
      max_yrange_policy->setChecked(true); _maxYRange->setEnabled(true);
    } else {
      max_yrange_policy->setChecked(false); _maxYRange->setEnabled(false);
    }
    _maxYRange->setValidator(new QDoubleValidator());
    _maxYRange->setText(QString("%1").arg(_config->yRange().max()));
    max_yrange_layout->addWidget(max_yrange_policy);
    max_yrange_layout->addWidget(_maxYRange);
    layout->addRow(tr("max. Y"), max_yrange_layout);

    QObject::connect(title, SIGNAL(textChanged(QString)), this, SLOT(onTitleChanged(QString)));
    QObject::connect(xlabel, SIGNAL(textChanged(QString)), this, SLOT(onXLabelChanged(QString)));
    QObject::connect(ylabel, SIGNAL(textChanged(QString)), this, SLOT(onYLabelChanged(QString)));
    QObject::connect(min_xrange_policy, SIGNAL(toggled(bool)), this, SLOT(onMinXRangePolicyChanged(bool)));
    QObject::connect(_minXRange, SIGNAL(textChanged(QString)), this, SLOT(onMinXRangeChanged(QString)));
    QObject::connect(max_xrange_policy, SIGNAL(toggled(bool)), this, SLOT(onMaxXRangePolicyChanged(bool)));
    QObject::connect(_maxXRange, SIGNAL(textChanged(QString)), this, SLOT(onMaxXRangeChanged(QString)));
    QObject::connect(min_yrange_policy, SIGNAL(toggled(bool)), this, SLOT(onMinYRangePolicyChanged(bool)));
    QObject::connect(_minYRange, SIGNAL(textChanged(QString)), this, SLOT(onMinYRangeChanged(QString)));
    QObject::connect(max_yrange_policy, SIGNAL(toggled(bool)), this, SLOT(onMaxYRangePolicyChanged(bool)));
    QObject::connect(_maxYRange, SIGNAL(textChanged(QString)), this, SLOT(onMaxYRangeChanged(QString)));

    QWidget *panel = new QWidget(); panel->setLayout(layout);
    _tabwidget->addTab(panel, tr("Label/Ranges"));
  }

  _stack = new QStackedWidget();
  _stack->addWidget(_plotview);
  _stack->addWidget(new QLabel(tr("Add a graph to the plot by pressing '+'")));
  if (0 == _config->numGraphs()) { _stack->setCurrentIndex(1); }
  else { _stack->setCurrentIndex(0); }

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

  QVBoxLayout *layout = new QVBoxLayout();
  QHBoxLayout *main_layout = new QHBoxLayout();

  main_layout->addWidget(_stack, 1);
  main_layout->addWidget(_tabwidget, 0);
  layout->addLayout(main_layout);
  layout->addWidget(buttons);
  setLayout(layout);

  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(onAccepted()));
  QObject::connect(_graph_list_view, SIGNAL(doubleClicked(QModelIndex)),
                   this, SLOT(onEditGraph(QModelIndex)));
  QObject::connect(&_replotTimer, SIGNAL(timeout()), this, SLOT(onUpdatePlot()));
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
  LineGraphConfig *graph_config = new LineGraphConfig(_config->data(), numGraphs());
  LineGraphDialog add_graph_dialog(graph_config);
  if (QDialog::Rejected == add_graph_dialog.exec()) { delete graph_config; return; }
  _graph_list.addGraph(graph_config);
  onUpdatePlot();
}

void
PlotConfigDialog::onAddVarLineGraph() {
  VarianceLineGraphConfig *graph_config = new VarianceLineGraphConfig(_config->data(), numGraphs());
  VarianceLineGraphDialog add_graph_dialog(graph_config);
  if (QDialog::Rejected == add_graph_dialog.exec()) { delete graph_config; return; }
  _graph_list.addGraph(graph_config);
  onUpdatePlot();
}

void
PlotConfigDialog::onEditGraph(const QModelIndex &index) {
  // get graph config and edit it:
  AbstractGraphConfig *graph_config = _config->graph(index.row());
  // Dispatch by type:
  if (VarianceLineGraphConfig *var_config = dynamic_cast<VarianceLineGraphConfig *>(graph_config)) {
    VarianceLineGraphDialog graph_dialog(var_config);
    if (QDialog::Rejected == graph_dialog.exec()) { return; }
  } else if (LineGraphConfig *line_config = dynamic_cast<LineGraphConfig *>(graph_config)) {
    LineGraphDialog graph_dialog(line_config);
    if (QDialog::Rejected == graph_dialog.exec()) { return; }
  }
  onUpdatePlot();
}

void
PlotConfigDialog::onRemoveGraph() {
  if (! _graph_list_view->selectionModel()->hasSelection()) { return; }
  QModelIndexList selected_items =_graph_list_view->selectionModel()->selectedIndexes();
  if (1 != selected_items.count()) { return; }

  _graph_list.removeGraph(selected_items.at(0).row());
  onUpdatePlot();
}

void
PlotConfigDialog::onUpdatePlot()
{
  // Assemble new plot & add plot
  Plot::Figure *plot = _config->createFigure();
  _plotview->setPlot(plot);
  plot->updateAxes();
  plot->updateAxes();
  _plotview->update();

  if (0 == _graph_list.rowCount(QModelIndex())) { _stack->setCurrentIndex(1); }
  else { _stack->setCurrentIndex(0); }
}

void
PlotConfigDialog::onTitleChanged(QString title) {
  _config->setTile(title);
  _replotTimer.start(2000);
}

void
PlotConfigDialog::onXLabelChanged(QString label) {
  _config->setXLabel(label);
  _replotTimer.start(2000);
}

void
PlotConfigDialog::onYLabelChanged(QString label) {
  _config->setYLabel(label);
  _replotTimer.start(2000);
}

void
PlotConfigDialog::onMinXRangePolicyChanged(bool fixed) {
  RangePolicy policy = _config->xRangePolicy();
  if (fixed) { policy.setMinPolicy(RangePolicy::FIXED); _minXRange->setEnabled(true); }
  else { policy.setMinPolicy(RangePolicy::AUTOMATIC); _minXRange->setEnabled(false); }
  _config->setXRangePolicy(policy);
  _replotTimer.start(2000);
}

void
PlotConfigDialog::onMinXRangeChanged(QString value) {
  Range range = _config->xRange();
  range.setMin(value.toDouble());
  _config->setXRange(range);
  _replotTimer.start(2000);
}

void
PlotConfigDialog::onMaxXRangeChanged(QString value) {
  Range range = _config->xRange();
  range.setMax(value.toDouble());
  _config->setXRange(range);
  _replotTimer.start(2000);
}

void
PlotConfigDialog::onMinYRangeChanged(QString value) {
  Range range = _config->yRange();
  range.setMin(value.toDouble());
  _config->setYRange(range);
  _replotTimer.start(2000);
}

void
PlotConfigDialog::onMaxYRangeChanged(QString value) {
  Range range = _config->yRange();
  range.setMax(value.toDouble());
  _config->setYRange(range);
  _replotTimer.start(2000);
}

void
PlotConfigDialog::onMaxXRangePolicyChanged(bool fixed) {
  RangePolicy policy = _config->xRangePolicy();
  if (fixed) { policy.setMaxPolicy(RangePolicy::FIXED); _maxXRange->setEnabled(true); }
  else { policy.setMaxPolicy(RangePolicy::AUTOMATIC); _maxXRange->setEnabled(false); }
  _config->setXRangePolicy(policy);
  _replotTimer.start(2000);
}

void
PlotConfigDialog::onMinYRangePolicyChanged(bool fixed) {
  RangePolicy policy = _config->yRangePolicy();
  if (fixed) { policy.setMinPolicy(RangePolicy::FIXED); _minYRange->setEnabled(true); }
  else { policy.setMinPolicy(RangePolicy::AUTOMATIC); _minYRange->setEnabled(false); }
  _config->setYRangePolicy(policy);
  _replotTimer.start(2000);
}

void
PlotConfigDialog::onMaxYRangePolicyChanged(bool fixed) {
  RangePolicy policy = _config->yRangePolicy();
  if (fixed) { policy.setMaxPolicy(RangePolicy::FIXED); _maxYRange->setEnabled(true); }
  else { policy.setMaxPolicy(RangePolicy::AUTOMATIC); _maxYRange->setEnabled(false); }
  _config->setYRangePolicy(policy);
  _replotTimer.start(2000);
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
 * Implementation of ColorButton
 * ******************************************************************************************** */
ColorButton::ColorButton(const QColor &color, QWidget *parent)
  : QPushButton(parent), _color(color)
{
  this->setMinimumSize(64, 32); this->setMaximumSize(64,32); this->resize(64,32);
  QObject::connect(this, SIGNAL(clicked()), this, SLOT(onSelectColor()));
}

const QColor &
ColorButton::selectedColor() const {
  return _color;
}

void
ColorButton::onSelectColor() {
  // Set default colors for plot dialog:
  for (int i=0; i<AbstractGraphConfig::defaultColors.size(); i++) {
    QColorDialog::setStandardColor(i, AbstractGraphConfig::defaultColors.at(i).rgb());
  }
  // Open color dialog
  QColorDialog dialog(_color);
  dialog.setOptions(QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
  if (QDialog::Accepted != dialog.exec()) { return; }
  // Store color in config.
  _color = dialog.selectedColor();
  update();
  // signal color selected
  emit colorSelected(_color);
}

void
ColorButton::paintEvent(QPaintEvent *event) {
  QPushButton::paintEvent(event);
  QPainter painter(this);
  painter.save();
  double w = width(), h=height();
  painter.setPen(_color);
  painter.setBrush(QBrush(_color));
  painter.drawRect(8,8,w-16,h-16);
  painter.restore();
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
  _line_color = new ColorButton(_config->lineColor());
  _line_color->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  QFormLayout *grid = new QFormLayout();
  grid->addRow(tr("Label"), _label);
  grid->addRow(tr("Line color"), _line_color);
  grid->addRow(tr("X="), _formula_x);
  grid->addRow(tr("Y="), _formula_y);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(grid);
  layout->addWidget(buttons);

  setLayout(layout);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(checkInputAndExit()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(_line_color, SIGNAL(colorSelected(QColor)), this, SLOT(onSelectColor(QColor)));
}

void
LineGraphDialog::onSelectColor(QColor color) {
  _config->setLineColor(color);
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
  ColorButton *line_color = new ColorButton(_config->lineColor());
  ColorButton *fill_color = new ColorButton(_config->fillColor());

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  QFormLayout *grid = new QFormLayout();
  grid->addRow(tr("Label"), _label);
  grid->addRow(tr("Line color"), line_color);
  grid->addRow(tr("Fill color"), fill_color);
  grid->addRow(tr("X="), _formula_x);
  grid->addRow(tr("Y="), _formula_y);
  grid->addRow(tr("var(Y)="), _formula_var);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(grid);
  layout->addWidget(buttons);

  setLayout(layout);

  QObject::connect(line_color, SIGNAL(colorSelected(QColor)), this, SLOT(onSelectLineColor(QColor)));
  QObject::connect(fill_color, SIGNAL(colorSelected(QColor)), this, SLOT(onSelectFillColor(QColor)));
  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(checkInputAndExit()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}


void
VarianceLineGraphDialog::onSelectLineColor(QColor color) {
  _config->setLineColor(color);
}

void
VarianceLineGraphDialog::onSelectFillColor(QColor color) {
  _config->setFillColor(color);
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
