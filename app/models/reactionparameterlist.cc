#include "reactionparameterlist.hh"
#include <sstream>
#include "parser/parser.hh"
#include "utils/logger.hh"
#include "exception.hh"
#include "../tinytex/tinytex.hh"
#include "../tinytex/ginac2formula.hh"
#include "../views/unitrenderer.hh"




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
  if (columnCount() <= index.column()) return Qt::NoItemFlags;
  if (rowCount() <= index.row()) return Qt::NoItemFlags;

  // Mark only column 1 & 2 editable
  if ( (1 == index.column()) || (2 == index.column()) ) item_flags |= Qt::ItemIsEditable;

  return item_flags;
}


QVariant
ReactionParameterList::data(const QModelIndex &index, int role) const
{
  // Filter invalid indices:
  if (! index.isValid() || columnCount() <= index.column()) { return QVariant(); }
  if (rowCount() <= index.row()) { return QVariant(); }

  // Get selected paramter:
  Fluc::Ast::Parameter *param = this->_kinetic_law->getParameter(index.row());

  // Dispatch by column:
  switch (index.column()) {
  case 0: return _getIdentifier(param, role);
  case 1: return _getName(param, role);
  case 2: return _getInitialValue(param, role);
  case 3: return _getUnit(param, role);
  default: break;
  }

  return QVariant();
}


bool
ReactionParameterList::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Filter invald indices
  if (index.row() >= rowCount()) return false;
  if (index.column() >= columnCount()) return false;

  // Get paramter for index (row):
  Fluc::Ast::Parameter *param = _kinetic_law->getParameter(index.row());

  // Dispatch...
  bool success = false;
  switch (index.column()) {
  case 1: success = _updateName(param, value); break;
  case 2: success = _updateInitialValue(param, value); break;
  default: break;
  }

  // Emit dataChanged on success
  if (success) { emit dataChanged(index, index); }
  return success;
}


QVariant
ReactionParameterList::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (Qt::DisplayRole != role || orientation != Qt::Horizontal || columnCount() <= section) {
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  switch (section) {
  case 0: return QVariant("Id");
  case 1: return QVariant("Name");
  case 2: return QVariant("Value");
  case 3: return QVariant("Unit");
  default: break;
  }

  return QVariant();
}


int
ReactionParameterList::rowCount(const QModelIndex &parent) const {
  return this->_kinetic_law->numParameters();
}


int
ReactionParameterList::columnCount(const QModelIndex &parent) const {
  return 4;
}


Fluc::Ast::KineticLaw &
ReactionParameterList::kineticLaw() {
  return *_kinetic_law;
}


QVariant
ReactionParameterList::_getIdentifier(Fluc::Ast::Parameter *param, int role) const
{
  if (Qt::DisplayRole != role) { return QVariant(); }
  return QString(param->getIdentifier().c_str());
}


QVariant
ReactionParameterList::_getName(Fluc::Ast::Parameter *param, int role) const
{
  if (Qt::DisplayRole == role) {
    if (param->hasName()) {
      return TinyTex::toPixmap(param->getName().c_str());
    }
    return TinyTex::toPixmap("<none>");
  }

  if (Qt::EditRole == role) {
    if (param->hasName()) {
      return QString(param->getName().c_str());
    }
    return QString("");
  }

  return QVariant();
}


bool
ReactionParameterList::_updateName(Fluc::Ast::Parameter *param, const QVariant &value)
{
  param->setName(value.toString().toStdString());
  return true;
}


QVariant
ReactionParameterList::_getInitialValue(Fluc::Ast::Parameter *param, int role) const
{
  // Try to render formula as pixmap
  if (Qt::DisplayRole == role) {
    if (! param->hasValue()) { return QVariant(); }
    return QVariant(Ginac2Formula::toPixmap(param->getValue(), *_kinetic_law));
  }

  if (Qt::EditRole == role) {
    // Export formula as string
    std::stringstream str;
    if (param->hasValue()) { str << param->getValue(); }
    return QString(str.str().c_str());
  }

  return QVariant();
}


bool
ReactionParameterList::_updateInitialValue(Fluc::Ast::Parameter *param, const QVariant &value)
{
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
  return true;
}


QVariant
ReactionParameterList::_getUnit(Fluc::Ast::Parameter *param, int role) const
{
  if ((Qt::DecorationRole != role)) { return QVariant(); }

  UnitRenderer renderer(param->getUnit());
  return renderer.toPixmap();
}


QVariant
ReactionParameterList::_getConstFlag(Fluc::Ast::Parameter *param, int role) const
{
  if (Qt::CheckStateRole != role) { return QVariant(); }
  if (param->isConst()) { return Qt::Checked; }
  return Qt::Unchecked;
}


bool
ReactionParameterList::_updateConstFlag(Fluc::Ast::Parameter *param, const QVariant &value)
{
  if (value == Qt::Checked) {
    param->setConst(true);
  } else {
    param->setConst(false);
  }
  return true;
}
