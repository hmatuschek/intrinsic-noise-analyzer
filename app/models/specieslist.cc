#include "specieslist.hh"
#include "parser/parser.hh"
#include "parser/unit/unitparser.hh"
#include "exception.hh"
#include "utils/logger.hh"
#include "referencecounter.hh"
#include "variableruledata.hh"
#include "../views/unitrenderer.hh"
#include "../tinytex/tinytex.hh"
#include "../tinytex/ginac2formula.hh"
#include "ast/identifier.hh"
#include <QMessageBox>
#include <QPainter>
#include <QDebug>



SpeciesList::SpeciesList(iNA::Ast::Model *model, QObject *parent)
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

  if (0==index.column() || 1==index.column() || 2==index.column() || 5==index.column()) {
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
  iNA::Ast::Species *spec = this->_model->getSpecies(index.row());

  switch (index.column()) {
  case 0: return _getIdentifier(spec, role);
  case 1: return _getName(spec, role);
  case 2: return _getInitialValue(spec, role);
  case 3: return _getUnit(spec, role);
  case 4: return _getConstFlag(spec, role);
  case 5: return _getCompartment(spec, role);
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
  iNA::Ast::Species *species = _model->getSpecies(index.row());

  // Dispatch by column:
  bool success=false;
  switch (index.column()) {
  case 0: success = _updateIdentifier(species, value); break;
  case 1: success = _updateName(species, value); break;
  case 2: success = _updateInitialValue(species, value); break;
  //case 3: success = _updateUnit(species, value); break;
  case 4: success = _updateConstFlag(species, value); break;
  case 5: success = _updateCompartment(species, value); break;
  default: break;
  }

  if (success) {
    emit dataChanged(index, index);
    emit modelModified();
  }
  return success;
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
  //case 6: return QVariant("Rule");
  default: break;
  }

  return QVariant();
}


int SpeciesList::rowCount(const QModelIndex &parent) const { return _model->numSpecies(); }
int SpeciesList::columnCount(const QModelIndex &parent) const { return 6; }

iNA::Ast::Model & SpeciesList::model() { return *_model; }


QVariant
SpeciesList::_getIdentifier(iNA::Ast::Species *species, int role) const {
  if ((Qt::DisplayRole != role) && (Qt::EditRole != role)) { return QVariant(); }
  return QVariant(species->getIdentifier().c_str());
}

bool
SpeciesList::_updateIdentifier(iNA::Ast::Species *species, const QVariant &value)
{
  // Get ID
  QString qid = value.toString();
  std::string id = qid.toStdString();

  // If nothing changed -> done.
  if (id == species->getIdentifier()) { return true; }

  // Check ID format
  if (! iNA::Ast::isValidId(id)) {
    QMessageBox::warning(0, tr("Can not set identifier"),
                         tr("Identifier \"%1\" has invalid format.").arg(qid));
    return false;
  }

  // Check if id is not assigned allready:
  if (_model->hasDefinition(id)) {
    QMessageBox::warning(0, tr("Can not set identifier"),
                         tr("Identifier \"%1\" already in use.").arg(qid));
    return false;
  }

  // Ok, assign identifier:
  _model->resetIdentifier(species->getIdentifier(), id);
  return true;
}


QVariant
SpeciesList::_getName(iNA::Ast::Species *species, int role) const
{
  if ( (Qt::DisplayRole != role) && (Qt::EditRole != role) ) { return QVariant(); }

  if (Qt::EditRole == role) {
    return QString::fromStdString(species->getName());
  } else {
    if (species->hasName()) {
      return TinyTex::toPixmap(species->getName());
    } else {
      return TinyTex::toPixmap("<none>");
    }
  }

  return QVariant();
}


bool
SpeciesList::_updateName(iNA::Ast::Species *species, const QVariant &value)
{
  // Debug message:
  iNA::Utils::Message msg = LOG_MESSAGE(iNA::Utils::Message::DEBUG);
  msg << "Update species name from " << species->getName()
      << " to " << value.toString().toStdString();
  iNA::Utils::Logger::get().log(msg);

  // If name is changed, get new name
  QString new_name = value.toString();
  // set new name
  species->setName(new_name.toStdString());
  // All ok;
  return true;
}

QVariant
SpeciesList::_getInitialValue(iNA::Ast::Species *species, int role) const
{
  // filter by display role
  if ( (Qt::EditRole != role) && (Qt::DisplayRole != role))
  { return QVariant(); }

  // Try to render formula as pixmap
  if (Qt::DisplayRole == role) {
    if (! species->hasValue()) { return QVariant(); }
    return QVariant(Ginac2Formula::toPixmap(species->getValue(), *_model));
  }

  // Export formula as string
  std::stringstream buffer;
  if (species->hasValue()) {
    iNA::Parser::Expr::serializeExpression(species->getValue(), buffer, _model);
  }
  return QString(buffer.str().c_str());
}

