/**
 * @defgroup gui The Grafical User Interface Classes
 */

#ifndef __FLUC_APP_APPLICATION_HH__
#define __FLUC_APP_APPLICATION_HH__

#include <QApplication>
#include <QObject>

#include "configuration.hh"
#include "../doctree/documenttree.hh"
#include "versioncheck.hh"


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

  /** Checks if the item has a context-menu and displays it. */
  void showContextMenuAt(const QModelIndex &index, const QPoint &global_pos);

  /** Returns the document tree of the application. */
  DocumentTree *docTree();
  /** Returns a weak reference to the "new model" menu action. */
  QAction *newModelAction();
  /** Returns a weak reference to the "import model" menu action. */
  QAction *importModelAction();
  /** Returns a weak reference to the "export model" menu action. */
  QAction *exportModelAction();
  /** Returns a weak reference to the "edit model" menu action. */
  QAction *editModelAction();
  /** Returns a weak reference to the "close model" menu action. */
  QAction *closeModelAction();
  /** Returns a weak reference to the "close all" menu action. */
  QAction *closeAllAction();
  /** Returns a weak ref to the "expand rev reaction" menu action. */
  QAction *expandRevReacAction();
  /** Returns a weak ref to the "combine irrev reaction" menu action. */
  QAction *combineIrrevReacAction();
  QAction *configSteadyStateAction();
  QAction *configParameterScanAction();
  QAction *configTimeCourseAction();
  QAction *configSSAAnalysisAction();

  /** Returns the recentModelsMenu */
  QMenu *recentModelsMenu();

public slots:
  /** Resets the currently selected document tree item. */
  void resetSelectedItem();

  /** Opens an item in the document tree.
   * This method checks if the item has an associated view, if so, the item is displayed on
   * the main-panel using that view. It also sets the currently selected item. */
  void itemSelected(const QModelIndex &index);

  /** Checks if a new version of iNA is available. */
  void checkForNewVersion();

public:
  /** Shuts the only running application instance down. */
  static void shutdown();

  /** Retunrs the running application or 0 if there is none. */
  static Application *getApp();


private slots:
  /** Handler for the "new model" menu action. */
  void onNewModel();
  /** Handler for the "import model" menu action. */
  void onImportModel();
  /** Slot to import a certain model. */
  void onImportModel(const QString &path);
  /** Handler for "export model" action. */
  void onExportModel();
  /** Handler for "remove model" action. */
  void onCloseModel();
  /** Handler for "close all" action. */
  void onCloseAll();
  /** Handler for the "edit model" action. */
  void onEditModel();
  /** Handler for the "expand reversible reactions" action. */
  void onExpandRevReactions();
  /** Handler for the "combine irreversible reactions" action. */
  void onCombineIrrevReactions();
  /** Handler to open a recently opened model. */
  void onOpenRecentModel(QAction *action);
  /** handler to show Steady State Analysis Wizard. */
  void configSteadyState();
  /** Handler to show Parameter Scan Wizard. */
  void configParameterScan();
  /** Handler to show Time Course Analysis Wizard. */
  void configTimeCourseAnalysis();
  void configSSAAnalysis();
  /** Updates the recently imported models menu. */
  void updateRecentModelsMenu();
  /** Shows a message that there is a new version of iNA available if not disabled. */
  void onNewVersionAvailable(QString version);

private:
  /** Holds the only reference to the application running. */
  static Application *singleton_instance;

  /** Holds the main-window. */
  MainWindow *mainWindow;
  /** Holds the complete document tree. */
  DocumentTree *document_tree;
  /** Holds the version checker. */
  VersionCheck _versionCheck;
  /** Holds a weak reference to the currently selected item. */
  DocumentTreeItem *_selected_item;

  /** The "new model" menu action. */
  QAction *_newModel;
  /** The "import model" menu action. */
  QAction *_importModel;
  /** The "export model" menu action. */
  QAction *_exportModel;
  /** The "close model" menu action. */
  QAction *_closeModel;
  /** The "close all" menu action. */
  QAction *_closeAll;
  /** The "edit model" menu action. */
  QAction *_editModel;
  /** The "expand reversible ractions" menu action. */
  QAction *_expandRevReaction;
  /** The "combine irreversible ractions" menu action. */
  QAction *_combineIrvReaction;
  /** The "steady state analysis (SSE)" action. */
  QAction *_steadyStateAction;
  QAction *_parameterScanAction;
  QAction *_timeCourseAnalysisAction;
  QAction *_ssaAnalysisAction;

  /** Holds the "recently opened" menu. */
  QMenu *_recentModelsMenu;
};


#endif // APPLICATION_HH
