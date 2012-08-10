#include "specieslist.hh"
#include "parser/parser.hh"
#include "exception.hh"
#include "utils/logger.hh"
#include "variableruledata.hh"
#include "referencecounter.hh"

#include <QMessageBox>



SpeciesList::SpeciesList(Fluc::Ast::Model *model, QObject *parent)
  : QAbstractTableModel(parent), _model(model)
{
  // Pass...
}


Qt::ItemFlags
SpeciesList::flags(const QModelIndex &index) const
{
  Qt::ItemFlags flags =  Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  // Filter invalid indices:
  if (! index.isValid() || columnCount() <= index.column()) { return Qt::NoItemFlags; }
  if (int(this->_model->numSpecies()) <= index.row()) { return Qt::NoItemFlags; }

  if (1==index.column() || 2==index.column() ||
      5==index.column() || 6==index.column()) {
    flags |= Qt::ItemIsEditable;
  }

  if (4 == index.column()) {
    flags |= Qt::ItemIsUserCheckable;
  }

  return flags;
}


QVariant
SpeciesList::data(const QModelIndex &index, int role) const
{
  // Filter invalid indices:
  if (! index.isValid() || columnCount() <= index.column()) { return QVariant(); }
  if (int(this->_model->numSpecies()) <= index.row()) { return QVariant(); }

  // Get selected species by row
  Fluc::Ast::Species *spec = this->_model->getSpecies(index.row());

  switch (index.column()) {
  case 0: return _getIdentifier(spec, role);
  case 1: return _getName(spec, role);
  case 2: return _getInitialValue(spec, role);
  case 3: return _getUnit(spec, role);
  case 4: return _getConstFlag(spec, role);
  case 5: return _getCompartment(spec, role);
  case 6: return _getRule(spec, role);
  default: break;
  }

  return QVariant();
}


bool
SpeciesList::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Filter invalid items:
  if (index.row() >= int(_model->numSpecies())) return false;
  if (columnCount() <= index.column()) return false;

  // Get paramter for index (row):
  Fluc::Ast::Species *species = _model->getSpecies(index.row());

  // Dispatch by column:
  switch (index.column()) {
  case 1:
    if (_updateName(species, value)) { emit dataChanged(index, index); return true; }
    break;

  case 2:
    if (_updateInitialValue(species, value)) { emit dataChanged(index, index); return true; }
    break;

  case 4:
    if (_updateConstFlag(species, value)) { emit dataChanged(index, index); return true; }
    break;

  case 5:
    if (_updateCompartment(species, value)) { emit dataChanged(index, index); return true; }
    break;

  case 6:
    if (_updateRule(species, value)) { emit dataChanged(index, index); return true; }
    break;

  default: break;
  }

  return false;
}


QVariant
SpeciesList::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (Qt::DisplayRole != role || orientation != Qt::Horizontal || columnCount() <= section) {
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  switch (section) {
  case 0: return QVariant("Id");
  case 1: return QVariant("Name");
  case 2: return QVariant("Initial Value");
  case 3: return QVariant("Unit");
  case 4: return QVariant("Constant");
  case 5: return QVariant("Compartment");
  case 6: return QVariant("Rule");
  default: break;
  }

  return QVariant();
}


int SpeciesList::rowCount(const QModelIndex &parent) const { return _model->numSpecies(); }
int SpeciesList::columnCount(const QModelIndex &parent) const { return 7; }

Fluc::Ast::Model & SpeciesList::model() { return *_model; }


QVariant
SpeciesList::_getIdentifier(Fluc::Ast::Species *species, int role) const {
  if (Qt::DisplayRole != role) { return QVariant(); }
  return QVariant(species->getIdentifier().c_str());
}


QVariant
SpeciesList::_getName(Fluc::Ast::Species *species, int role) const
{
  if ( (Qt::DisplayRole != role) && (Qt::EditRole != role) ) { return QVariant(); }

  if (species->hasName()) {
    return QString(species->getName().c_str());
  } else {
    if (Qt::DisplayRole == role) { return QString("<none>"); }
  }

  return QString();
}

bool
SpeciesList::_updateName(Fluc::Ast::Species *species, const QVariant &value)
{
  // Debug message:
  Fluc::Utils::Message msg = LOG_MESSAGE(Fluc::Utils::Message::DEBUG);
  msg << "Update species name from " << species->getName()
      << " to " << value.toString().toStdString();
  Fluc::Utils::Logger::get().log(msg);

  // If name is changed, get new name
  QString new_name = value.toString();
  // set new name
  species->setName(new_name.toStdString());
  // All ok;
  return true;
}

QVariant
SpeciesList::_getInitialValue(Fluc::Ast::Species *species, int role) const
{
  if ( (Qt::EditRole != role) && (Qt::DisplayRole != role) ) { return QVariant(); }
  std::stringstream str;
  if (species->hasValue())
    str << species->getValue();
  QString init_val(str.str().c_str());
  return init_val;
}

bool
SpeciesList::_updateInitialValue(Fluc::Ast::Species *species, const QVariant &value)
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
  species->setValue(new_value);
  return true;
}


