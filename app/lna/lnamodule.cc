#include "lnamodule.hh"
#include "../application.hh"
#include "lnawizard.hh"
#include "lnatask.hh"
#include "lnataskwrapper.hh"
#include <QMessageBox>


LNAModule::LNAModule(QObject *parent) :
  Module(parent)
{
  // Create wizard:
  this->wizard = new LNAWizard();
  this->wizard->setModal(true);

  // Register menus
  this->lnaAct = new QAction(tr("&Time Course Analysis (SSE)"), this);
  this->lnaAct->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_T));
  this->lnaAct->setStatusTip(tr("Configure & Run the time course analysis using System Size Expansion."));
  Application::getApp()->addToAnalysesMenu(this->lnaAct);
  QObject::connect(this->lnaAct, SIGNAL(triggered()), this, SLOT(configLNA()));
}


void
LNAModule::configLNA()
{
  // Show/run the wizard:
  this->wizard->restart();
  if (! this->wizard->exec())
  {
    return;
  }

  // Construct a task from configuration:
  LNATask *task = 0;

  try {
    task = new LNATask(this->wizard->getTaskConfig());
  } catch (Fluc::Exception err) {
    QMessageBox::warning(
          0, tr("Can not construct time course analysis (SSE) from model: "), err.what());
    return;
  }

  // Add task to application and run it:
  Application::getApp()->docTree()->addTask(
        this->wizard->getDocument(), new LNATaskWrapper(task));
  task->start();
}


