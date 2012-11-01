#include "application.hh"

#include "../views/mainwindow.hh"
#include "../views/importmodeldialog.hh"
#include "../views/sbmlsheditordialog.hh"
#include "../views/newversiondialog.hh"

#include "../steadystate/steadystatetask.hh"
#include "../steadystate/steadystatetaskwrapper.hh"
#include "../steadystate/steadystatewizard.hh"

#include "../paramscan/paramscantask.hh"
#include "../paramscan/paramscantaskwrapper.hh"
#include "../paramscan/paramscanwizard.hh"

#include "../sse/ssewizard.hh"
#include "../sse/ssetaskconfig.hh"
#include "../sse/retaskwrapper.hh"
#include "../sse/lnataskwrapper.hh"
#include "../sse/iostaskwrapper.hh"

#include "../ssa/ssatask.hh"
#include "../ssa/ssataskwrapper.hh"
#include "../ssa/ssawizard.hh"

#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>

#include <parser/parser.hh>
#include <parser/exception.hh>
#include <trafo/reversiblereactionconverter.hh>


using namespace iNA;

/**
* Yields the parent item
*
* @todo Maybe go as static method in DocumentItem.
*/
DocumentItem * getParentDocumentItem(TreeItem * item)
{
  if (item==0) return 0;

  return (0 != dynamic_cast<DocumentItem *>(item)
      ? dynamic_cast<DocumentItem *>(item)
      : getParentDocumentItem(item->getTreeParent()));
}


/*
 * Holds the singleton instance (or null if there is none).
 */
Application *Application::singleton_instance = 0;


void
Application::shutdown()
{
  if (0 != Application::singleton_instance) {
    delete Application::singleton_instance;
    Application::singleton_instance = 0;
  }
}


Application *
Application::getApp()
{
  if (0 == Application::singleton_instance) {
    return new Application();
  }
  return Application::singleton_instance;
}



Application::Application() :
  QObject(0), Configuration(), mainWindow(0), _versionCheck()
{
  // First, store instance as singleton instance:
  Application::singleton_instance = this;

  // Construct new Document tree (model):
  this->document_tree = new DocumentTree(this);

  // There is no item selected yet:
  _selected_item = 0;

  // Assemble menu actions:
  _newModel = new QAction(tr("New model..."), this);
  _newModel->setEnabled(true);
  _newModel->setStatusTip(tr("Creates an empty model"));
  _newModel->setShortcut(QKeySequence(Qt::CTRL+ Qt::Key_N));

  _importModel = new QAction(tr("Open model..."), this);
  _importModel->setEnabled(true);
  _importModel->setStatusTip(tr("Open a model description file"));
  _importModel->setShortcut(QKeySequence::Open);

  _exportModel = new QAction(tr("Export model..."), this);
  _exportModel->setEnabled(false);
  _exportModel->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
  _exportModel->setStatusTip(tr("Export a model to a file"));

  _closeModel = new QAction(tr("Close model"), this);
  _closeModel->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_C));
  _closeModel->setEnabled(false);

  _closeAll = new QAction(tr("Close all models"), this);
  _closeAll->setEnabled(docTree()->getTreeChildCount());

  _editModel = new QAction(tr("Edit model"), this);
  _editModel->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
  _editModel->setEnabled(false);

  _expandRevReaction = new QAction(tr("Expand reversible reactions"), this);
  _expandRevReaction->setEnabled(false);

  _combineIrvReaction = new QAction(tr("Collapse irreversible reactions"), this);
  _combineIrvReaction->setEnabled(false);

  _steadyStateAction = new QAction(tr("&Steady State Analysis (SSE)"), this);
  _steadyStateAction->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_S));
  _steadyStateAction->setStatusTip(tr("Configure & Run the steady state analysis using the System Size Expansion."));

  _timeCourseAnalysisAction = new QAction(tr("&Time Course Analysis (SSE)"), this);
  _timeCourseAnalysisAction->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_T));
  _timeCourseAnalysisAction->setStatusTip(tr("Configure & Run the time course analysis (SSE)."));

  _parameterScanAction = new QAction(tr("&Steady State Parameter Scan (SSE)"), this);
  _parameterScanAction->setShortcut(QKeySequence(Qt::CTRL+Qt::SHIFT+Qt::Key_P));

  _ssaAnalysisAction = new QAction(tr("Stochastic Simulation Algorithm (SSA)"), this);

  _recentModelsMenu = new QMenu("Recent models");
  updateRecentModelsMenu();

  // Connect signals
  QObject::connect(_newModel, SIGNAL(triggered()), this, SLOT(onNewModel()));
  QObject::connect(_importModel, SIGNAL(triggered()), this, SLOT(onImportModel()));
  QObject::connect(_exportModel, SIGNAL(triggered()), this, SLOT(onExportModel()));
  QObject::connect(_closeModel, SIGNAL(triggered()), this, SLOT(onCloseModel()));
  QObject::connect(_closeAll, SIGNAL(triggered()), this, SLOT(onCloseAll()));
  QObject::connect(_editModel, SIGNAL(triggered()), this, SLOT(onEditModel()));
  QObject::connect(_expandRevReaction, SIGNAL(triggered()), this, SLOT(onExpandRevReactions()));
  QObject::connect(_combineIrvReaction, SIGNAL(triggered()), this, SLOT(onCombineIrrevReactions()));
  QObject::connect(_steadyStateAction, SIGNAL(triggered()), this, SLOT(configSteadyState()));
  QObject::connect(_parameterScanAction, SIGNAL(triggered()), this, SLOT(configParameterScan()));
  QObject::connect(_timeCourseAnalysisAction, SIGNAL(triggered()), this, SLOT(configTimeCourseAnalysis()));
  QObject::connect(_ssaAnalysisAction, SIGNAL(triggered()), this, SLOT(configSSAAnalysis()));
  QObject::connect(_recentModelsMenu, SIGNAL(triggered(QAction*)), this, SLOT(onOpenRecentModel(QAction*)));
  QObject::connect(&_versionCheck, SIGNAL(newVersionAvailable(QString)), this, SLOT(onNewVersionAvailable(QString)));
}


