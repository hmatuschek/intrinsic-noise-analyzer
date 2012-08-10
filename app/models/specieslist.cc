#include "specieslist.hh"
#include "parser/parser.hh"
#include "exception.hh"
#include "utils/logger.hh"
#include "variableruledata.hh"
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

  if (1==index.column() || 2==index.column() || 5==index.column() || 6==index.column()) {
    flags |= Qt::ItemIsEditable;
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
    // Handle rules:
    if (6 == index.column()) {
      if (spec->hasRule()) {
        std::stringstream stream; stream << spec->getRule()->getRule();
        if (Fluc::Ast::Node::isAssignmentRule(spec->getRule())) {
          return QVariant(QString("%1=%2").arg(spec->getIdentifier().c_str(), stream.str().c_str()));
        } else {
          return QVariant(QString("d%1/dt=%2").arg(spec->getIdentifier().c_str(), stream.str().c_str()));
        }
      } else {
        return QVariant("<none>");
      }
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
    // Handle compartment value:
    if (5 == index.column()) { return QVariant(spec->getCompartment()->getIdentifier().c_str()); }
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
  if (1 == index.column()) {
    if (_updateName(species, value)) {
      emit dataChanged(index, index); return true;
    }
    return false;
  } else if (2 == index.column()) {
    if (_updateInitialValue(species, value)) {
      emit dataChanged(index, index); return true;
    }
    return false;
  } else if (5 == index.column()) {
    if (_updateCompartment(species, value)) {
      emit dataChanged(index, index); return true;
    }
    return true;
  } else if (6 == index.column()) {
    if (_updateRule(species, value)) {
      emit dataChanged(index, index); return true;
    }
    return false;
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


QString
SpeciesList::getInitialValueForSpecies(Fluc::Ast::Species *spec) const
{
  std::stringstream str;
  if (spec->hasValue())
    str << spec->getValue();
  QString init_val(str.str().c_str());
  return init_val;
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

  QMessageBox::information(0, tr("Not implemented yet."), tr(".. working on it."));
}
