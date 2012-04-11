#include "parameterlist.hh"

ParameterList::ParameterList(Fluc::Ast::Model *model, QObject *parent)
  : QAbstractTableModel(parent), model(model)
{
  // Pass...
}


QVariant
ParameterList::data(const QModelIndex &index, int role) const
{
  if (! index.isValid() || 5 <= index.column())
  {
    return QVariant();
  }

  if (int(this->model->numParameters()) <= index.row())
  {
    return QVariant();
  }

  Fluc::Ast::Parameter *param = this->model->getParameter(index.row());

  if (4 == index.column() && Qt::CheckStateRole == role)
  {
    if (param->isConst())
    {
      return Qt::Checked;
    }
    return Qt::Unchecked;
  }

  if (Qt::DisplayRole != role)
  {
    return QVariant();
  }

  switch (index.column())
  {
  case 0:
    return QVariant(param->getIdentifier().c_str());

  case 1:
    if (! param->hasName())
      QVariant("<not set>");
    return QVariant(param->getName().c_str());

  case 2:
    return QVariant(this->getInitialValueForParameter(param));

  case 3:
  {
    std::stringstream str; param->getUnit().dump(str);
    return QVariant(str.str().c_str());
  }

  default:
    break;
  }

  return QVariant();
}


QVariant
ParameterList::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (Qt::DisplayRole != role || orientation != Qt::Horizontal || 5 <= section)
  {
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  switch (section)
  {
  case 0:
    return QVariant("Id");

  case 1:
    return QVariant("Name");

  case 2:
    return QVariant("Initial Value");

  case 3:
    return QVariant("Unit");

  case 4:
    return QVariant("Constant");

  default:
    break;
  }

  return QVariant();
}


int
ParameterList::rowCount(const QModelIndex &parent) const
{
  return this->model->numParameters();
}


int
ParameterList::columnCount(const QModelIndex &parent) const
{
  return 5;
}


QString
ParameterList::getInitialValueForParameter(Fluc::Ast::Parameter *param) const
{
  std::stringstream str;
  str << param->getValue();
  QString init_val(str.str().c_str());
  return init_val;
}