bool
SpeciesList::_updateInitialValue(iNA::Ast::Species *species, const QVariant &value)
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
    // Show message
    QMessageBox::warning(
          0, tr("Cannot parse expression"),
          tr("Cannot parse expression \"%1\": %2").arg(value.toString(), err.what()));
    return false;
  }

  // Set new "value"
  species->setValue(new_value);
  return true;
}


QVariant
SpeciesList::_getUnit(iNA::Ast::Species *species, int role) const
{
  if ((Qt::DecorationRole != role)) { return QVariant(); }

  UnitRenderer renderer(_model->getSpeciesUnit());
  return renderer.toPixmap();
}


bool
SpeciesList::_updateUnit(iNA::Ast::Species *species, const QVariant &value)
{
  return false;
}


QVariant
SpeciesList::_getConstFlag(iNA::Ast::Species *species, int role) const
{
  if (Qt::CheckStateRole != role) { return QVariant(); }
  if (species->isConst()) { return Qt::Checked; }
  return Qt::Unchecked;
}

bool
SpeciesList::_updateConstFlag(iNA::Ast::Species *species, const QVariant &value)
{
  if (value == Qt::Checked) {
    species->setConst(true);
  } else {
    species->setConst(false);
  }
  return true;
}


QVariant
SpeciesList::_getCompartment(iNA::Ast::Species *species, int role) const
{
  if ( (Qt::DecorationRole != role) && (Qt::EditRole != role)) { return QVariant(); }

  QString id = species->getCompartment()->getIdentifier().c_str();

  if ( Qt::DecorationRole == role ) {
    if (species->getCompartment()->hasName()) {
      return TinyTex::toPixmap(species->getCompartment()->getName());
    }
    return TinyTex::toPixmap(id.toStdString());
  }
  return id;
}

bool
SpeciesList::_updateCompartment(iNA::Ast::Species *species, const QVariant &value)
{
  // Get compartment by identifier:
  if (! _model->hasCompartment(value.toString().toStdString())) { return false; }
  iNA::Ast::Compartment *compartment =
      _model->getCompartment(value.toString().toStdString());
  species->setCompartment(compartment);
  return true;
}


QVariant
SpeciesList::_getRule(iNA::Ast::Species *species, int role) const
{
  if (Qt::DecorationRole != role) {  return QVariant(); }

  if (species->hasRule()) {
    // Translate ginac expression into formula:
    MathFormula *formula = new MathFormula();
    if (iNA::Ast::Node::isAssignmentRule(species->getRule())) {
      formula->appendItem(
            Ginac2Formula::toFormula(species->getSymbol(), *_model));
      formula->appendItem(new MathText("="));
    } else {
      formula->appendItem(
            new MathSub(new MathText(QChar(0x2202)), new MathText("t")));
      formula->appendItem(
            Ginac2Formula::toFormula(species->getSymbol(), *_model));
      formula->appendItem(new MathText("="));
    }
    formula->appendItem(Ginac2Formula::toFormula(
                          species->getRule()->getRule(), *_model));

    // Draw formula into pixmap:
    QGraphicsItem *rendered = formula->layout(MathContext());
    QGraphicsScene *scene = new QGraphicsScene();
    scene->addItem(rendered);
    QSize size = scene->sceneRect().size().toSize();
    QPixmap pixmap(size.width(), size.height());
    QPainter painter(&pixmap);
    painter.fillRect(0,0, size.width(), size.height(), QColor(255,255,255));
    scene->render(&painter);
    delete formula; delete scene;
    qDebug() << "Return pixmap for rule: " << pixmap.rect();
    return pixmap;
  } else {
    return QVariant(TinyTex::toPixmap("<none>"));
  }
}

bool
SpeciesList::_updateRule(iNA::Ast::Species *species, const QVariant &value)
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
  try { new_rule_expression = iNA::Parser::Expr::parseExpression(expression, _model); }
  catch (iNA::Exception &err) {
    iNA::Utils::Message msg = LOG_MESSAGE(iNA::Utils::Message::INFO);
    msg << "Can not parse expression: " << expression << ": " << err.what();
    iNA::Utils::Logger::get().log(msg);
    delete data; return false;
  }

  // Assemble rule object:
  iNA::Ast::Rule *rule = 0;
  if (VariableRuleData::ASSIGNMENT_RULE == data->ruleKind()) {
    rule = new iNA::Ast::AssignmentRule(new_rule_expression);
  } else {
    rule = new iNA::Ast::RateRule(new_rule_expression);
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
  iNA::Ast::Compartment *compartment = _model->getCompartment(0);
  std::string identifier = _model->getNewIdentifier("species");

  // Signal views and add sepecies:
  int new_idx = _model->numSpecies();
  beginInsertRows(QModelIndex(), new_idx, new_idx);
  _model->addDefinition(
        new iNA::Ast::Species(identifier, compartment, false));
  endInsertRows();

  // signal model modified
  emit modelModified();
}


void
SpeciesList::remSpecies(int row)
{
  if (row >= int(_model->numSpecies())) { return; }

  // Get Species and count its references:
  iNA::Ast::Species *species = _model->getSpecies(row);
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

  // signal model modified
  emit modelModified();
}
