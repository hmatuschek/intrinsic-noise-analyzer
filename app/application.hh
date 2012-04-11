/**
 * @defgroup gui The Grafical User Interface Classes
 */

#ifndef __FLUC_APP_APPLICATION_HH__
#define __FLUC_APP_APPLICATION_HH__

#include <QApplication>
#include <QObject>

#include "module.hh"
#include "configuration.hh"
#include "doctree/documenttree.hh"


/*
 * Forward declarations...
 */
class MainWindow;


/**
 * This class forms the model of the whole running application.
 *
 * It implements also the TreeModel interface lising all open SBMLModel instances.
 *
 * @ingroup gui
 */
class Application : public QObject, public Configuration
{
  Q_OBJECT

protected:
  /**
   * Default constructor for an Application.
   */
  explicit Application();


public:
  /**
   * Destructor, also frees the SBML models held. (on application exit)
   */
  ~Application();

  /**
   * Sets the main-window of the application.
   */
  void setMainWindow(MainWindow *mainwindow);

  /**
   * Adds a module to the application.
   */
  void addModule(Module *module);

  /**
   * Adds some entry to the 'Analyses' menu.
   */
  void addToAnalysesMenu(QAction *action);

  /**
   * Opens an item in the document tree.
   *
   * This method checks if the item has an associated view, if so, the item is displayed on
   * the main-panel using that view.
   */
  void openModelIndex(const QModelIndex &index);

  /**
   * Checks if the item has a context-menu and displays it.
   */
  void showContextMenuAt(const QModelIndex &index, const QPoint &global_pos);

  /**
   * Returns the document tree of the application.
   */
  DocumentTree *docTree();


public:
  /**
   * Factory method for application singleton.
   */
  static Application *factory();

  /**
   * Shuts the only running application instance down.
   */
  static void shutdown();

  /**
   * Retunrs the running application or 0 if there is none.
   */
  static Application *getApp();


private:
  /**
   * Holds the only reference to the application running.
   */
  static Application *singleton_instance;

  /**
   * Holds the main-window.
   */
  MainWindow *mainWindow;

  /**
   * Holds the complete document tree.
   */
  DocumentTree *document_tree;

  /**
   * Hold the list of loaded modules.
   */
  QList<Module *> modules;
};


#endif // APPLICATION_HH
