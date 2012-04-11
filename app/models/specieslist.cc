#include "specieslist.hh"

SpeciesList::SpeciesList(Fluc::Ast::Model *model, QObject *parent)
  : QAbstractTableModel(parent), model(model)
{
  // Pass...
}


QVariant
SpeciesList::data(const QModelIndex &index, int role) const
{
  if (! index.isValid() || 6 <= index.column())
  {
    return QVariant();
  }

  if (int(this->model->numSpecies()) <= index.row())
  {
    return QVariant();
  }

  Fluc::Ast::Species *spec = this->model->getSpecies(index.row());

  if (4 == index.column() && Qt::CheckStateRole == role)
  {
    if (spec->isConst())
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
    return QVariant(spec->getIdentifier().c_str());

  case 1:
    if (! spec->hasName())
      return QVariant("<not set>");
    return QVariant(spec->getName().c_str());

  case 2:
    return QVariant(this->getInitialValueForSpecies(spec));

  case 3:
  {
    std::stringstream str; spec->getUnit().dump(str);
    return QVariant(str.str().c_str());
  }

  case 5:
    if (spec->getCompartment()->hasName())
      return QVariant(spec->getCompartment()->getName().c_str());
    return QVariant(spec->getCompartment()->getIdentifier().c_str());

  default:
    break;
  }

  return QVariant();
}


QVariant
SpeciesList::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (Qt::DisplayRole != role || orientation != Qt::Horizontal || 6 <= section)
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

  case 5:
    return QVariant("Compartment");

  default:
    break;
  }

  return QVariant();
}


int
SpeciesList::rowCount(const QModelIndex &parent) const
{
  return this->model->numSpecies();
}


int
SpeciesList::columnCount(const QModelIndex &parent) const
{
  return 6;
}


QString
SpeciesList::getInitialValueForSpecies(Fluc::Ast::Species *spec) const
{
  std::stringstream str;
  if (spec->hasValue())
    str << spec->getValue();
  QString init_val(str.str().c_str());
  return init_val;
}