Application::~Application()
{
  // Done...
}


/* ******************************************************************************************** *
 * ...
 * ******************************************************************************************** */
void
Application::setMainWindow(MainWindow *mainwindow)
{
  this->mainWindow = mainwindow;
}


void
Application::resetSelectedItem() {
  _selected_item = 0;
  _exportModel->setEnabled(false);
  _editModel->setEnabled(false);
  _closeModel->setEnabled(false);
  _expandRevReaction->setEnabled(false);
  _combineIrvReaction->setEnabled(false);
  _closeAll->setEnabled(docTree()->getTreeChildCount());

}


void
Application::itemSelected(const QModelIndex &index)
{
  TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
  DocumentTreeItem *wrapper = 0;

  // This should no happen, anyway...
  if (0 == (wrapper = dynamic_cast<DocumentTreeItem *>(item))) { return; }

  // If wrapper provides a view -> show it:
  if (wrapper->providesView()) {
    this->mainWindow->showPanel(wrapper->createView());
  }

  // Set selected item:
  _selected_item = wrapper;

  // If selected item is a document item -> enabled exportModel menu item:
  if (0 != dynamic_cast<DocumentItem *>(getParentDocumentItem(wrapper))) {
    _exportModel->setEnabled(true);
    _editModel->setEnabled(true);
    _closeModel->setEnabled(true);
    _expandRevReaction->setEnabled(true);
    _combineIrvReaction->setEnabled(true);
  } else {
    _exportModel->setEnabled(false);
    _editModel->setEnabled(false);
    _closeModel->setEnabled(false);
    _expandRevReaction->setEnabled(false);
    _combineIrvReaction->setEnabled(false);
  }

}


void
Application::checkForNewVersion()
{
  // Do not check if disabled by compiler flag
#ifdef INA_DISABLE_NEW_VERSION_CHECK
  return;
#endif

  // Otherwise start check.
  _versionCheck.startCheck();
}


void
Application::showContextMenuAt(const QModelIndex &index, const QPoint &global_pos)
{
  TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
  DocumentTreeItem *wrapper = 0;

  if (0 != (wrapper = dynamic_cast<DocumentTreeItem *>(item)) && wrapper->providesContextMenu())
  {
    wrapper->showContextMenu(global_pos);
  }
}

