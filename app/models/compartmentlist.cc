#include "compartmentlist.hh"
#include "parser/parser.hh"
#include "parser/unit/unitparser.hh"
#include "exception.hh"
#include "utils/logger.hh"
#include "referencecounter.hh"
#include "../tinytex/ginac2formula.hh"
#include "../tinytex/tinytex.hh"
#include "../views/unitrenderer.hh"
#include <QMessageBox>


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
  if (columnCount() <= index.column()) return Qt::NoItemFlags;
  if (rowCount() <= index.row()) return Qt::NoItemFlags;

  // Mark only column 1 & 2 editable
  if ( (1 == index.column()) || (2 == index.column()) || (3 == index.column()) )
    item_flags |= Qt::ItemIsEditable;

  return item_flags;
}


QVariant
CompartmentList::data(const QModelIndex &index, int role) const
{
  // Filter invalid indices:
  if (! index.isValid() || columnCount() <= index.column()) { return QVariant(); }
  if (rowCount() <= index.row()) { return QVariant(); }

  // Get selected compartment:
  Fluc::Ast::Compartment *comp = this->_model->getCompartment(index.row());

  // Dispatch:
  switch (index.column()) {
  case 0: return _getIdentifier(comp, role);
  case 1: return _getName(comp, role);
  case 2: return _getInitValue(comp, role);
  case 3: return _getUnit(comp, role);
  case 4: return _getConstFlag(comp, role);
  default: break;
  }

  return QVariant();
}


bool
CompartmentList::setData(const QModelIndex &index, const QVariant &value, int role)
{
  // Filter invald indices:
  if (index.row() >= rowCount()) return false;
  if (index.column() >= columnCount()) return false;

  // Get compartment:
  Fluc::Ast::Compartment *comp = _model->getCompartment(index.row());

  // Dispatch
  bool success = false;
  switch(index.column()) {
  case 1: success = _updateName(comp, value); break;
  case 2: success = _updateInitValue(comp, value); break;
  case 3: success = _updateUnit(comp, value); break;
  case 4: success = _updateConstFlag(comp, value); break;
  default: break;
  }

  // on success, signal view that data has changed:
  if (success) { emit dataChanged(index, index); }
  return success;
}


QVariant
CompartmentList::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (Qt::DisplayRole != role || orientation != Qt::Horizontal || columnCount() <= section) {
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


int CompartmentList::rowCount(const QModelIndex &parent) const {
  return this->_model->numCompartments();
}


int CompartmentList::columnCount(const QModelIndex &parent) const {
  return 5;
}

Fluc::Ast::Model &
CompartmentList::model() {
  return *_model;
}


QVariant
CompartmentList::_getIdentifier(Fluc::Ast::Compartment *compartment, int role) const
{
  if (Qt::DisplayRole != role) { return QVariant(); }
  return QString(compartment->getIdentifier().c_str());
}


QVariant
CompartmentList::_getName(Fluc::Ast::Compartment *compartment, int role) const
{
  if ( (Qt::DisplayRole != role) && (Qt::EditRole != role)) { return QVariant(); }

  if (Qt::DisplayRole == role) {
    if (compartment->hasName()) {
      return TinyTex::toPixmap(compartment->getName().c_str());
    }
    return TinyTex::toPixmap("<none>");
  } else {
    if (compartment->hasName() ) {
      return QString(compartment->getName().c_str());
    }
  }

  return QString("");
}

bool
CompartmentList::_updateName(Fluc::Ast::Compartment *compartment, const QVariant &value)
{
  compartment->setName(value.toString().toStdString());
  return true;
}


QVariant
CompartmentList::_getInitValue(Fluc::Ast::Compartment *comp, int role) const
{
  if (Qt::DisplayRole == role) {
    // Render initial value:
    return Ginac2Formula::toPixmap(comp->getValue(), *_model);
  } else if (Qt::EditRole == role) {
    // Serialize expression for editing:
    std::stringstream buffer; buffer << comp->getValue();
    return QString(buffer.str().c_str());
  }

  return QVariant();
}

bool
CompartmentList::_updateInitValue(Fluc::Ast::Compartment *compartment, const QVariant &value)
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
  compartment->setValue(new_value);
  return true;
}


QVariant
CompartmentList::_getUnit(Fluc::Ast::Compartment *compartment, int role) const
{
  if ((Qt::DecorationRole != role) || (Qt::EditRole == role)) { return QVariant(); }

  // Return rendered unit for decoration role:
  if (Qt::DecorationRole == role) {
    UnitRenderer renderer(compartment->getUnit());
    return renderer.toPixmap();
  }

  // Return serialized unit for edit role:
  return Fluc::Parser::Unit::UnitParser::write(compartment->getUnit()).c_str();
}


bool
CompartmentList::_updateUnit(Fluc::Ast::Compartment *compartment, const QVariant &value)
{
  // Try to parse the unit from string:
  try {
    Fluc::Ast::Unit unit = Fluc::Parser::Unit::UnitParser::parse(value.toString().toStdString());
    compartment->setUnit(unit);
  } catch (const Fluc::Parser::ParserError &err) {
    return false;
  }
  return true;
}

QVariant
CompartmentList::_getConstFlag(Fluc::Ast::Compartment *compartment, int role) const
{
  if (Qt::CheckStateRole != role) { return QVariant(); }
  if (compartment->isConst()) { return Qt::Checked; }
  return Qt::Unchecked;
}

bool
CompartmentList::_updateConstFlag(Fluc::Ast::Compartment *compartment, const QVariant &value)
{
  if (Qt::Checked == value) { compartment->setConst(true); }
  else { compartment->setConst(false); }
  return true;
}


void
CompartmentList::addCompartment()
{
  std::string identifier = _model->getNewIdentifier("compartment");
  int new_idx = _model->numSpecies();

  // Signal views and add compartment:
  beginInsertRows(QModelIndex(), new_idx, new_idx);
  _model->addDefinition(
        new Fluc::Ast::Compartment(
          identifier, _model->getDefaultVolumeUnit(), Fluc::Ast::Compartment::VOLUME, true));
  endInsertRows();
}


void
CompartmentList::remCompartment(int row)
{
  if (row >= rowCount()) { return; }
  // Get compartment and count its references:
  Fluc::Ast::Compartment *compartment = _model->getCompartment(row);
  ReferenceCounter refs(compartment); _model->accept(refs);

  // Show message id
  if (0 < refs.references().size()) {
    QMessageBox::information(
          0, tr("Can not delete compartment."),
          tr("Can not delete compartment as it is referenced %1").arg(
            QStringList(refs.references()).join(", ")));
    return;
  }

  // otherwise, remove compartment
  beginRemoveRows(QModelIndex(), row, row);
  _model->remDefinition(compartment);
  endRemoveRows();

}
