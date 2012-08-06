#include "reactionparameterlist.hh"
#include <sstream>
#include "parser/parser.hh"
#include "utils/logger.hh"
#include "exception.hh"



/* ********************************************************************************************* *
 * Implementation of ReactionParameterModel for the local parameters of a reaction
 * ********************************************************************************************* */
ReactionParameterList::ReactionParameterList(Fluc::Ast::KineticLaw *law, QObject *parent)
  : QAbstractTableModel(parent), _kinetic_law(law)
{
  // Pass...
}


Qt::ItemFlags
ReactionParameterList::flags(const QModelIndex &index) const
{
  // Default flags:
  Qt::ItemFlags item_flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  // Filter invalid indices:
  if (! index.isValid()) return Qt::NoItemFlags;
  if (5 <= index.column()) return Qt::NoItemFlags;
  if (int(_kinetic_law->numParameters()) <= index.row()) return Qt::NoItemFlags;

  // Mark only column 1 & 2 editable
  if ( (1 == index.column()) || (2 == index.column()) ) item_flags |= Qt::ItemIsEditable;

  return item_flags;
}


QVariant
ReactionParameterList::data(const QModelIndex &index, int role) const
{
  // Filter invalid indices:
  if (! index.isValid() || 5 <= index.column()) { return QVariant(); }
  if (int(this->_kinetic_law->numParameters()) <= index.row()) { return QVariant(); }

  // Get selected paramter:
  Fluc::Ast::Parameter *param = this->_kinetic_law->getParameter(index.row());

  // Handle const flag:
  if (4 == index.column() && Qt::CheckStateRole == role) {
    if (param->isConst()) { return Qt::Checked; }
    return Qt::Unchecked;
  }

  // Handle display role:
  if (Qt::DisplayRole == role) {
    // Handle identifier:
    if (0 == index.column()) { return QVariant(param->getIdentifier().c_str()); }
    // Handle paramter name
    if (1 == index.column()) {
      if (! param->hasName())
        return QVariant("<not set>");
      return QVariant(param->getName().c_str());
    }
    // Handle initial value:
    if (2 == index.column()) {
      std::ostringstream stream; stream << param->getValue();
      return QVariant(stream.str().c_str());
    }
    // Handle unit:
    if (3 == index.column()) {
      std::ostringstream stream; param->getUnit().dump(stream);
      return QVariant(stream.str().c_str());
    }
  }

  // Handle edit role:
  if (Qt::EditRole == role) {
    // Handle paramter name
    if (1 == index.column()) {
      if (! param->hasName()) { return QVariant(""); }
      return QVariant(param->getName().c_str());
    }
    // Handle initial value:
    if (2 == index.column()) {
      std::ostringstream stream; stream << param->getValue();
      return QVariant(stream.str().c_str());
    }
  }

  return QVariant();
}


bool
ReactionParameterList::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Filter invald indices
  if (index.row() >= int(_kinetic_law->numParameters())) return false;
  if (index.column() >= 5) return false;

  // Get paramter for index (row):
  Fluc::Ast::Parameter *param = _kinetic_law->getParameter(index.row());

  if (1 == index.column()) {
    // If name is changed, get new name
    QString new_name = value.toString();
    // set new name
    param->setName(new_name.toStdString());
    // signal that data has changed:
    emit dataChanged(index, index);
    return true;
  }

  if (2 == index.column()) {
    // If the initial value was changed: get expression
    std::string expression = value.toString().toStdString();
    // parse expression
    GiNaC::ex new_value;
    try { new_value = Fluc::Parser::Expr::parseExpression(expression, _kinetic_law); }
    catch (Fluc::Exception &err) {
      Fluc::Utils::Message msg = LOG_MESSAGE(Fluc::Utils::Message::INFO);
      msg << "Can not parse expression: " << expression << ": " << err.what();
      Fluc::Utils::Logger::get().log(msg);
      return false;
    }
    // Set new "value"
    param->setValue(new_value);
    // Signal data changed:
    emit dataChanged(index, index);
    return true;
  }

  return false;
}


QVariant
ReactionParameterList::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (Qt::DisplayRole != role || orientation != Qt::Horizontal || 6 <= section) {
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  switch (section) {
  case 0: return QVariant("Id");
  case 1: return QVariant("Name");
  case 2: return QVariant("Value");
  case 3: return QVariant("Unit");
  case 4: return QVariant("Constant");
  default: break;
  }

  return QVariant();
}


int
ReactionParameterList::rowCount(const QModelIndex &parent) const
{
  return this->_kinetic_law->numParameters();
}


int
ReactionParameterList::columnCount(const QModelIndex &parent) const
{
  return 5;
}
