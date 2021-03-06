#include "conservationanalysistask.hh"
#include "trafo/constantfolder.hh"

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QHeaderView>

#include "../tinytex/ginac2formula.hh"
#include "utils/logger.hh"
#include "../views/unitrenderer.hh"


using namespace iNA;


/* ******************************************************************************************** *
 * Implementation of Task config.
 * ******************************************************************************************** */
ConservationAnalysisConfig::ConservationAnalysisConfig()
  : GeneralTaskConfig(), ModelSelectionTaskConfig()
{
  // pass...
}

ConservationAnalysisConfig::ConservationAnalysisConfig(const ConservationAnalysisConfig &other)
  : GeneralTaskConfig(), ModelSelectionTaskConfig(other)
{
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of analysis Task.
 * ******************************************************************************************** */
ConservationAnalysisTask::ConservationAnalysisTask(
    const ConservationAnalysisConfig &config, QObject *parent)
  : Task(parent), _config(config), _analysis(0), _cons_expr(), _cons_const()
{
  // pass...
}

ConservationAnalysisTask::~ConservationAnalysisTask() {
  if (0 != _analysis) { delete _analysis; }
}


void
ConservationAnalysisTask::process() {
  setState(Task::RUNNING); setProgress(0.0);

  // Create & perform analysis:
  _analysis = new Models::ConservedQuantities(_config.getModelDocument()->getModel());
  // Now, get result...
  Trafo::InitialValueFolder folder(*_analysis);

  // Assemble vector of species symbols
  Eigen::VectorXex s(_analysis->numSpecies());
  for (size_t i=0; i<_analysis->numSpecies(); i++) {
    s(i) = _analysis->getSpecies(i)->getSymbol();
  }

  // Obtain conserved cycles
  _cons_expr = _analysis->getConservedCycles(s);
  _cons_const = folder.apply(_cons_expr);

  // done. (big deal...)
  setState(Task::DONE); setProgress(1.0);
}

QString
ConservationAnalysisTask::getLabel() {
  return tr("Conservation analysis");
}

size_t
ConservationAnalysisTask::numCycles() const {
  return _cons_expr.rows();
}

GiNaC::ex
ConservationAnalysisTask::cycle(size_t i) const {
  return _cons_expr(i);
}

GiNaC::ex
ConservationAnalysisTask::constant(size_t i) const {
  return _cons_const(i);
}

Ast::Model &
ConservationAnalysisTask::model() {
  return *_analysis;
}


/* ******************************************************************************************** *
 * Implementation of analysis result table.
 * ******************************************************************************************** */
ConservationAnalysisTable::ConservationAnalysisTable(ConservationAnalysisTask *task)
  : QAbstractTableModel(), _task(task)
{
  // pass...
}

int
ConservationAnalysisTable::rowCount(const QModelIndex &parent) const {
  return _task->numCycles();
}

int
ConservationAnalysisTable::columnCount(const QModelIndex &parent) const {
  return 3;
}

QVariant
ConservationAnalysisTable::data(const QModelIndex &index, int role) const {
  if ((Qt::DecorationRole != role) || !index.isValid()) { return QVariant(); }
  if ((index.row() >= int(_task->numCycles())) || (index.column() >= 3)) { return QVariant(); }

  // Return conserved quantity or unit
  switch (index.column()) {
  case 0:
    return Ginac2Formula::toPixmap(_task->cycle(index.row()), _task->model(), true);

  case 1:
    return Ginac2Formula::toPixmap(_task->constant(index.row()), _task->model(), true);

  case 2: {
    UnitRenderer renderer(_task->model().getSpeciesUnit());
    return renderer.toPixmap();
  }

  default:
    break;
  }
  // oops...
  return QVariant();
}

QVariant
ConservationAnalysisTable::headerData(int section, Qt::Orientation orientation, int role) const {
  if (Qt::DisplayRole != role) { return QVariant(); }

  if (Qt::Horizontal == orientation) {
    switch (section) {
    case 0: return tr("Conserved quantity");
    case 1: return tr("Value");
    case 2: return tr("Unit");
    default: break;
    }
  } else if (Qt::Vertical == orientation) {
    if ((section < 0) || (section >= int(_task->numCycles()))) { return QVariant(); }
    return QString("%1").arg(section+1);
  }

  return QVariant();
}


/* ******************************************************************************************** *
 * Implementation of analysis result widget.
 * ******************************************************************************************** */
ConservationAnalysisWidget::ConservationAnalysisWidget(ConservationAnalysisTask *task, QWidget *parent)
  : QWidget(parent), _task(task)
{
  /// @todo Explain conservation cycles.
  QLabel *label = new QLabel(tr(""));
  label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  label->setVisible(false);

  QTableView *view = new QTableView();
  view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  view->setModel(new ConservationAnalysisTable(_task));
  view->setSelectionMode(QAbstractItemView::NoSelection);
  view->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);

  QPushButton *updateButton = new QPushButton(tr("Update"));
  updateButton->setToolTip(tr("Update analysis for modified model."));
  updateButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Layout widget:
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label);
  layout->addWidget(view);

  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addStretch(1);
  button_box->addWidget(updateButton);
  layout->addLayout(button_box);

  setLayout(layout);

  QObject::connect(updateButton, SIGNAL(clicked()), this, SLOT(onUpdateAnalysis()));
}

