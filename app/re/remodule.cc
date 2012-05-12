#include "remodule.hh"
#include "../application.hh"
#include "rewizard.hh"
#include "retaskwrapper.hh"

#include <QMessageBox>


REModule::REModule(QObject *parent) :
  Module(parent)
{
  // Create wizard:
  this->wizard = new REWizard();
  this->wizard->setModal(true);

  // Register menus
  this->reAct = new QAction(tr("&Time Course Analysis (RE)"), this);
  this->reAct->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_R));
  this->reAct->setStatusTip(tr("Configure & Run the deterministic time course analysis."));
  Application::getApp()->addToAnalysesMenu(this->reAct);
  QObject::connect(this->reAct, SIGNAL(triggered()), this, SLOT(configRE()));
}


void
REModule::configRE()
{
  // Show/run the wizard:
  this->wizard->restart();
  if (! this->wizard->exec())
  {
    return;
  }

  // Construct a task from configuration:
  RETask *task = 0;

  try {
    task = new RETask(this->wizard->getConfigCast<RETask::Config>());
  } catch (Fluc::Exception err) {
    QMessageBox::warning(
          0, tr("Can not construct time course analysis (RE) from model: "), err.what());
    return;
  }

  // Add task to application and run it:
  Application::getApp()->docTree()->addTask(
        this->wizard->getConfigCast<RETask::Config>().getModelDocument(), new RETaskWrapper(task));
  task->start();
}


