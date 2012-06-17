#include "ssamodule.hh"
#include "../application.hh"
#include "ssatask.hh"
#include "ssataskwrapper.hh"
#include <QMessageBox>


SSAModule::SSAModule(QObject *parent)
  : Module(parent)
{
  QAction *act = new QAction(tr("Stochastic Simulation Algorithm (SSA)"), this);
  QObject::connect(act, SIGNAL(triggered()), this, SLOT(configSSA()));
  Application::getApp()->addToAnalysesMenu(act);

  this->wizard = new SSAWizard();
}


SSAModule::~SSAModule()
{
  delete this->wizard;
}


void
SSAModule::configSSA()
{
  this->wizard->setModal(true);
  this->wizard->restart();

  // Show/run the wizard:
  if (! this->wizard->exec())
  {
    return;
  }

  // Construct task from wizard:
  SSATask *task = 0;
  try {
    task = new SSATask(wizard->getConfigCast<SSATaskConfig>());
  } catch (Fluc::Exception err) {
    QMessageBox::warning(
          0, tr("Can not construct steady state anlysis from model: "), err.what());
    return;
  }


  // Add task to application and run it:
  Application::getApp()->docTree()->addTask(
        this->wizard->getConfigCast<SSATaskConfig>().getModelDocument(),
        new SSATaskWrapper(task));

  task->start();
}
