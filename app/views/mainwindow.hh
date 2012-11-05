#ifndef __INA_APP_VIEWS_MAINWINDOW_HH__
#define __INA_APP_VIEWS_MAINWINDOW_HH__

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QSplitter>
#include <QScrollArea>

#include "../models/application.hh"
#include "documentsview.hh"
#include "logwindow.hh"



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
  /** Toggels the "check custom updates" */
  void checkcustomUpdatesToggled();

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
  /** Shows the "check custom updates" menu item. */
  QAction *_checkcustomUpdatesAct;
  /** Holds the splitter, that splits the main window into a left and right half. */
  QSplitter *_mainSplitter;
  /** Holds the QTreeView custom the left pane, showing the all open models, and their structutre. */
  DocumentsView *_modelView;
  /** Holds a QScrollArea, holding the main-widget of the right-pane. */
  QScrollArea *_mainPane;
  /** Holds the (initially invisible) log-window. */
  LogWindow *_logWindow;
};


#endif // __INA_APP_VIEWS_MAINWINDOW_HH__
