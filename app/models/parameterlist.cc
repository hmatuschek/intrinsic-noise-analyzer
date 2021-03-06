#include "parameterlist.hh"
#include "exception.hh"
#include "parser/expr/parser.hh"
#include "parser/unit/unitparser.hh"
#include "utils/logger.hh"
#include "../tinytex/tinytex.hh"
#include "../tinytex/ginac2formula.hh"
#include "../views/unitrenderer.hh"
#include "referencecounter.hh"
#include "ast/identifier.hh"
#include <QMessageBox>


ParameterList::ParameterList(iNA::Ast::Model *model, QObject *parent)
  : QAbstractTableModel(parent), _model(model)
{
  // pass...
}


QVariant
ParameterList::data(const QModelIndex &index, int role) const
{
  if (! index.isValid() || 5 <= index.column()) { return QVariant(); }
  if (rowCount() <= index.row()) { return QVariant(); }

  iNA::Ast::Parameter *param = this->_model->getParameter(index.row());

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
  iNA::Ast::Parameter *param = _model->getParameter(index.row());

  // Dispatch
  bool success = false;
  switch (index.column()) {
  case 0: success = _updateIdentifier(param, value); break;
  case 1: success = _updateName(param, value); break;
  case 2: success = _updateInitialValue(param, value); break;
  case 3: success = _updateUnit(param, value); break;
  case 4: success = _updateConstFlag(param, value); break;
  default: break;
  }

  // Emmit data-changed on success:
  if (success) {
    emit dataChanged(index, index);
    emit modelModified();
  }

  // done.
  return success;
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

  // Mark only column 1, 2, 3 & 4 editable
  if ( (0 == index.column()) || (1 == index.column()) || (2 == index.column()) || (3== index.column()))
    item_flags |= Qt::ItemIsEditable;

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

iNA::Ast::Model &
ParameterList::model() {
  return *_model;
}

void
ParameterList::addParameter() {
  // Get new, unique identifier for parameter
  std::string identifier = _model->getNewIdentifier("parameter");
  // Get index of new parameter
  int new_idx = _model->numParameters();

  // Add paramter to model and update table
  beginInsertRows(QModelIndex(), new_idx, new_idx);
  _model->addDefinition(
        new iNA::Ast::Parameter(identifier, 0, iNA::Ast::Unit::dimensionless(), true));
  endInsertRows();

  // Signal that the model was modified
  emit modelModified();
}


void
ParameterList::remParameter(int row)
{
  // skip invalid rows
  if ((0 > row) || (row >= int(_model->numParameters()))) { return; }

  // Get param and count its references:
  iNA::Ast::Parameter *parameter = _model->getParameter(row);
  ReferenceCounter refs(parameter); _model->accept(refs);

  // Show message id
  if (0 < refs.references().size()) {
    QMessageBox::information(
          0, tr("Can not delete parameter."),
          tr("Can not delete parameter as it is referenced %1").arg(
            QStringList(refs.references()).join(", ")));
    return;
  }

  // otherwise, remove parameter
  beginRemoveRows(QModelIndex(), row, row);
  _model->remDefinition(parameter);
  endRemoveRows();

  // Signal that the model was modified
  emit modelModified();
}


QVariant
ParameterList::_getIdentifier(iNA::Ast::Parameter *param, int role) const
{
  if ((Qt::DisplayRole != role) && (Qt::EditRole != role)) { return QVariant(); }

  return param->getIdentifier().c_str();
}

bool
ParameterList::_updateIdentifier(iNA::Ast::Parameter *param, const QVariant &value)
{
  // Get ID
  QString qid = value.toString();
  std::string id = qid.toStdString();

  // If nothing changed -> done.
  if (id == param->getIdentifier()) { return true; }

  // Check ID format
  if (! iNA::Ast::isValidId(id)) {
    QMessageBox::warning(0, tr("Can not set identifier"),
                         tr("Identifier \"%1\" has invalid format.").arg(qid));
    return false;
  }

  // Check if id is not assigned allready:
  if (_model->hasDefinition(id)) {
    QMessageBox::warning(0, tr("Can not set identifier"),
                         tr("Identifier \"%1\" is already in use.").arg(qid));
    return false;
  }

  // Ok, assign identifier:
  _model->resetIdentifier(param->getIdentifier(), id);
  return true;
}


QVariant
ParameterList::_getName(iNA::Ast::Parameter *param, int role) const
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
ParameterList::_updateName(iNA::Ast::Parameter *param, const QVariant &value)
{
  param->setName(value.toString().toStdString());
  return true;
}

QVariant
ParameterList::_getInitialValue(iNA::Ast::Parameter *param, int role) const
{
  if ((Qt::DisplayRole != role) && (Qt::EditRole != role)) { return QVariant(); }

  if (Qt::DisplayRole == role) {
    // Render initial value:
    return Ginac2Formula::toPixmap(param->getValue(), *_model);
  } else {
    // Serialize expression for editing:
    std::stringstream buffer;
    iNA::Parser::Expr::serializeExpression(param->getValue(), buffer, _model);
    return QString(buffer.str().c_str());
  }
}

bool
ParameterList::_updateInitialValue(iNA::Ast::Parameter *param, const QVariant &value)
{
  // If the initial value was changed: get expression
  std::string expression = value.toString().toStdString();
  // parse expression
  GiNaC::ex new_value;
  try { new_value = iNA::Parser::Expr::parseExpression(expression, _model); }
  catch (iNA::Exception &err) {
    // Log message
    iNA::Utils::Message msg = LOG_MESSAGE(iNA::Utils::Message::ERROR);
    msg << "Cannot parse expression: " << expression << ": " << err.what();
    iNA::Utils::Logger::get().log(msg);
    // Show message:
    QMessageBox::warning(
          0, tr("Can not set initial value"),
          tr("Cannot parse expression \"%1\": %2").arg(value.toString(), err.what()));
    return false;
  }
  // Set new "value"
  param->setValue(new_value);
  return true;
}

QVariant
ParameterList::_getUnit(iNA::Ast::Parameter *param, int role) const
{
  if (Qt::DecorationRole == role) {
    // Render unit as pixmap
    UnitRenderer renderer(param->getUnit());
    return renderer.toPixmap();
  } else if (Qt::EditRole == role) {
    // Serialize unit to string:
    return QString(iNA::Parser::Unit::UnitParser::write(param->getUnit()).c_str());
  }
  // Unknown role:
  return QVariant();
}

bool
ParameterList::_updateUnit(iNA::Ast::Parameter *param, const QVariant &value)
{
  // If unit expression is empty -> set to dimensionless:
  if (0 == value.toString().size()) {
    param->setUnit(iNA::Ast::Unit::dimensionless());
    return true;
  }

  // Parse Unit;
  iNA::Ast::Unit unit;
  try {
    unit = iNA::Parser::Unit::UnitParser::parse(value.toString().toStdString());
  } catch (iNA::Exception &err) {
    // Log message
    iNA::Utils::Message message = LOG_MESSAGE(iNA::Utils::Message::WARN);
    message << "Can not parse unit expression: " << err.what();
    iNA::Utils::Logger::get().log(message);
    // Show message
    QMessageBox::warning(
          0, tr("Can not parse unit expression"),
          tr("Can not parse unit expression \"%1\": %2").arg(value.toString(), err.what()));
    return false;
  }
  // Set parsed unit and signal success:
  param->setUnit(unit);
  return true;
}


QVariant
ParameterList::_getConstFlag(iNA::Ast::Parameter *param, int role) const {
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
ParameterList::_updateConstFlag(iNA::Ast::Parameter *param, const QVariant &value)
{
  param->setConst(value.toBool());
  return true;
}


