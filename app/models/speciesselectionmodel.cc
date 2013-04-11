#include "speciesselectionmodel.hh"
#include <ast/model.hh>
#include "../tinytex/tinytex.hh"

SpeciesSelectionModel::SpeciesSelectionModel(iNA::Ast::Model *model, QObject *parent) :
  QAbstractTableModel(parent), _model(model), _selection(_model->numSpecies(), false)
{
  // Pass...
}

QStringList
SpeciesSelectionModel::selectedSpecies() {
  QStringList selected_species;
  for (int i=0; i<_selection.size(); i++) {
    if (_selection[i]) {
      selected_species.append(QString::fromStdString(_model->getSpecies(i)->getIdentifier()));
    }
  }
  return selected_species;
}

void
SpeciesSelectionModel::selectAllSpecies() {
  for (int i=0; i<_selection.size(); i++) {
    _selection[i] = true;
  }
  if (0 < _model->numSpecies()) {
    dataChanged(index(0, 0), index(_model->numSpecies()-1, 0));
  }
  emit selectionChanged();
}

void
SpeciesSelectionModel::selectNoSpecies() {
  for (int i=0; i<_selection.size(); i++) {
    _selection[i] = false;
  }
  if (0 < _model->numSpecies()) {
    dataChanged(index(0, 0), index(_model->numSpecies()-1, 0));
  }
  emit selectionChanged();
}

void
SpeciesSelectionModel::invertSelection() {
  for (int i=0; i<_selection.size(); i++) {
    _selection[i] = ! _selection[i];
  }
  if (0 < _model->numSpecies()) {
    dataChanged(index(0, 0), index(_model->numSpecies()-1, 0));
  }
  emit selectionChanged();
}

int
SpeciesSelectionModel::rowCount(const QModelIndex &parent) const {
  return _model->numSpecies();
}

int
SpeciesSelectionModel::columnCount(const QModelIndex &parent) const {
  return 1;
}

Qt::ItemFlags
SpeciesSelectionModel::flags(const QModelIndex &index) const {
  return Qt::NoItemFlags | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
}

QVariant
SpeciesSelectionModel::data(const QModelIndex &index, int role) const
{
  if (index.row() >= int(_model->numSpecies())) { return QVariant(); }
  if (index.column() != 0) { return QVariant(); }

  if (Qt::CheckStateRole == role) {
    return _selection[index.row()] ? Qt::Checked : Qt::Unchecked;
  }

  if (Qt::DecorationRole != role) { return QVariant(); }
  return TinyTex::toPixmap(_model->getSpecies(index.row())->getLabel());
}

bool
SpeciesSelectionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if (index.row() >= int(_model->numSpecies())) { return false; }
  if (Qt::CheckStateRole != role) { return false; }
  if (index.column() != 0) { return false; }

  _selection[index.row()] = value.toBool();
  emit selectionChanged();
  return true;
}

