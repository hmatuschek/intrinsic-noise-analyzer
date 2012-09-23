#include "application.hh"
#include "mainwindow.hh"
#include "views/importmodeldialog.hh"
#include "views/exportmodeldialog.hh"

#include <QMessageBox>

#include "views/sbmlsheditordialog.hh"

#include "parser/parser.hh"
#include "parser/exception.hh"

#include <trafo/reversiblereactionconverter.hh>


using namespace iNA;


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
  QObject(0), Configuration(), mainWindow(0)
{
  // First, store instance as singleton instance:
  Application::singleton_instance = this;

  // Construct new Document tree (model):
  this->document_tree = new DocumentTree(this);

  // There is no item selected yet:
  _selected_item = 0;

  // Assemble menu actions:
  _importModel = new QAction(tr("Open model..."), this);
  _importModel->setEnabled(true);
  _importModel->setStatusTip(tr("Open a model description file"));
  _importModel->setShortcut(QKeySequence::Open);

  _exportModel = new QAction(tr("Export model..."), this);
  _exportModel->setEnabled(false);
  _exportModel->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
  _exportModel->setStatusTip(tr("Export a model to a file"));

  _closeModel = new QAction(tr("Close model"), this);
  _closeModel->setEnabled(false);

  _editModel = new QAction(tr("Edit model"), this);
  _editModel->setEnabled(false);

  _expandRevReaction = new QAction(tr("Expand rev. reactions"), this);
  _expandRevReaction->setEnabled(false);

  _combineIrvReaction = new QAction(tr("Combine irrev. reactions"), this);
  _combineIrvReaction->setEnabled(false);

  // Connect signals
  QObject::connect(_importModel, SIGNAL(triggered()), this, SLOT(onImportModel()));
  QObject::connect(_exportModel, SIGNAL(triggered()), this, SLOT(onExportModel()));
  QObject::connect(_closeModel, SIGNAL(triggered()), this, SLOT(onCloseModel()));
  QObject::connect(_editModel, SIGNAL(triggered()), this, SLOT(onEditModel()));
  QObject::connect(_expandRevReaction, SIGNAL(triggered()), this, SLOT(onExpandRevReactions()));
  QObject::connect(_combineIrvReaction, SIGNAL(triggered()), this, SLOT(onCombineIrrevReactions()));
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
Application::addModule(Module *module)
{
  this->modules.append(module);
  module->setParent(this);
}


void
Application::addToAnalysesMenu(QAction *action)
{
  this->mainWindow->getAnalysesMenu()->addAction(action);
}


void
Application::resetSelectedItem() {
  _selected_item = 0;
  _exportModel->setEnabled(false);
  _editModel->setEnabled(false);
  _closeModel->setEnabled(false);
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
  if (0 != dynamic_cast<DocumentItem *>(wrapper)) {
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
QAction *Application::importModelAction() { return _importModel; }
QAction *Application::exportModelAction() { return _exportModel; }
QAction *Application::closeModelAction()  { return _closeModel; }
QAction *Application::editModelAction()   { return _editModel; }
QAction *Application::expandRevReacAction() { return _expandRevReaction; }
QAction *Application::combineIrrevReacAction() { return _combineIrvReaction; }


/* ******************************************************************************************** *
 * Implementation of callbacks/event handlers...
 * ******************************************************************************************** */
void Application::onImportModel()
{
  // Show a file-dialog for files:
  ImportModelDialog *dialog = new ImportModelDialog();
  if (QDialog::Accepted != dialog->exec()) return;

  // Get filename and description format
  QString fileName = dialog->getFileName();
  ImportModelDialog::Format format = dialog->getFormat();
  delete dialog;

  DocumentItem *new_doc = 0;
  try {
    // Try to construct model from file:
    if (ImportModelDialog::SBML_MODEL == format) {
      new_doc = new DocumentItem(Parser::Sbml::importModel(fileName.toStdString()), fileName);
    } else if (ImportModelDialog::SBMLSH_MODEL == format) {
      new_doc = new DocumentItem(Parser::Sbmlsh::importModel(fileName.toStdString()), fileName);
    }
  } catch (iNA::Parser::ParserError &err) {
    QMessageBox::warning(
          0, tr("Can not open model"), err.what());
    return;
  } catch (iNA::Exception &err) {
    QMessageBox::warning(0, tr("Can not open model"), err.what());
    return;
  }

  // Add new document to tree:
  docTree()->addDocument(new_doc);
}


void Application::onExportModel()
{
  DocumentItem *document = 0;
  if (0 == _selected_item) { return; }
  if (0 == (document = dynamic_cast<DocumentItem *>(_selected_item))) { return; }

  // Show export model dialog:
  ExportModelDialog *dialog = new ExportModelDialog();
  if (QDialog::Accepted != dialog->exec()) return;

  // Get filename and description format
  QString fileName = dialog->getFileName();
  ExportModelDialog::Format format = dialog->getFormat();
  delete dialog;

  // Serialize model into file...
  try {
    if (ExportModelDialog::SBML_MODEL == format) {
      Parser::Sbml::exportModel(document->getModel(), fileName.toStdString());
    } else if (ExportModelDialog::SBMLSH_MODEL == format){
      Parser::Sbmlsh::exportModel(document->getModel(), fileName.toStdString());
    }
  } catch (Exception &err) {
    QMessageBox::warning(0, "Can not export model...", err.what());
  }
}


void Application::onCloseModel()
{
  DocumentItem *document = 0;

  if (0 == _selected_item) { return; }
  if (0 == (document = dynamic_cast<DocumentItem *>(_selected_item))) { return; }
  // signal document to close
  document->closeDocument();
}


void Application::onEditModel()
{
  DocumentItem *document = 0;

  if (0 == _selected_item) { return; }
  if (0 == (document = dynamic_cast<DocumentItem *>(_selected_item))) { return; }

  // Create model editor dialog:
  SbmlshEditorDialog *dialog = new SbmlshEditorDialog();
  dialog->setModel(document->getModel());
  if (QDialog::Accepted != dialog->exec()) { delete dialog; return; }

  // Obtain model from dialog
  iNA::Ast::Model *new_model = dialog->takeModel(); delete dialog;
  // Assemble new document item and add it to the tree...
  DocumentItem *new_doc = new DocumentItem(new_model);
  docTree()->addDocument(new_doc);
}


void Application::onExpandRevReactions()
{
  DocumentItem *document = 0;

  if (0 == _selected_item) { return; }
  if (0 == (document = dynamic_cast<DocumentItem *>(_selected_item))) { return; }
  iNA::Ast::Model &model = document->getModel();

  iNA::Trafo::ReversibleReactionConverter converter; converter.apply(model);
  docTree()->resetCompleteTree();
}

void Application::onCombineIrrevReactions()
{
  DocumentItem *document = 0;

  if (0 == _selected_item) { return; }
  if (0 == (document = dynamic_cast<DocumentItem *>(_selected_item))) { return; }
  iNA::Ast::Model &model = document->getModel();


  iNA::Trafo::IrreversibleReactionCollapsor collector; collector.apply(model);

  QMessageBox::information(0, "Not implemented yet.", "This feature is not implemented yet.");
}
