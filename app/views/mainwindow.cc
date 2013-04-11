#include "mainwindow.hh"
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QPalette>
#include <QDesktopServices>
#include <QUrl>

#include "../models/application.hh"
#include <ina.hh>
#include "aboutdialog.hh"
#include "importmodeldialog.hh"


using namespace iNA;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
  // Most important feature: main-title:
  setWindowTitle("Intrinsic Noise Analyzer");

  // Create actions and menus:
  _createActions();
  _createMenus();

  // Create layout:
  _mainSplitter = new QSplitter(Qt::Horizontal, this);

  _modelView = new DocumentsView();
  _mainSplitter->addWidget(this->_modelView);

  _mainPane = new QScrollArea();
  _mainPane->setBackgroundRole(QPalette::Dark);
  _mainPane->setWidgetResizable(true);
  _mainSplitter->addWidget(this->_mainPane);

  // approx golden ratio for splitter:
  _mainSplitter->setStretchFactor(0, 0);
  _mainSplitter->setStretchFactor(1, 2);

  setCentralWidget(this->_mainSplitter);

  // Ensure window has minimum size:
  setMinimumSize(810, 500);

  // Create hidden log window.
  _logWindow = new LogWindow();
  _logWindow->setVisible(false);
}


void
MainWindow::showPanel(QWidget *panel)
{
  this->_mainPane->setWidget(panel);
}


void
MainWindow::_createActions()
{
  // Define some actions for main menu:
  this->_quitAct = new QAction(tr("&Quit"), this);
  this->_quitAct->setShortcuts(QKeySequence::Quit);
  this->_quitAct->setStatusTip(tr("Quit the application"));

  this->_aboutAct = new QAction(tr("&About"), this);
  this->_aboutAct->setMenuRole(QAction::AboutRole);

  this->_onlineHelp = new QAction(tr("&Help"), this);
  this->_onlineHelp->setShortcuts(QKeySequence::HelpContents);
  this->_onlineHelp->setStatusTip(tr("Open the online-help"));

  this->_showLogsAct = new QAction(tr("Show log"), this);
  this->_showLogsAct->setShortcut(QKeySequence(Qt::Key_F10));
  this->_showLogsAct->setStatusTip(tr("Show the log window"));

  this->_checkForUpdatesAct = new QAction(tr("Check for updates"), this);
  this->_checkForUpdatesAct->setStatusTip(tr("Enables periodic check for new versions of iNA."));
  this->_checkForUpdatesAct->setCheckable(true);
  this->_checkForUpdatesAct->setChecked(Application::getApp()->checkNewVersionAvailable());

  // If update check is disabled at compile time -> disable menu entry:
#ifndef INA_ENABLE_VERSION_CHECK
  this->_checkForUpdatesAct->setEnabled(false);
  this->_checkForUpdatesAct->setChecked(false);
  this->_checkForUpdatesAct->setVisible(false);
#endif

  // Connect signals:
  connect(this->_quitAct, SIGNAL(triggered()), this, SLOT(quit()));
  connect(this->_aboutAct, SIGNAL(triggered()), this, SLOT(about()));
  connect(this->_onlineHelp, SIGNAL(triggered()), this, SLOT(openTutorial()));
  connect(this->_showLogsAct, SIGNAL(triggered()), this, SLOT(showLogs()));
}


void
MainWindow::_createMenus()
{
  _fileMenu = menuBar()->addMenu(tr("&File"));
  _fileMenu->addAction(Application::getApp()->newModelAction());
  _fileMenu->addAction(Application::getApp()->importModelAction());
  _fileMenu->addMenu(Application::getApp()->recentModelsMenu());
  _fileMenu->addAction(Application::getApp()->exportModelAction());
  _fileMenu->addSeparator();
  _fileMenu->addAction(Application::getApp()->closeModelAction());
  _fileMenu->addAction(Application::getApp()->closeAllAction());
  _fileMenu->addSeparator();
  _fileMenu->addAction(_quitAct);

  this->_modelMenu = this->menuBar()->addMenu(tr("&Edit"));

  this->_modelMenu->addAction(Application::getApp()->expandRevReacAction());
  this->_modelMenu->addAction(Application::getApp()->combineIrrevReacAction());
  this->_modelMenu->addSeparator();
  this->_modelMenu->addAction(Application::getApp()->editModelAction());

  this->_analysisMenu = this->menuBar()->addMenu(tr("&Analysis"));
  this->_analysisMenu->addAction(Application::getApp()->configSteadyStateAction());
  this->_analysisMenu->addAction(Application::getApp()->configParameterScanAction());
  this->_analysisMenu->addAction(Application::getApp()->configTimeCourseAction());
  this->_analysisMenu->addSeparator();
  this->_analysisMenu->addAction(Application::getApp()->configSSAParameterScanAction());
  this->_analysisMenu->addAction(Application::getApp()->configSSAAnalysisAction());

  this->_helpMenu = this->menuBar()->addMenu(tr("&Help"));
  this->_helpMenu->addAction(this->_onlineHelp);
  this->_helpMenu->addAction(this->_showLogsAct);
  this->_helpMenu->addAction(this->_checkForUpdatesAct);
  this->_helpMenu->addAction(this->_aboutAct);
}


void MainWindow::quit() { QApplication::exit(0); }

void
MainWindow::about() {
  AboutDialog about_dialog;
  about_dialog.setModal(true);
  about_dialog.exec();
}

void
MainWindow::openTutorial() {
  QDesktopServices::openUrl(QUrl("http://code.google.com/p/intrinsic-noise-analyzer/wiki/Help"));
}

void MainWindow::showLogs() { this->_logWindow->setVisible(true); }


void
MainWindow::checkForUpdatesToggled()
{
  Application::getApp()->setCheckNewVersionAvailable(_checkForUpdatesAct->isChecked());
}
