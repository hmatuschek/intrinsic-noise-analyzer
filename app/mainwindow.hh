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
  /** Constructor. */
  explicit MainWindow(QWidget *parent = 0);


public slots:
  /** Shows the given widget in the main-panel. */
  void showPanel(QWidget *panel);


private slots:
  /** When triggered, quits the application. */
  void quit();
  /** Shows the about dialog. */
  void about();
  /** Opens a browser and shows the online-help. */
  void openTutorial();
  /** Shows the logging window. */
  void showLogs();
  /** Toggels the "check for updates" */
  void checkForUpdatesToggled();

private:
  /** Helper method to construct all manu-actions. */
  void _createActions();

  /** Helper method to construct menu-bar. */
  void _createMenus();


private:
  /** Holds the file-menu. */
  QMenu *_fileMenu;
  /** Holds the 'Analyses' menu. */
  QMenu *_analysisMenu;
  /** Holds the 'Model' menu. */
  QMenu *_modelMenu;
  /** Holds the 'Help' menu. */
  QMenu *_helpMenu;
  /** Holds the "quit the application" action. */
  QAction *_quitAct;
  /** The "about" action. */
  QAction *_aboutAct;
  /** The online-help action. */
  QAction *_onlineHelp;
  /** Shows the log window. */
  QAction *_showLogsAct;
  /** Shows the "check for updates" menu item. */
  QAction *_checkForUpdatesAct;
  /** Holds the splitter, that splits the main window into a left and right half. */
  QSplitter *_mainSplitter;
  /** Holds the QTreeView for the left pane, showing the all open models, and their structutre. */
  DocumentsView *_modelView;
  /** Holds a QScrollArea, holding the main-widget of the right-pane. */
  QScrollArea *_mainPane;
  /** Holds the (initially invisible) log-window. */
  LogWindow *_logWindow;
};


#endif // MAINWINDOW_HH
