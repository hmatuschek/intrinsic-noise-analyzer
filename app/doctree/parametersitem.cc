#include "parametersitem.hh"
#include "../application.hh"
#include "../views/parameterview.hh"


using namespace iNA;



ParametersItem::ParametersItem(iNA::Ast::Model *model, QObject *parent)
  : QObject(parent), _parameters(0), itemLabel(tr("Global Parameters"))
{
  // Construct list of parameters and task ownership:
  this->_parameters = new ParameterList(model, this);
}


ParameterList *
ParametersItem::parmeters()
{
  return this->_parameters;
}


const QString &
ParametersItem::getLabel() const
{
  return this->itemLabel;
}


bool
ParametersItem::providesView() const
{
  return true;
}


QWidget *
ParametersItem::createView()
{
  return new ParameterView(this);
}
