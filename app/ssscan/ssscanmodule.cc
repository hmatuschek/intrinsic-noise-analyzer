#include "ssscanmodule.hh"
#include "ssscantask.hh"
#include "ssscantaskwrapper.hh"
#include "../application.hh"
#include <QMessageBox>


SSScanModule::SSScanModule(QObject *parent) :
  Module(parent)
{
  // Create wizard:
  this->wizard = new SSScanWizard();
  this->wizard->setModal(true);

  // Create Menu:
  // Register menus
  this->scanAction = new QAction(tr("&ParameterScan (SSE)"), this);
  Application::getApp()->addToAnalysesMenu(this->scanAction);
  QObject::connect(this->scanAction, SIGNAL(triggered()), this, SLOT(configTask()));
}


void
SSScanModule::configTask()
{
  this->wizard->restart();

  // Show/run the wizard:
  if (! this->wizard->exec()) { return; }

  // Construct a task from configuration:
  SSScanTask *task = 0;

  try {
    task = new SSScanTask(this->wizard->getConfigCast<SSScanTask::Config>());
  } catch (iNA::Exception &err) {
    QMessageBox::warning(
          0, tr("Can not construct parameter scan from model: "), err.what());
    return;
  }

  // Add task to application and run it:
  Application::getApp()->docTree()->addTask(
        this->wizard->getConfigCast<SSScanTask::Config>().getModelDocument(),
        new SSScanTaskWrapper(task));

  task->start();
}
