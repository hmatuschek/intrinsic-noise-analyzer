#include "plotitem.hh"
#include "../views/plotview.hh"
#include <iostream>


PlotItem::PlotItem(Plot::Figure *plot, QObject *parent)
  : QObject(parent), DocumentTreeItem(), plot(plot), itemLabel(plot->getTitle())
{
  // Take ownership of plot:
  this->plot->setParent(this);

  // pass...
}


PlotItem::~PlotItem()
{
  //std::cerr << "in PlotWrapper::~PlotWrapper() ..." << std::endl;
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