DocumentTree * Application::docTree() { return this->document_tree; }


/* ******************************************************************************************** *
 * ...
 * ******************************************************************************************** */
QAction *Application::newModelAction() { return _newModel; }
QAction *Application::importModelAction() { return _importModel; }
QAction *Application::exportModelAction() { return _exportModel; }
QAction *Application::closeModelAction()  { return _closeModel; }
QAction *Application::closeAllAction()  { return _closeAll; }
QAction *Application::editModelAction()   { return _editModel; }
QAction *Application::expandRevReacAction() { return _expandRevReaction; }
QAction *Application::combineIrrevReacAction() { return _combineIrvReaction; }
QAction *Application::configSteadyStateAction() { return _steadyStateAction; }
QAction *Application::configParameterScanAction() { return _parameterScanAction; }
QAction *Application::configTimeCourseAction() { return _timeCourseAnalysisAction; }
QAction *Application::configSSAAnalysisAction() { return _ssaAnalysisAction; }
QMenu   *Application::recentModelsMenu() { return _recentModelsMenu; }


/* ******************************************************************************************** *
 * Implementation of callbacks/event handlers...
 * ******************************************************************************************** */
void Application::onNewModel()
{

  iNA::Ast::Model *new_model = new iNA::Ast::Model(
              "New_model", "New model");
  docTree()->addDocument(new DocumentItem(new_model));

}


void Application::onImportModel()
{
  // Show a file-dialog for files:
  QString fileName = QFileDialog::getOpenFileName(
        0, tr("Import model"), "",
        tr("All Models (*.xml *.sbml *.mod *.sbmlsh);;SBML Models (*.xml *.sbml);;SBML-sh Models (*.mod *.sbmlsh);;All Files (*.*"));
  if (0 == fileName.size()) { return; }

  onImportModel(fileName);
}


void Application::onImportModel(const QString &fileName)
{
  QFileInfo info(fileName);
  // Check if file is readable:
  if (! info.isReadable()) {
    QMessageBox::critical(0, tr("Can not import model"),
                          tr("Can not import model from file %1: File not readable.").arg(info.fileName()));
    return;
  }

  try {
    DocumentItem *new_doc = 0;
    // Try to determine file type by extension:
    if (("xml" == info.suffix()) || ("sbml" == info.suffix())) {
      new_doc = new DocumentItem(Parser::Sbml::importModel(fileName.toStdString()), fileName);
    } else if (("mod" == info.suffix()) || ("sbmlsh" == info.suffix())) {
      new_doc = new DocumentItem(Parser::Sbmlsh::importModel(fileName.toStdString()), fileName);
    }
    // Add new document to tree:
    if (0 != new_doc) {
      docTree()->addDocument(new_doc);
      addRecentModel(fileName);
      updateRecentModelsMenu();
      return;
    }
  } catch (iNA::Parser::ParserError &err) {
    QMessageBox::warning(
          0, tr("Can not open model"), err.what());
    return;
  } catch (iNA::Exception &err) {
    QMessageBox::warning(0, tr("Can not open model"), err.what());
    return;
  }

  // If unknown extension -> ask the user
  ModelFormatQuestion dialog(fileName);
  if (QDialog::Accepted != dialog.exec()) { return; }
  ModelFormatQuestion::Format format = dialog.getFormat();

  // load model.
  try {
    DocumentItem *new_doc = 0;
    // Try to determine file type by extension:
    if (ModelFormatQuestion::SBML_MODEL == format) {
      new_doc = new DocumentItem(Parser::Sbml::importModel(fileName.toStdString()), fileName);
    } else if (ModelFormatQuestion::SBMLSH_MODEL == format) {
      new_doc = new DocumentItem(Parser::Sbmlsh::importModel(fileName.toStdString()), fileName);
    }
    // Add new document to tree:
    if (0 != new_doc) {
      docTree()->addDocument(new_doc);
      addRecentModel(fileName);
      updateRecentModelsMenu();
      return;
    }
  } catch (iNA::Parser::ParserError &err) {
    QMessageBox::warning(
          0, tr("Can not open model"), err.what());
    return;
  } catch (iNA::Exception &err) {
    QMessageBox::warning(0, tr("Can not open model"), err.what());
    return;
  }
}


