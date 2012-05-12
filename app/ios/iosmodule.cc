#include "iosmodule.hh"
#include "../application.hh"
#include "ioswizard.hh"
#include "iostaskwrapper.hh"

#include <QMessageBox>


IOSModule::IOSModule(QObject *parent) :
  Module(parent)
{
  // Create wizard:
  this->wizard = new IOSWizard();
  this->wizard->setModal(true);

  // Register menus
  this->iosAct = new QAction(tr("&Time Course Analysis (IOS)"), this);
  this->iosAct->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_I));
  this->iosAct->setStatusTip(tr("Configure & Run the time course analysis (IOS)."));
  Application::getApp()->addToAnalysesMenu(this->iosAct);
  QObject::connect(this->iosAct, SIGNAL(triggered()), this, SLOT(configIOS()));
}


void
IOSModule::configIOS()
{
  // Show/run the wizard:
  this->wizard->restart();
  if (! this->wizard->exec())
  {
    return;
  }

  // Construct a task from configuration:
  IOSTask *task = 0;

  try {
    task = new IOSTask(this->wizard->getConfigCast<IOSTask::Config>());
  } catch (Fluc::Exception err) {
    QMessageBox::warning(
          0, tr("Can not construct time course analysis (IOS) from model: "), err.what());
    return;
  }

  // Add task to application and run it:
  Application::getApp()->docTree()->addTask(
        this->wizard->getConfigCast<IOSTask::Config>().getModelDocument(), new IOSTaskWrapper(task));
  task->start();
}


