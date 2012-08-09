#include "application.hh"
#include "mainwindow.hh"
#include "views/importmodeldialog.hh"
#include "views/exportmodeldialog.hh"
#include <QMessageBox>

#include "parser/parser.hh"
#include "exception.hh"

using namespace Fluc;


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
  _importModel = new QAction(tr("Import model..."), this);
  _importModel->setEnabled(true);
  _importModel->setStatusTip(tr("Imports a model description file."));
  _importModel->setShortcut(QKeySequence::Open);

  _exportModel = new QAction(tr("Export model..."), this);
  _exportModel->setEnabled(false);
  _exportModel->setShortcut(QKeySequence::SaveAs);

  _closeModel = new QAction(tr("Close model"), this);
  _closeModel->setEnabled(false);

  // Connect signals
  QObject::connect(_importModel, SIGNAL(triggered()), this, SLOT(onImportModel()));
  QObject::connect(_exportModel, SIGNAL(triggered()), this, SLOT(onExportModel()));
  QObject::connect(_closeModel, SIGNAL(triggered()), this, SLOT(onCloseModel()));
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
    _closeModel->setEnabled(true);
  } else {
    _exportModel->setEnabled(false);
    _closeModel->setEnabled(false);
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
  } catch (Fluc::Exception &err) {
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