void Application::onExportModel()
{
  DocumentItem *document = dynamic_cast<DocumentItem *>(getParentDocumentItem(_selected_item));
  if (0 == document) { return; }

  // Ask for filename and type:
  QString selected_filter("");
  QString filename = QFileDialog::getSaveFileName(0, tr("Export model"), "", tr("SBML (*.xml *.sbml);;SBML-sh (*.mod *.sbmlsh)"), &selected_filter);
  if ("" == filename) { return; }

  // Serialize model into file...
  try {
    if ("SBML (*.xml *.sbml)" == selected_filter) {
      QFileInfo info(filename);
      if ( ("xml" != info.suffix()) && ("sbml" != info.suffix()) ) { filename.append(".xml"); }
      Parser::Sbml::exportModel(document->getModel(), filename.toStdString());
    } else if ("SBML-sh (*.mod *.sbmlsh)" == selected_filter){
      QFileInfo info(filename);
      if ( ("mod" != info.suffix()) && ("sbmlsh" != info.suffix()) ) { filename.append(".sbmlsh"); }
      Parser::Sbmlsh::exportModel(document->getModel(), filename.toStdString());
    } else {
      QMessageBox::critical(0, tr("Can not export model"), tr("Unkown file type: %1").arg(selected_filter));
    }
  } catch (Exception &err) {
    QMessageBox::warning(0, "Can not export model", err.what());
  }
}


void Application::onCloseModel()
{
  DocumentItem *document = 0;

// redundant:  if (0 == _selected_item) { return; }
  if (0 == (document = dynamic_cast<DocumentItem *>(getParentDocumentItem(_selected_item)))) { return; }
  // signal document to close
  document->closeDocument();
  resetSelectedItem();
}


void Application::onCloseAll()
{
  DocumentItem *document = 0;

  QObjectList::const_iterator it = docTree()->children().begin();

  while(it!=docTree()->children().end())
  {
    document = dynamic_cast<DocumentItem *>(*it++);
    document->closeDocument();
  }

  resetSelectedItem();

}

void Application::onEditModel()
{
  DocumentItem *document = 0;

// redundant:  if (0 == _selected_item) { return; }
  if (0 == (document = dynamic_cast<DocumentItem *>(getParentDocumentItem(_selected_item)))) { return; }

  // Create model editor dialog:
  SbmlshEditorDialog dialog;
  dialog.setModel(document->getModel());
  if (QDialog::Accepted != dialog.exec()) { return; }

  // Obtain model from dialog
  iNA::Ast::Model *new_model = dialog.takeModel();
  // Assemble new document item and add it to the tree...
  DocumentItem *new_doc = new DocumentItem(new_model);
  docTree()->addDocument(new_doc);
}


void Application::onExpandRevReactions()
{
  DocumentItem *document = 0;

// redundant:  if (0 == _selected_item) { return; }
  if (0 == (document = dynamic_cast<DocumentItem *>(getParentDocumentItem(_selected_item)))) { return; }
  iNA::Ast::Model &model = document->getModel();

  iNA::Trafo::ReversibleReactionConverter converter; converter.apply(model);

  docTree()->resetCompleteTree();

}

void Application::onCombineIrrevReactions()
{
  DocumentItem *document = 0;

// redundant:  if (0 == _selected_item) { return; }
  if (0 == (document = dynamic_cast<DocumentItem *>(getParentDocumentItem(_selected_item)))) { return; }
  iNA::Ast::Model &model = document->getModel();

  iNA::Trafo::IrreversibleReactionCollapser collector; collector.apply(model);

  docTree()->resetCompleteTree();

}

void
Application::configSteadyState()
{
  // Construct wizard:
  SteadyStateWizard wizard(0); wizard.restart();
  if (QDialog::Accepted != wizard.exec()) { return; }

  // Construct a task from configuration:
  SteadyStateTask *task = 0;

  try {
    task = new SteadyStateTask(wizard.getConfigCast<SteadyStateTask::Config>());
  } catch (iNA::Exception &err) {
    QMessageBox::warning(
          0, tr("Can not construct stochastic simulation analysis from model: "), err.what());
    return;
  }

  // Add task to application and run it:
  docTree()->addTask(
        wizard.getConfigCast<SteadyStateTask::Config>().getModelDocument(),
        new SteadyStateTaskWrapper(task));
  task->start();
}


