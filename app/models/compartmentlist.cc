#include "compartmentlist.hh"

CompartmentList::CompartmentList(Fluc::Ast::Model *model, QObject *parent)
  : QAbstractTableModel(parent), model(model)
{
  // Pass...
}



QVariant
CompartmentList::data(const QModelIndex &index, int role) const
{
  if (! index.isValid() || 5 <= index.column()) {
    return QVariant();
  }

  if (int(this->model->numCompartments()) <= index.row()) {
    return QVariant();
  }

  Fluc::Ast::Compartment *comp = this->model->getCompartment(index.row());
  if (4 == index.column() && Qt::CheckStateRole == role)
  {
    if (comp->isConst()) {
      return Qt::Checked;
    }
    return Qt::Unchecked;
  }

  if (Qt::DisplayRole != role) {
    return QVariant();
  }

  switch (index.column())
  {
  case 0:
    return QVariant(comp->getIdentifier().c_str());

  case 1:
    if (! comp->hasName())
      return QVariant("<not set>");
    return QVariant(comp->getName().c_str());

  case 2:
    return QVariant(this->getInitialValueForCompartment(comp));

  case 3:
  {
    std::stringstream str; comp->getUnit().dump(str);
    return QVariant(str.str().c_str());
  }

  default:
    break;
  }

  return QVariant();
}


QVariant
CompartmentList::headerData(int section, Qt::Orientation orientation, int role) const
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
    return QVariant("Volume");

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
CompartmentList::rowCount(const QModelIndex &parent) const
{
  return this->model->numCompartments();
}


int
CompartmentList::columnCount(const QModelIndex &parent) const
{
  return 5;
}


QString
CompartmentList::getInitialValueForCompartment(Fluc::Ast::Compartment *comp) const
{
  std::stringstream str; str << comp->getValue();
  QString init_val(str.str().c_str());
  return init_val;
}
