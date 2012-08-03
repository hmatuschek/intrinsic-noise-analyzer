#include "specieslist.hh"
#include "parser/parser.hh"
#include "exception.hh"
#include "utils/logger.hh"


SpeciesList::SpeciesList(Fluc::Ast::Model *model, QObject *parent)
  : QAbstractTableModel(parent), model(model)
{
  // Pass...
}


Qt::ItemFlags
SpeciesList::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags =  Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  // Filter invalid indices:
  if (! index.isValid() || 6 <= index.column()) { return Qt::NoItemFlags; }
  if (int(this->model->numSpecies()) <= index.row()) { return Qt::NoItemFlags; }

  if (1 == index.column() || 2 == index.column()) { flags |= Qt::ItemIsEditable; }

  return flags;
}


QVariant
SpeciesList::data(const QModelIndex &index, int role) const
{
  // Filter invalid indices:
  if (! index.isValid() || 6 <= index.column()) { return QVariant(); }
  if (int(this->model->numSpecies()) <= index.row()) { return QVariant(); }

  // Get selected species by row
  Fluc::Ast::Species *spec = this->model->getSpecies(index.row());

  // Handle constant flag:
  if (4 == index.column() && Qt::CheckStateRole == role) {
    if (spec->isConst()) {
      return Qt::Checked;
    }
    return Qt::Unchecked;
  }

  // Handle display role:
  if (Qt::DisplayRole == role) {
    // Handle identifier
    if (0 == index.column()) { return QVariant(spec->getIdentifier().c_str()); }
    // Handle name
    if (1 == index.column()) {
      if (! spec->hasName())
        return QVariant("<not set>");
      return QVariant(spec->getName().c_str());
    }
    // Handle initial value
    if (2 == index.column()) { return QVariant(this->getInitialValueForSpecies(spec)); }
    // Handle unit
    if (3 == index.column()) {
      std::stringstream str; spec->getUnit().dump(str);
      return QVariant(str.str().c_str());
    }
    // Handle compartment
    if (5 == index.column()) {
      if (spec->getCompartment()->hasName())
        return QVariant(spec->getCompartment()->getName().c_str());
      return QVariant(spec->getCompartment()->getIdentifier().c_str());
    }
  }

  // Handle edit role:
  if (Qt::EditRole == role) {
    // Handle name
    if (1 == index.column()) {
      if (! spec->hasName()) { return QVariant(""); }
      return QVariant(spec->getName().c_str());
    }
    // Handle initial value
    if (2 == index.column()) { return QVariant(this->getInitialValueForSpecies(spec)); }
  }

  return QVariant();
}


bool
SpeciesList::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Filter invalid items:
  if (index.row() >= int(model->numParameters())) return false;
  if (index.column() >= 5) return false;

  // Get paramter for index (row):
  Fluc::Ast::Species *species = model->getSpecies(index.row());

  if (1 == index.column()) {
    // If name is changed, get new name
    QString new_name = value.toString();
    // set new name
    species->setName(new_name.toStdString());
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
    species->setValue(new_value);
    // Signal data changed:
    emit dataChanged(index, index);
    return true;
  }

  return false;
}


QVariant
SpeciesList::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (Qt::DisplayRole != role || orientation != Qt::Horizontal || 6 <= section) {
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  switch (section) {
  case 0: return QVariant("Id");
  case 1: return QVariant("Name");
  case 2: return QVariant("Initial Value");
  case 3: return QVariant("Unit");
  case 4: return QVariant("Constant");
  case 5: return QVariant("Compartment");
  default: break;
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

