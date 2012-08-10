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
  /** Default constructor for an Application. */
  explicit Application();


public:
  /** Destructor, also frees the SBML models held. (on application exit) */
  ~Application();

  /** Sets the main-window of the application. */
  void setMainWindow(MainWindow *mainwindow);

  /** Adds a module to the application. */
  void addModule(Module *module);

  /** Adds some entry to the 'Analyses' menu. */
  void addToAnalysesMenu(QAction *action);

  /** Checks if the item has a context-menu and displays it. */
  void showContextMenuAt(const QModelIndex &index, const QPoint &global_pos);

  /** Returns the document tree of the application. */
  DocumentTree *docTree();
  /** Returns a weak reference to the "import model" menu action. */
  QAction *importModelAction();
  /** Returns a weak reference to the "export model" menu action. */
  QAction *exportModelAction();
  /** Retunrs a weak reference to the "close model" menu action. */
  QAction *closeModelAction();


public slots:
  /** Resets the currently selected document tree item. */
  void resetSelectedItem();

  /** Opens an item in the document tree.
   * This method checks if the item has an associated view, if so, the item is displayed on
   * the main-panel using that view. It also sets the currently selected item. */
  void itemSelected(const QModelIndex &index);


public:
  /** Shuts the only running application instance down. */
  static void shutdown();

  /** Retunrs the running application or 0 if there is none. */
  static Application *getApp();


private slots:
  /** Handler for the "import model" menu action. */
  void onImportModel();

  /** Handler for "export model" action. */
  void onExportModel();

  /** Handler for "remove model" action. */
  void onCloseModel();


private:
  /** Holds the only reference to the application running. */
  static Application *singleton_instance;

  /** Holds the main-window. */
  MainWindow *mainWindow;
  /** Holds the complete document tree. */
  DocumentTree *document_tree;
  /** Hold the list of loaded modules. */
  QList<Module *> modules;

  /** Holds a weak reference to the currently selected item. */
  DocumentTreeItem *_selected_item;

  /** The "import model" menu action. */
  QAction *_importModel;
  /** The "export model" menu action. */
  QAction *_exportModel;
  /** The "close model" menu action. */
  QAction *_closeModel;
};


#endif // APPLICATION_HH
