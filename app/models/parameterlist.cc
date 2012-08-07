#include "parameterlist.hh"
#include "exception.hh"
#include "parser/expr/parser.hh"
#include "utils/logger.hh"


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

  if ((Qt::DisplayRole != role) && (Qt::EditRole != role)) {
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


bool
ParameterList::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Filter invald indices
  if (index.row() >= int(model->numParameters())) return false;
  if (index.column() >= 5) return false;

  // Get paramter for index (row):
  Fluc::Ast::Parameter *param = model->getParameter(index.row());

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
    try { new_value = Fluc::Parser::Expr::parseExpression(expression, model); }
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


Qt::ItemFlags
ParameterList::flags(const QModelIndex &index) const
{
  // Default flags:
  Qt::ItemFlags item_flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  // Filter invalid indices:
  if (! index.isValid()) return Qt::NoItemFlags;
  if (5 <= index.column()) return Qt::NoItemFlags;
  if (int(model->numParameters()) <= index.row()) return Qt::NoItemFlags;

  // Mark only column 1 & 2 editable
  if ( (1 == index.column()) || (2 == index.column()) ) item_flags |= Qt::ItemIsEditable;

  return item_flags;
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