void
ConservationAnalysisEqWidget::onUpdateAnalysis() {
  if (_task->isFinished())
    _task->start();
}


/* ******************************************************************************************** *
 * Implementation of analysis result widget.
 * ******************************************************************************************** */
ConservationAnalysisEqWidget::ConservationAnalysisEqWidget(ConservationAnalysisTask *task, QWidget *parent)
  : QWidget(parent), _task(task)
{
  MathAlign align;

  // fill it...
  for (size_t i=0; i<task->numCycles(); i++) {
    MathItem *left = Ginac2Formula::toFormula(task->cycle(i), task->model(), true);
    MathFormula *right = new MathFormula();
    right->appendItem(new MathText(" = "));
    right->appendItem(Ginac2Formula::toFormula(task->constant(i), task->model(), true));
    align.addRow(left, right);
  }

  MathContext ctx; ctx.setFontSize(ctx.fontSize()+4);
  QGraphicsScene *scene = new QGraphicsScene();
  QGraphicsView *view = new QGraphicsView(); view->setScene(scene);
  view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  // Layout conservation cycles and show on scene
  QGraphicsItem *align_item = align.layout(ctx);
  scene->addItem(align_item);

  /// @todo Explain conservation cycles.
  QLabel *label = new QLabel(tr(""));
  label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  label->setVisible(false);

  QPushButton *updateButton = new QPushButton(tr("Update"));
  updateButton->setToolTip(tr("Updates analysis for modified model."));
  updateButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  // Layout widget:
  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(label);
  layout->addWidget(view);

  QHBoxLayout *button_box = new QHBoxLayout();
  button_box->addStretch(1);
  button_box->addWidget(updateButton);
  layout->addLayout(button_box);

  setLayout(layout);

  QObject::connect(updateButton, SIGNAL(clicked()), this, SLOT(onUpdateAnalysis()));
}

void
ConservationAnalysisWidget::onUpdateAnalysis() {
  if (_task->isFinished())
    _task->start();
}


/* ******************************************************************************************** *
 * Implementation of analysis result view
 * ******************************************************************************************** */
ConservationAnalysisView::ConservationAnalysisView(TaskItem *item, QWidget *parent)
  : TaskView(item, parent)
{
  // Update view
  taskStateChanged();
}


QWidget *
ConservationAnalysisView::createResultWidget(TaskItem *task_item) {
  ConservationAnalysisTask *task = dynamic_cast<ConservationAnalysisTask *>(task_item->getTask());
  //return new ConservationAnalysisEqWidget(task);
  return new ConservationAnalysisWidget(task);
}


/* ******************************************************************************************** *
 * Implementation of analysis task item
 * ******************************************************************************************** */
ConservationAnalysisItem::ConservationAnalysisItem(ConservationAnalysisTask *task)
  : TaskItem(task)
{
  // pass...
}

bool
ConservationAnalysisItem::providesView() const {
  return true;
}

QWidget *
ConservationAnalysisItem::createView() {

  return new ConservationAnalysisView(this);
}


/* ******************************************************************************************** *
 * Implementation of analysis task config wizard
 * ******************************************************************************************** */
ConservationAnalysisWizard::ConservationAnalysisWizard(ConservationAnalysisConfig &config)
  : GeneralTaskWizard(), _config(config)
{
  setWindowTitle(tr("Configure conservation analysis"));

  ModelSelectionWizardPage *page = new ModelSelectionWizardPage(this);
  page->setTitle(tr("Conservation Analysis"));
  page->setSubTitle(tr("Select model to view conserved quantities."));
  addPage(page);
}

GeneralTaskConfig &
ConservationAnalysisWizard::getConfig() {
  return _config;
}



