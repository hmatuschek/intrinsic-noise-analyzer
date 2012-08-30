#include "speciesitem.hh"
#include "../application.hh"
#include "../views/speciesview.hh"


using namespace iNA;



/* ********************************************************************************************* *
 * Implementation of SBML Model Species List
 * ********************************************************************************************* */
SpeciesItem::SpeciesItem(iNA::Ast::Model *model, QObject *parent)
  : QObject(parent), DocumentTreeItem(), _species(0), _itemLabel(tr("Species"))
{
  // Constructs the list of species and taske its ownership:
  this->_species = new SpeciesList(model, this);
}


SpeciesList *
SpeciesItem::species()
{
  return this->_species;
}


const QString &
SpeciesItem::getLabel() const
{
  return this->_itemLabel;
}


bool
SpeciesItem::providesView() const
{
  return true;
}


QWidget *
SpeciesItem::createView()
{
  return new SpeciesView(this);
}
