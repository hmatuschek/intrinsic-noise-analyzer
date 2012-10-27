#include "ssemodule.hh"
#include "ssewizard.hh"
#include "ssetaskconfig.hh"

#include "retaskwrapper.hh"
#include "lnataskwrapper.hh"
#include "iostaskwrapper.hh"

#include "../application.hh"
#include "utils/logger.hh"

#include <QMessageBox>


using namespace iNA;


SSEModule::SSEModule(QObject *parent) :
  Module(parent)
{
  // Create wizard:
  this->wizard = new SSEWizard();
  this->wizard->setModal(true);

  // Register menus
  this->sseAct = new QAction(tr("&Time Course Analysis (SSE)"), this);
  this->sseAct->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_T));
  this->sseAct->setStatusTip(tr("Configure & Run the time course analysis (SSE)."));
  Application::getApp()->addToAnalysesMenu(this->sseAct);
  QObject::connect(this->sseAct, SIGNAL(triggered()), this, SLOT(configSSE()));
}


void
SSEModule::configSSE()
{
  // Show/run the wizard:
  this->wizard->restart();
  if (! this->wizard->exec())
  {
    return;
  }

  // Construct a task from configuration:
  Task *task = 0;

  Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
  message << "Created SSE analysis.";
  Utils::Logger::get().log(message);

  try {
    // Decide which analysis task to create:
    switch (this->wizard->getConfigCast<SSETaskConfig>().getMethod()) {

    case SSETaskConfig::RE_ANALYSIS:
      task = new RETask(this->wizard->getConfigCast<SSETaskConfig>());
      break;

    case SSETaskConfig::LNA_ANALYSIS:
      task = new LNATask(this->wizard->getConfigCast<SSETaskConfig>());
      break;

    case SSETaskConfig::IOS_ANALYSIS:
      task = new IOSTask(this->wizard->getConfigCast<SSETaskConfig>());
      break;

    default:
      QMessageBox::warning(
            0, tr("Can not construct time course analysis (SEE) from model: "),
            "No method was selected.");
      return;
    }
  } catch (iNA::Exception err) {
    QMessageBox::warning(
          0, tr("Can not construct time course analysis (SEE) from model: "), err.what());
    return;
  }

  // Add task to application and run it:
  switch(this->wizard->getConfigCast<SSETaskConfig>().getMethod()) {

    case SSETaskConfig::RE_ANALYSIS:
      Application::getApp()->docTree()->addTask(
            this->wizard->getConfigCast<SSETaskConfig>().getModelDocument(),
            new RETaskWrapper(dynamic_cast<RETask *>(task)));
      break;

    case SSETaskConfig::LNA_ANALYSIS:
      Application::getApp()->docTree()->addTask(
            this->wizard->getConfigCast<SSETaskConfig>().getModelDocument(),
            new LNATaskWrapper(dynamic_cast<LNATask *>(task)));
      break;

    case SSETaskConfig::IOS_ANALYSIS:
      Application::getApp()->docTree()->addTask(
            this->wizard->getConfigCast<SSETaskConfig>().getModelDocument(),
            new IOSTaskWrapper(dynamic_cast<IOSTask *>(task)));
      break;

  default:
    return;
  }

  task->start();
}


