#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QSplitter>
#include <QScrollArea>

#include "application.hh"
#include "views/documentsview.hh"
#include "views/logwindow.hh"



/**
 * The application main window, implements the layout.
 *
 * @ingroup gui
 */
class MainWindow : public QMainWindow
{
  Q_OBJECT


public:
  /**
   * Constructor.
   */
  explicit MainWindow(QWidget *parent = 0);

  /**
   * Returns a weak reference to the 'Analyses' menu.
   */
  QMenu *getAnalysesMenu();


public slots:
  /**
   * Shows the given widget in the main-panel.
   */
  void showPanel(QWidget *panel);


private slots:
  /** When triggered, quits the application. */
  void quit();

  /** Shows the about dialog. */
  void about();

  /** Opens a browser and shows the online-help. */
  void openTutorial();

  void showLogs();


private:
  /**
   * Helper method to construct all manu-actions.
   */
  void createActions();

  /**
   * Helper method to construct menu-bar.
   */
  void createMenus();


private:
  /**
   * Holds the file-menu.
   */
  QMenu *fileMenu;

  /**
   * Holds the 'Analyses' menu.
   */
  QMenu *analysisMenu;

  /**
   * Holds the 'Model' menu.
   */
  QMenu *modelMenu;

  /**
   * Holds teh 'Help' menu.
   */
  QMenu *helpMenu;

  /**
   * Holds the "quit the application" action.
   */
  QAction *quitAct;

  /**
   * The "about" action.
   */
   QAction *aboutAct;

  /**
   * The online-help action.
   */
   QAction *onlineHelp;

   QAction *showLogsAct;

  /**
   * Holds the splitter, that splits the main window into a left and right half.
   */
  QSplitter *mainSplitter;

  /**
   * Holds the QTreeView for the left pane, showing the all open models, and their structutre.
   */
  DocumentsView *modelView;

  /**
   * Holds a QScrollArea, holding the main-widget of the right-pane.
   */
   QScrollArea *mainPane;

   /**
    * Holds the (initially invisible) log-window.
    */
   LogWindow *logWindow;
};


#endif // MAINWINDOW_HH