QVariant
SpeciesList::_getUnit(Fluc::Ast::Species *species, int role) const
{
  if (Qt::DisplayRole != role) { return QVariant(); }

  std::stringstream str; species->getUnit().dump(str);
  return QVariant(str.str().c_str());
}


QVariant
SpeciesList::_getConstFlag(Fluc::Ast::Species *species, int role) const
{
  if (Qt::CheckStateRole != role) { return QVariant(); }
  if (species->isConst()) { return Qt::Checked; }
  return Qt::Unchecked;
}

bool
SpeciesList::_updateConstFlag(Fluc::Ast::Species *species, const QVariant &value)
{
  if (value == Qt::Checked) {
    species->setConst(true);
  } else {
    species->setConst(false);
  }
  return true;
}


QVariant
SpeciesList::_getCompartment(Fluc::Ast::Species *species, int role) const
{
  if ( (Qt::DisplayRole != role) && (Qt::EditRole != role)) { return QVariant(); }
  QString id = species->getCompartment()->getIdentifier().c_str();
  if ( (Qt::DisplayRole == role) && species->getCompartment()->hasName()) {
    return QString("%1 (%2)").arg(species->getCompartment()->getName().c_str(), id);
  }
  return id;
}

bool
SpeciesList::_updateCompartment(Fluc::Ast::Species *species, const QVariant &value)
{
  // Get compartment by identifier:
  if (! _model->hasCompartment(value.toString().toStdString())) { return false; }
  Fluc::Ast::Compartment *compartment =
      _model->getCompartment(value.toString().toStdString());
  species->setCompartment(compartment);
  return true;
}


QVariant
SpeciesList::_getRule(Fluc::Ast::Species *species, int role) const
{
  if (Qt::DisplayRole != role) {  return QVariant(); }

  if (species->hasRule()) {
    std::stringstream stream; stream << species->getRule()->getRule();
    if (Fluc::Ast::Node::isAssignmentRule(species->getRule())) {
      return QVariant(QString("%1=%2").arg(species->getIdentifier().c_str(), stream.str().c_str()));
    } else {
      return QVariant(QString("d%1/dt=%2").arg(species->getIdentifier().c_str(), stream.str().c_str()));
    }
  } else {
    return QVariant("<none>");
  }
}

bool
SpeciesList::_updateRule(Fluc::Ast::Species *species, const QVariant &value)
{
  // Get variable rule data:
  VariableRuleData *data = (VariableRuleData *)(value.value<void *>());

  // No rule is set / present rule being deleted:
  if (VariableRuleData::NO_RULE == data->ruleKind()) {
    if (species->hasRule()) {
      delete species->getRule();
      species->setRule(0);
    }
    // Signal which element has changed
    delete data; return true;
  }

  // Parse expression in module:
  std::string expression = data->ruleExpression().toStdString();
  GiNaC::ex new_rule_expression;
  try { new_rule_expression = Fluc::Parser::Expr::parseExpression(expression, _model); }
  catch (Fluc::Exception &err) {
    Fluc::Utils::Message msg = LOG_MESSAGE(Fluc::Utils::Message::INFO);
    msg << "Can not parse expression: " << expression << ": " << err.what();
    Fluc::Utils::Logger::get().log(msg);
    delete data; return false;
  }

  // Assemble rule object:
  Fluc::Ast::Rule *rule = 0;
  if (VariableRuleData::ASSIGNMENT_RULE == data->ruleKind()) {
    rule = new Fluc::Ast::AssignmentRule(new_rule_expression);
  } else {
    rule = new Fluc::Ast::RateRule(new_rule_expression);
  }

  // Replace rule in species:
  if (species->hasRule()) {
    delete species->getRule();
  }
  species->setRule(rule);
  return true;
}


void
SpeciesList::addSpecies()
{
  // check if there exists at least one compartment in the model:
  if (0 == _model->numCompartments()) {
    QMessageBox::information(0, tr("Can not create species."),
                             tr("You must define at least one compartment first."));
    return;
  }

  // Get compartment and new unique identifier:
  Fluc::Ast::Compartment *compartment = _model->getCompartment(0);
  std::string identifier = _model->getNewIdentifier("species");

  // Signal views and add sepecies:
  int new_idx = _model->numSpecies();
  beginInsertRows(QModelIndex(), new_idx, new_idx);
  _model->addDefinition(
        new Fluc::Ast::Species(
          identifier, _model->getDefaultSubstanceUnit(), compartment, false));
  endInsertRows();
}


void
SpeciesList::remSpecies(int row)
{
  if (row >= int(_model->numSpecies())) { return; }

  // Get Species and count its references:
  Fluc::Ast::Species *species = _model->getSpecies(row);
  ReferenceCounter refs(species); _model->accept(refs);

  // Show message id
  if (0 < refs.references().size()) {
    QMessageBox::information(
          0, tr("Can not delete species."),
          tr("Can not delete species as it is referenced %1").arg(
            QStringList(refs.references()).join(", ")));
    return;
  }

  // otherwise, remove species
  beginRemoveRows(QModelIndex(), row, row);
  _model->remDefinition(species);
  endRemoveRows();
}
