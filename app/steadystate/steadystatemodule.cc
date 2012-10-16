#include "steadystatemodule.hh"
#include "steadystatetask.hh"
#include "steadystatetaskwrapper.hh"
#include "../application.hh"
#include <QMessageBox>


SteadyStateModule::SteadyStateModule(QObject *parent) :
  Module(parent)
{
  // Create wizard:
  this->wizard = new SteadyStateWizard();
  this->wizard->setModal(true);

  // Create Menu:
  // Register menus
  this->steadyStateAction = new QAction(tr("&Steady State Analysis (SSE)"), this);
  this->steadyStateAction->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_S));
  this->steadyStateAction->setStatusTip(tr("Configure & Run the steady state analysis using the System Size Expansion."));
  Application::getApp()->addToAnalysesMenu(this->steadyStateAction);
  QObject::connect(this->steadyStateAction, SIGNAL(triggered()), this, SLOT(configSteadyState()));
}


void
SteadyStateModule::configSteadyState()
{
  this->wizard->restart();

  // Show/run the wizard:
  if (! this->wizard->exec())
  {
    return;
  }

  // Construct a task from configuration:
  SteadyStateTask *task = 0;

  try {
    task = new SteadyStateTask(this->wizard->getConfigCast<SteadyStateTask::Config>());
  } catch (iNA::Exception &err) {
    QMessageBox::warning(
          0, tr("Can not construct stochastic simulation analysis from model: "), err.what());
    return;
  }

  // Add task to application and run it:
  Application::getApp()->docTree()->addTask(
        this->wizard->getConfigCast<SteadyStateTask::Config>().getModelDocument(),
        new SteadyStateTaskWrapper(task));

  task->start();
}
