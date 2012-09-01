#include "ssscanmodule.hh"
#include "ssscantask.hh"
#include "ssscantaskwrapper.hh"
#include "../application.hh"
#include <QMessageBox>


ParamScanModule::ParamScanModule(QObject *parent) :
  Module(parent)
{
  // Create wizard:
  this->wizard = new ParamScanWizard();
  this->wizard->setModal(true);

  // Create Menu:
  // Register menus
  this->scanAction = new QAction(tr("&Parameter Scan (SSE)"), this);
  Application::getApp()->addToAnalysesMenu(this->scanAction);
  QObject::connect(this->scanAction, SIGNAL(triggered()), this, SLOT(configTask()));
}


void
ParamScanModule::configTask()
{
  this->wizard->restart();

  // Show/run the wizard:
  if (! this->wizard->exec()) { return; }

  // Construct a task from configuration:
  ParamScanTask *task = 0;

  try {
    task = new ParamScanTask(this->wizard->getConfigCast<ParamScanTask::Config>());
  } catch (iNA::Exception &err) {
    QMessageBox::warning(
          0, tr("Cannot construct parameter scan from model: "), err.what());
    return;
  }

  // Add task to application and run it:
  Application::getApp()->docTree()->addTask(
        this->wizard->getConfigCast<ParamScanTask::Config>().getModelDocument(),
        new SSScanTaskWrapper(task));

  task->start();
}
