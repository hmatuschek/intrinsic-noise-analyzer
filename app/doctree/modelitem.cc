#include "modelitem.hh"
#include "parametersitem.hh"
#include "speciesitem.hh"
#include "reactionsitem.hh"
#include "compartmentsitem.hh"

#include "../views/modelview.hh"

#include <parser/parser.hh>

using namespace iNA;



/* ********************************************************************************************* *
 * Implementation of SBML Model...
 * ********************************************************************************************* */
ModelItem::ModelItem(const QString &file_path, QObject *parent) :
  QObject(parent), base_model(0), itemLabel("Model")
{
  // Now, assemble a base model from SBML:
  Ast::Model model; Parser::Sbml::importModel(model, file_path.toStdString());
  base_model = new Models::BaseModel(model);

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


ModelItem::ModelItem(iNA::Ast::Model *model, QObject *parent) :
  QObject(parent), base_model(0), itemLabel("Model")
{
  // Now, assemble a base model from SBML:
  base_model = new Models::BaseModel(*model);

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
  // Free model instance:
  delete this->base_model;
}


const QString &
ModelItem::getLabel() const
{
  return this->itemLabel;
}


iNA::Ast::Model &
ModelItem::getModel()
{
  return *(this->base_model);
}


const iNA::Ast::Model &
ModelItem::getModel() const
{
  return *(this->base_model);
}


bool ModelItem::providesView() const { return true; }

QWidget *
ModelItem::createView() {
  return new ModelView(this);
}
