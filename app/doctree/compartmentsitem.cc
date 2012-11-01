#include "compartmentsitem.hh"
#include "../models/application.hh"
#include "../views/compartmentview.hh"


using namespace iNA;



/* ********************************************************************************************* *
 * Implementation of SBML Model Parameters
 * ********************************************************************************************* */
CompartmentsItem::CompartmentsItem(iNA::Ast::Model *model, QObject *parent)
  : QObject(parent), _compartments(0), itemLabel(tr("Compartments"))
{
  // Construct list of compartments and takes its ownership:
  this->_compartments = new CompartmentList(model, this);
}


const QString &
CompartmentsItem::getLabel() const
{
  return this->itemLabel;
}


CompartmentList *
CompartmentsItem::compartments()
{
  return this->_compartments;
}


bool
CompartmentsItem::providesView() const
{
  return true;
}


QWidget *
CompartmentsItem::createView()
{
  return new CompartmentView(this);
}
