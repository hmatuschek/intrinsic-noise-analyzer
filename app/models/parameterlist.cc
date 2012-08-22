#include "parameterlist.hh"
#include "exception.hh"
#include "parser/expr/parser.hh"
#include "utils/logger.hh"
#include "../tinytex/tinytex.hh"
#include "../tinytex/ginac2formula.hh"
#include "../views/unitrenderer.hh"


ParameterList::ParameterList(Fluc::Ast::Model *model, QObject *parent)
  : QAbstractTableModel(parent), _model(model)
{
  // Install some delegates for some columns:

}


QVariant
ParameterList::data(const QModelIndex &index, int role) const
{
  if (! index.isValid() || 5 <= index.column()) { return QVariant(); }
  if (rowCount() <= index.row()) { return QVariant(); }

  Fluc::Ast::Parameter *param = this->_model->getParameter(index.row());

  switch(index.column()) {
  case 0: return _getIdentifier(param, role);
  case 1: return _getName(param, role);
  case 2: return _getInitialValue(param, role);
  case 3: return _getUnit(param, role);
  case 4: return _getConstFlag(param, role);
  }

  return QVariant();
}


bool
ParameterList::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Filter invald indices
  if (rowCount() <= index.row()) return false;
  if (columnCount() <= index.column()) return false;

  // Get paramter for index (row):
  Fluc::Ast::Parameter *param = _model->getParameter(index.row());

  if (1 == index.column()) {
    if (_updateName(param, value)) {
      emit dataChanged(index, index);
      return true;
    }
  } else if (2 == index.column()) {
    if (_updateInitialValue(param, value)) {
      emit dataChanged(index, index);
      return true;
    }
    // Signal data changed:
    emit dataChanged(index, index);
    return true;
  }

  return false;
}


Qt::ItemFlags
ParameterList::flags(const QModelIndex &index) const
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
ParameterList::headerData(int section, Qt::Orientation orientation, int role) const
{
  // Return default header for rows:
  if (Qt::DisplayRole != role || orientation != Qt::Horizontal || columnCount() <= section) {
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  // handle column headers:
  switch (section) {
  case 0: return QVariant("Id");
  case 1: return QVariant("Name");
  case 2: return QVariant("Initial Value");
  case 3: return QVariant("Unit");
  case 4: return QVariant("Constant");
  default: break;
  }

  return QVariant();
}


int
ParameterList::rowCount(const QModelIndex &parent) const {
  return this->_model->numParameters();
}


int
ParameterList::columnCount(const QModelIndex &parent) const {
  return 5;
}

Fluc::Ast::Model &
ParameterList::model() {
  return *_model;
}


QVariant
ParameterList::_getIdentifier(Fluc::Ast::Parameter *param, int role) const
{
  if (Qt::DisplayRole != role) { return QVariant(); }

  return param->getIdentifier().c_str();
}

QVariant
ParameterList::_getName(Fluc::Ast::Parameter *param, int role) const
{
  if ((Qt::DisplayRole != role) && (Qt::EditRole != role)) { return QVariant(); }

  if (Qt::DisplayRole == role) {
    if (param->hasName()) {
      return TinyTex::toPixmap(param->getName().c_str());
    } else {
      return TinyTex::toPixmap("<none>");
    }
  } else {
    return param->getName().c_str();
  }

  return QVariant();
}

bool
ParameterList::_updateName(Fluc::Ast::Parameter *param, const QVariant &value)
{
  param->setName(value.toString().toStdString());
  return true;
}

QVariant
ParameterList::_getInitialValue(Fluc::Ast::Parameter *param, int role) const
{
  if ((Qt::DisplayRole != role) && (Qt::EditRole != role)) { return QVariant(); }

  if (Qt::DisplayRole == role) {
    // Render initial value:
    return Ginac2Formula::toPixmap(param->getValue(), *_model);
  } else {
    // Serialize expression for editing:
    std::stringstream buffer; buffer << param->getValue();
    return QString(buffer.str().c_str());
  }
}

bool
ParameterList::_updateInitialValue(Fluc::Ast::Parameter *param, const QVariant &value)
{
  // If the initial value was changed: get expression
  std::string expression = value.toString().toStdString();
  // parse expression
  GiNaC::ex new_value;
  try { new_value = Fluc::Parser::Expr::parseExpression(expression, _model); }
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
ParameterList::_getUnit(Fluc::Ast::Parameter *param, int role) const
{
  if ((Qt::DecorationRole != role)) { return QVariant(); }

  UnitRenderer renderer(param->getUnit());
  return renderer.toPixmap();
}

QVariant
ParameterList::_getConstFlag(Fluc::Ast::Parameter *param, int role) const {
  if ((Qt::CheckStateRole != role) && (Qt::EditRole != role)) { return QVariant(); }

  if (Qt::CheckStateRole == role) {
    if (param->isConst()) { return Qt::Checked; }
    return Qt::Unchecked;
  } else {
    return param->isConst();
  }

  return QVariant();
}


bool
ParameterList::_updateConstFlag(Fluc::Ast::Parameter *param, const QVariant &value)
{
  param->setConst(value.toBool());
  return true;
}


