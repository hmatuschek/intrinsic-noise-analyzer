#include "compartmentlist.hh"
#include "parser/parser.hh"
#include "exception.hh"
#include "utils/logger.hh"


CompartmentList::CompartmentList(Fluc::Ast::Model *model, QObject *parent)
  : QAbstractTableModel(parent), _model(model)
{
  // Pass...
}


Qt::ItemFlags
CompartmentList::flags(const QModelIndex &index) const
{
  // Default flags:
  Qt::ItemFlags item_flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  // Filter invalid indices:
  if (! index.isValid()) return Qt::NoItemFlags;
  if (5 <= index.column()) return Qt::NoItemFlags;
  if (int(_model->numParameters()) <= index.row()) return Qt::NoItemFlags;

  // Mark only column 1 & 2 editable
  if ( (1 == index.column()) || (2 == index.column()) ) item_flags |= Qt::ItemIsEditable;

  return item_flags;
}


QVariant
CompartmentList::data(const QModelIndex &index, int role) const
{
  // Filter invalid indices:
  if (! index.isValid() || 5 <= index.column()) { return QVariant(); }
  if (int(this->_model->numCompartments()) <= index.row()) { return QVariant(); }

  // Get selected compartment:
  Fluc::Ast::Compartment *comp = this->_model->getCompartment(index.row());

  // handle is-const flag:
  if (4 == index.column() && Qt::CheckStateRole == role) {
    if (comp->isConst()) { return Qt::Checked; }
    return Qt::Unchecked;
  }

  // Return data for display:
  if (Qt::DisplayRole == role) {
    // on identifier
    if (0 == index.column()) { return QVariant(comp->getIdentifier().c_str()); }
    // on name
    if (1 == index.column()) {
      if (! comp->hasName())
        return QVariant("<not set>");
      return QVariant(comp->getName().c_str());
    }
    // on initial value:
    if (2 == index.column()) { return QVariant(this->getInitialValueForCompartment(comp)); }
    // on unit
    if (3 == index.column()) {
      std::stringstream str; comp->getUnit().dump(str);
      return QVariant(str.str().c_str());
    }
    // else
    return QVariant();
  } else if (Qt::EditRole == role) {
    // on name
    if (1 == index.column()) {
      if (! comp->hasName())
        return QVariant("<not set>");
      return QVariant(comp->getName().c_str());
    }
    // on initial value
    if (2 == index.column()) {
      return QVariant(this->getInitialValueForCompartment(comp));
    }
  }

  return QVariant();
}


bool
CompartmentList::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Filter invald indices:
  if (index.row() >= int(_model->numCompartments())) return false;
  if (index.column() >= 5) return false;

  // Get compartment:
  Fluc::Ast::Compartment *comp = _model->getCompartment(index.row());

  // If name is set
  if (1 == index.column()) {
    comp->setName(value.toString().toStdString());
    emit dataChanged(index, index);
    return true;
  }

  // If initial value is set:
  if (2 == index.column()) {
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
    comp->setValue(new_value);
    // Signal data changed:
    emit dataChanged(index, index);
    return true;
  }

  return false;
}


QVariant
CompartmentList::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (Qt::DisplayRole != role || orientation != Qt::Horizontal || 5 <= section) {
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  switch (section) {
  case 0: return QVariant("Id");
  case 1: return QVariant("Name");
  case 2: return QVariant("Volume");
  case 3: return QVariant("Unit");
  case 4: return QVariant("Constant");
  default: break;
  }

  return QVariant();
}


int
CompartmentList::rowCount(const QModelIndex &parent) const
{
  return this->_model->numCompartments();
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
