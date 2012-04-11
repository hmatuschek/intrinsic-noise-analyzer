#include "reactionparameterlist.hh"
#include <sstream>



/* ********************************************************************************************* *
 * Implementation of ReactionParameterModel for the local parameters of a reaction
 * ********************************************************************************************* */
ReactionParameterList::ReactionParameterList(Fluc::Ast::KineticLaw *law, QObject *parent)
  : QAbstractTableModel(parent), kinetic_law(law)
{
  // Pass...
}


QVariant
ReactionParameterList::data(const QModelIndex &index, int role) const
{
  if (! index.isValid() || 5 <= index.column())
  {
    return QVariant();
  }

  if (int(this->kinetic_law->numParameters()) <= index.row())
  {
    return QVariant();
  }

  Fluc::Ast::Parameter *param = this->kinetic_law->getParameter(index.row());
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
      return QVariant("<not set>");
    return QVariant(param->getName().c_str());

  case 2:
  {
    std::ostringstream stream; stream << param->getValue();
    return QVariant(stream.str().c_str());
  }

  case 3:
  {
    std::ostringstream stream; param->getUnit().dump(stream);
    return QVariant(stream.str().c_str());
  }
  default:
    break;
  }

  return QVariant();
}



QVariant
ReactionParameterList::headerData(int section, Qt::Orientation orientation, int role) const
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
    return QVariant("Value");

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
ReactionParameterList::rowCount(const QModelIndex &parent) const
{
  return this->kinetic_law->numParameters();
}


int
ReactionParameterList::columnCount(const QModelIndex &parent) const
{
  return 5;
}
