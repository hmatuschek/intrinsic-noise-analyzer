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
  QObject(parent), _model(0), _itemLabel("Model")
{
  // Now, assemble a base model from SBML:
  _model = new Ast::Model();
  Parser::Sbml::importModel(*_model, file_path.toStdString());

  // Assemble child-items:
  CompartmentsItem *compartments = new CompartmentsItem(this->_model, this);
  compartments->setTreeParent(this);

  SpeciesItem *species = new SpeciesItem(this->_model, this);
  species->setTreeParent(this);

  ParametersItem *parameters = new ParametersItem(this->_model, this);
  parameters->setTreeParent(this);

  ReactionsItem *reactions = new ReactionsItem(this->_model, this);
  reactions->setTreeParent(this);

  this->_children.append(compartments);
  this->_children.append(species);
  this->_children.append(reactions);
  this->_children.append(parameters);
}


ModelItem::ModelItem(iNA::Ast::Model *model, QObject *parent) :
  QObject(parent), _model(model), _itemLabel("Model")
{
  // Assemble child-items:
  addChild(new CompartmentsItem(this->_model, this));
  addChild(new SpeciesItem(this->_model, this));
  addChild(new ParametersItem(this->_model, this));
  addChild(_reactions = new ReactionsItem(this->_model, this));
}


ModelItem::~ModelItem() {
  // Free model instance:
  delete this->_model;
}

const QString &
ModelItem::getLabel() const {
  return _itemLabel;
}

iNA::Ast::Model &
ModelItem::getModel() {
  return *(this->_model);
}

const iNA::Ast::Model &
ModelItem::getModel() const {
  return *(this->_model);
}

ReactionsItem *
ModelItem::reactionsItem() {
  return _reactions;
}

bool
ModelItem::providesView() const {
  return true;
}

QWidget *
ModelItem::createView() {
  return new ModelView(this);
}

void
ModelItem::updateItem() {
  // pass...
}
