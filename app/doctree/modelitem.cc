#include "modelitem.hh"
#include "parametersitem.hh"
#include "speciesitem.hh"
#include "reactionsitem.hh"
#include "compartmentsitem.hh"


using namespace Fluc;



/* ********************************************************************************************* *
 * Implementation of SBML Model...
 * ********************************************************************************************* */
ModelItem::ModelItem(libsbml::SBMLDocument *document, QObject *parent) :
  QObject(parent), base_model(0), itemLabel("Model")
{
  // Now, assemble a base model from SBML:
  this->base_model = new Models::BaseModel(document->getModel());

  // Assemble child-items:
  CompartmentsItem *compartments = new CompartmentsItem(this->base_model, this);
  compartments->setTreeParent(this);

  SpeciesItem *species = new SpeciesItem(this->base_model, this);
  species->setTreeParent(this);

  ParametersItem *parameters = new ParametersItem(this->base_model, this);
  parameters->setTreeParent(this);

  ReactionsItem *reactions = new ReactionsItem(this->base_model, this);
  reactions->setTreeParent(this);

  this->_children.append(compartments);
  this->_children.append(species);
  this->_children.append(reactions);
  this->_children.append(parameters);
}



ModelItem::~ModelItem()
{
  // Free libsbml::SBMLDocument instance:
  delete this->base_model;
}


const QString &
ModelItem::getLabel() const
{
  return this->itemLabel;
}


Fluc::Ast::Model *
ModelItem::getModel()
{
  return this->base_model;
}


const Fluc::Ast::Model *
ModelItem::getModel() const
{
  return this->base_model;
}
