#include "application.hh"
#include "mainwindow.hh"
#include "views/compartmentview.hh"


/*
 * Holds the singleton instance (or null if there is none).
 */
Application *Application::singleton_instance = 0;


Application *
Application::factory()
{
  if (0 != Application::singleton_instance)
  {
    return Application::singleton_instance;
  }

  // Instantiate application:
  Application::singleton_instance = new Application();
  return Application::singleton_instance;
}


void
Application::shutdown()
{
  if (0 != Application::singleton_instance)
  {
    delete Application::singleton_instance;
    Application::singleton_instance = 0;
  }
}


Application *
Application::getApp()
{
  return Application::singleton_instance;
}



Application::Application() :
  QObject(0), Configuration(), mainWindow(0)
{
  // Construct new Document tree:
  this->document_tree = new DocumentTree(this);
}


Application::~Application()
{
  // Done...
}


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
Application::openModelIndex(const QModelIndex &index)
{
  TreeItem *item = static_cast<TreeItem *>(index.internalPointer());
  DocumentTreeItem               *wrapper = 0;

  if (0 != (wrapper = dynamic_cast<DocumentTreeItem *>(item)) && wrapper->providesView())
  {
    this->mainWindow->showPanel(wrapper->createView());
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


DocumentTree *
Application::docTree()
{
  return this->document_tree;
}

