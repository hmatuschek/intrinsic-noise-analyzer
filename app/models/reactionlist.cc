#include "reactionlist.hh"
#include "../tinytex/tinytex.hh"
#include "../views/reactionequationrenderer.hh"



ReactionList::ReactionList(iNA::Ast::Model *model, QObject *parent)
  : QAbstractTableModel(parent), _model(model)
{
  /* Pass... */
}


iNA::Ast::Model & ReactionList::getModel() { return *_model; }

int ReactionList::rowCount(const QModelIndex &parent) const { return _model->numReactions(); }
int ReactionList::columnCount(const QModelIndex &parent) const { return 3; }

QVariant
ReactionList::headerData(int section, Qt::Orientation orientation, int role) const {
  // Return default header for rows:
  if (Qt::DisplayRole != role || orientation != Qt::Horizontal || columnCount() <= section) {
    return QAbstractTableModel::headerData(section, orientation, role);
  }

  // handle column headers:
  switch (section) {
  case 0: return QVariant("Name");
  case 1: return QVariant("Reaction");
  case 2: return QVariant("Propensity");
  default: break;
  }

  return QVariant();
}


QVariant
ReactionList::data(const QModelIndex &index, int role) const
{
  if (! index.isValid() || rowCount() <= index.row() || columnCount() <= index.column()) {
    return QVariant();
  }

  iNA::Ast::Reaction *reac = _model->getReaction(index.row());

  switch (index.column()) {
  case 0: return getName(reac, role);
  case 1: return getReaction(reac, role);
  case 2: return getKineticLaw(reac, role);
  default: break;
  }

  return QVariant();
}


QVariant
ReactionList::getName(iNA::Ast::Reaction *reaction, int role) const
{
  if (Qt::DecorationRole != role) { return QVariant(); }

  if (reaction->hasName()) {
    return TinyTex::toPixmap(reaction->getName().c_str());
  } else {
    return TinyTex::toPixmap(reaction->getIdentifier().c_str());
  }
}


QVariant
ReactionList::getReaction(iNA::Ast::Reaction *reaction, int role) const
{
  if (Qt::DecorationRole != role) { return QVariant(); }

  return ReactionEquationRenderer::renderReactionEquation(reaction);
}


QVariant
ReactionList::getKineticLaw(iNA::Ast::Reaction *reaction, int role) const
{
  if (Qt::DecorationRole != role) { return QVariant(); }

  return ReactionEquationRenderer::renderKineticLaw(reaction);
}
