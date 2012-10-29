#include "mainwindow.hh"
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QPalette>
#include <QDesktopServices>
#include <QUrl>

#include "application.hh"
#include "ina.hh"
#include "views/aboutdialog.hh"
#include "views/importmodeldialog.hh"


using namespace iNA;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
  // Most important feature: main-title:
  this->setWindowTitle("Intrinsic Noise Analyzer");

  // Create actions and menus:
  this->createActions();
  this->createMenus();

  // Create layout:
  this->mainSplitter = new QSplitter(Qt::Horizontal, this);

  this->modelView = new DocumentsView();
  this->mainSplitter->addWidget(this->modelView);

  this->mainPane = new QScrollArea();
  this->mainPane->setBackgroundRole(QPalette::Dark);
  this->mainPane->setWidgetResizable(true);
  this->mainSplitter->addWidget(this->mainPane);

  // approx golden ratio for splitter:
  this->mainSplitter->setStretchFactor(0, 0);
  this->mainSplitter->setStretchFactor(1, 2);

  this->setCentralWidget(this->mainSplitter);

  // Ensure window has minimum size:
  this->setMinimumSize(810, 500);

  this->logWindow = new LogWindow();
  this->logWindow->setVisible(false);
}


void
MainWindow::showPanel(QWidget *panel)
{
  this->mainPane->setWidget(panel);
}


void
MainWindow::createActions()
{
  // Define some actions for main menu:
  this->quitAct = new QAction(tr("&Quit"), this);
  this->quitAct->setShortcuts(QKeySequence::Quit);
  this->quitAct->setStatusTip(tr("Quit the application"));

  this->aboutAct = new QAction(tr("&About"), this);
  this->aboutAct->setMenuRole(QAction::AboutRole);

  this->onlineHelp = new QAction(tr("&Help"), this);
  this->onlineHelp->setShortcuts(QKeySequence::HelpContents);
  this->onlineHelp->setStatusTip(tr("Open the online-help"));

  this->showLogsAct = new QAction(tr("Show log"), this);
  this->showLogsAct->setShortcut(QKeySequence(Qt::Key_F10));
  this->showLogsAct->setStatusTip(tr("Show the log window"));

  // Connect signals:
  connect(this->quitAct, SIGNAL(triggered()), this, SLOT(quit()));
  connect(this->aboutAct, SIGNAL(triggered()), this, SLOT(about()));
  connect(this->onlineHelp, SIGNAL(triggered()), this, SLOT(openTutorial()));
  connect(this->showLogsAct, SIGNAL(triggered()), this, SLOT(showLogs()));
}


void
MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(Application::getApp()->newModelAction());
  fileMenu->addAction(Application::getApp()->importModelAction());
  fileMenu->addMenu(Application::getApp()->recentModelsMenu());
  fileMenu->addAction(Application::getApp()->exportModelAction());
  fileMenu->addSeparator();
  fileMenu->addAction(Application::getApp()->closeModelAction());
  fileMenu->addAction(Application::getApp()->closeAllAction());
  fileMenu->addSeparator();
  fileMenu->addAction(quitAct);

  this->modelMenu = this->menuBar()->addMenu(tr("&Edit")); //< or name "Model"

  this->modelMenu->addAction(Application::getApp()->expandRevReacAction());
  this->modelMenu->addAction(Application::getApp()->combineIrrevReacAction());
  this->modelMenu->addSeparator();
  this->modelMenu->addAction(Application::getApp()->editModelAction());

  this->analysisMenu = this->menuBar()->addMenu(tr("&Analysis"));

  this->helpMenu = this->menuBar()->addMenu(tr("&Help"));
  this->helpMenu->addAction(this->onlineHelp);
  this->helpMenu->addAction(this->aboutAct);
  this->helpMenu->addAction(this->showLogsAct);
}


QMenu *
MainWindow::getAnalysesMenu()
{
  return this->analysisMenu;
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

void MainWindow::showLogs() { this->logWindow->setVisible(true); }
