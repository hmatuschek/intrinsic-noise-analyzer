#include "conservationanalysistask.hh"
#include "libina/trafo/constantfolder.hh"

#include <QGraphicsScene>

#include "../tinytex/ginac2formula.hh"

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
  : Task(), _config(config), _analysis(0), _cons_expr(), _cons_const()
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
  _analysis = new Models::ConservationAnalysis(_config.getModelDocument()->getModel());
  // Now, get result...
  Trafo::InitialValueFolder folder(*_analysis);

  // Assemble vector of species symbols
  Eigen::VectorXex s(_analysis->numSpecies());
  for (size_t i=0; i<_analysis->numSpecies(); i++) { s(i) = _analysis->getSpecies(i)->getSymbol(); }
  // Obtain conserved cycles
  _cons_expr = _analysis->getConservedCycles(s);
  // Calc conservation constants
  _cons_const.resize(_cons_expr.cols());
  for (int i=0; i<_cons_const.cols(); i++) {
    _cons_const(i) = folder.apply(_cons_expr(i));
  }
  // done. (big deal...)

  setState(Task::DONE); setProgress(1.0);
}

QString
ConservationAnalysisTask::getLabel() {
  return tr("Conservation analysis");
}

size_t
ConservationAnalysisTask::numCycles() const {
  return _cons_expr.cols();
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
 * Implementation of analysis result widget.
 * ******************************************************************************************** */
ConservationAnalysisWidget::ConservationAnalysisWidget(ConservationAnalysisTask *task, QWidget *parent)
  : QGraphicsView()
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

  MathContext ctx;
  QGraphicsScene *scene = new QGraphicsScene(); setScene(scene);
  // Layout conservation cycles and show on scene
  QGraphicsItem *align_item = align.layout(ctx);
  scene->addItem(align_item);
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
  addPage(new ModelSelectionWizardPage(this));
}

GeneralTaskConfig &
ConservationAnalysisWizard::getConfig() {
  return _config;
}



