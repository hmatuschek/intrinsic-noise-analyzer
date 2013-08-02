#include "plotitem.hh"
#include "documenttree.hh"
#include "../models/application.hh"
#include "../views/plotview.hh"
#include "../plot/figure.hh"
#include "../plot/configuration.hh"
#include <iostream>

static QString _empty_plot_title = "<No Plot Title>";

PlotItem::PlotItem(Plot::Figure *plot, QObject *parent)
  : QObject(parent), DocumentTreeItem(), _plot(plot), _config(0), _itemLabel(plot->getTitle())
{
  // Take ownership of plot:
  this->_plot->setParent(this);

  // Construct actions:
  this->_context_menu = new QMenu();
  this->_removeAct = new QAction(tr("Remove Plot"), this->_context_menu);

  // Assemble menu:
  this->_context_menu->addAction(this->_removeAct);

  // Connect to menu events:
  QObject::connect(this->_removeAct, SIGNAL(triggered()), this, SLOT(removePlot()));
}


PlotItem::PlotItem(Plot::PlotConfig *config, QObject *parent)
  : QObject(parent), DocumentTreeItem(), _plot(0), _config(config), _itemLabel(_config->title())
{
  // Take ownership of plot:
  this->_plot = config->createFigure();
  this->_plot->setParent(this);

  // Construct actions:
  this->_context_menu = new QMenu();
  this->_removeAct = new QAction(tr("Remove Plot"), this->_context_menu);

  // Assemble menu:
  this->_context_menu->addAction(this->_removeAct);

  // Connect to menu events:
  QObject::connect(this->_removeAct, SIGNAL(triggered()), this, SLOT(removePlot()));
}


Plot::Figure *
PlotItem::getPlot() {
  return _plot;
}

bool
PlotItem::hasConfig() {
  return 0 != _config;
}

Plot::PlotConfig *
PlotItem::config() {
  return _config;
}

void
PlotItem::updatePlot() {
  _plot->deleteLater();
  _plot = _config->createFigure();
  _itemLabel = _plot->getTitle();
  Application::getApp()->docTree()->markForUpdate(this);
}

const QString &
PlotItem::getLabel() const {
  if (0 == _itemLabel.size()) { return _empty_plot_title; }
  return _itemLabel;
}


bool
PlotItem::providesView() const {
  return true;
}


QWidget *
PlotItem::createView() {
  return new PlotView(this);
}


bool
PlotItem::providesContextMenu() const {
  return true;
}


void
PlotItem::showContextMenu(const QPoint &global_pos)
{
  this->_context_menu->popup(global_pos);
}


void
PlotItem::removePlot()
{
  // Remove item from document tree:
  Application::getApp()->docTree()->removePlot(this);
  Application::getApp()->resetSelectedItem();

  // Mark object for deletion:
  this->deleteLater();
}

PlotItem::~PlotItem()
{
  // Hide and clean menu, and mark it for deletion later on:
  this->_context_menu->hide();
  this->_context_menu->clear();
  this->_context_menu->deleteLater();
}
