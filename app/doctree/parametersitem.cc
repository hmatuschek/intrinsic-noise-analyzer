#include "parametersitem.hh"
#include "../models/application.hh"
#include "../views/parameterview.hh"


using namespace iNA;



ParametersItem::ParametersItem(iNA::Ast::Model *model, QObject *parent)
  : QObject(parent), _itemLabel(tr("Global Parameters"))
{
  // pass...
}


const QString &
ParametersItem::getLabel() const {
  return this->_itemLabel;
}

bool
ParametersItem::providesView() const {
  return true;
}


QWidget *
ParametersItem::createView() {
  return new ParameterView(this);
}