void
Application::configParameterScan()
{
  ParamScanWizard wizard(0); wizard.restart();
  if (QDialog::Accepted != wizard.exec()) { return; }

  // Construct a task from configuration:
  ParamScanTask *task = 0;
  try {
    task = new ParamScanTask(wizard.getConfigCast<ParamScanTask::Config>());
  } catch (iNA::Exception &err) {
    QMessageBox::warning(
          0, tr("Cannot construct parameter scan from model: "), err.what());
    return;
  }

  // Add task to application and run it:
  docTree()->addTask(
        wizard.getConfigCast<ParamScanTask::Config>().getModelDocument(),
        new ParamScanTaskWrapper(task));

  task->start();
}

void
Application::configTimeCourseAnalysis() {
  SSEWizard wizard(0); wizard.restart();
  if (QDialog::Accepted != wizard.exec()) { return; }

  // Construct a task from configuration:
  Task *task = 0;

  Utils::Message message = LOG_MESSAGE(Utils::Message::INFO);
  message << "Created SSE analysis.";
  Utils::Logger::get().log(message);

  try {
    // Decide which analysis task to create:
    switch (wizard.getConfigCast<SSETaskConfig>().getMethod()) {

    case SSETaskConfig::RE_ANALYSIS:
      task = new RETask(wizard.getConfigCast<SSETaskConfig>());
      break;

    case SSETaskConfig::LNA_ANALYSIS:
      task = new LNATask(wizard.getConfigCast<SSETaskConfig>());
      break;

    case SSETaskConfig::IOS_ANALYSIS:
      task = new IOSTask(wizard.getConfigCast<SSETaskConfig>());
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
  switch(wizard.getConfigCast<SSETaskConfig>().getMethod()) {

    case SSETaskConfig::RE_ANALYSIS:
      docTree()->addTask(
            wizard.getConfigCast<SSETaskConfig>().getModelDocument(),
            new RETaskWrapper(dynamic_cast<RETask *>(task)));
      break;

    case SSETaskConfig::LNA_ANALYSIS:
      docTree()->addTask(
            wizard.getConfigCast<SSETaskConfig>().getModelDocument(),
            new LNATaskWrapper(dynamic_cast<LNATask *>(task)));
      break;

    case SSETaskConfig::IOS_ANALYSIS:
      docTree()->addTask(
            wizard.getConfigCast<SSETaskConfig>().getModelDocument(),
            new IOSTaskWrapper(dynamic_cast<IOSTask *>(task)));
      break;

  default:
    return;
  }

  task->start();
}


void
Application::configSSAAnalysis()
{
  SSAWizard wizard(0); wizard.restart();
  if (QDialog::Accepted != wizard.exec()) { return; }

  // Construct task from wizard:
  SSATask *task = 0;
  try {
    task = new SSATask(wizard.getConfigCast<SSATaskConfig>());
  } catch (iNA::Exception err) {
    QMessageBox::warning(
          0, tr("Can not construct steady state anlysis from model: "), err.what());
    return;
  }


  // Add task to application and run it:
  docTree()->addTask(
        wizard.getConfigCast<SSATaskConfig>().getModelDocument(),
        new SSATaskWrapper(task));

  task->start();
}


void
Application::onOpenRecentModel(QAction *action)
{
  onImportModel(action->data().toString());
}


void
Application::updateRecentModelsMenu() {
  // Remove all actions from the "recent models" sub-menu
  _recentModelsMenu->clear();
  QStringList recent_models; recentModels(recent_models);
  for (int i=0; i<recent_models.size(); i++) {
    // Get i-th path
    QString path = recent_models.at(i);
    // Skip unreadable files:
    if (!QFileInfo(path).isReadable()) { continue; }
    QAction *action = _recentModelsMenu->addAction(QFileInfo(path).fileName());
    action->setData(path);
    action->setToolTip(path);
  }

}


void
Application::onNewVersionAvailable(QString version)
{
  // If disabled by configuration -> skip.
  if (! notifyNewVersionAvailable()) { return; }

  // Show message;
  NewVersionDialog(version).exec();
}
