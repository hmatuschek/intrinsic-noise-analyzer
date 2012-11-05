#include "../models/application.hh"
#include "plotitem.hh"
#include "../views/plotview.hh"
#include <iostream>


PlotItem::PlotItem(Plot::Figure *plot, QObject *parent)
  : QObject(parent), DocumentTreeItem(), plot(plot), itemLabel(plot->getTitle())
{
  // Take ownership of plot:
  this->plot->setParent(this);

  // Construct actions:
  this->context_menu = new QMenu();
  this->removeAct = new QAction(tr("Remove Plot"), this->context_menu);

  // Assemble menu:
  this->context_menu->addAction(this->removeAct);

  // Connect to menu events:
  QObject::connect(this->removeAct, SIGNAL(triggered()), this, SLOT(removePlot()));

}


Plot::Figure *
PlotItem::getPlot()
{
  return this->plot;
}


const QString &
PlotItem::getLabel() const
{
  return this->itemLabel;
}


bool
PlotItem::providesView() const
{
  return true;
}


QWidget *
PlotItem::createView()
{
  return new PlotView(this);
}


bool
PlotItem::providesContextMenu() const
{
  return true;
}


void
PlotItem::showContextMenu(const QPoint &global_pos)
{
  this->context_menu->popup(global_pos);
}


void
PlotItem::removePlot()
{

  // Remove item from document tree:
  Application::getApp()->docTree()->removePlot(this);

  // Mark object for deletion:
  this->deleteLater();
}

PlotItem::~PlotItem()
{
  // Hide and clean menu, and mark it for deletion later on:
  this->context_menu->hide();
  this->context_menu->clear();
  this->context_menu->deleteLater();
}
