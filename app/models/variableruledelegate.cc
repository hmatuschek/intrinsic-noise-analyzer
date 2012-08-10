#include "variableruledelegate.hh"
#include "../views/variableruleeditor.hh"
#include <QApplication>


using namespace Fluc;


SpeciesRuleDelegate::SpeciesRuleDelegate(Fluc::Ast::Model &model, QObject *parent)
  : QItemDelegate(parent), _model(model)
{
  // Pass...
}


QWidget *
SpeciesRuleDelegate::createEditor(
  QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  VariableRuleEditor *editor = new VariableRuleEditor(parent);
  Ast::Species *species = _model.getSpecies(index.row());

  if (species->hasRule()) {
    if (Ast::Node::isAssignmentRule(species->getRule())) {
      editor->setRuleKind(VariableRuleData::ASSIGNMENT_RULE);
    } else {
      editor->setRuleKind(VariableRuleData::RATE_RULE);
    }
    // Serialize expression:
    std::stringstream stream; stream << species->getRule()->getRule();
    editor->setRuleExpression(stream.str().c_str());
  } else {
    editor->setRuleKind(VariableRuleData::NO_RULE);
  }

  return editor;
}


void
SpeciesRuleDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  VariableRuleEditor *rule_edit = static_cast<VariableRuleEditor *>(editor);
  Ast::Species *species = _model.getSpecies(index.row());

  if (species->hasRule()) {
    if (Ast::Node::isAssignmentRule(species->getRule())) {
      rule_edit->setRuleKind(VariableRuleData::ASSIGNMENT_RULE);
    } else {
      rule_edit->setRuleKind(VariableRuleData::RATE_RULE);
    }
    // Serialize expression:
    std::stringstream stream; stream << species->getRule()->getRule();
    rule_edit->setRuleExpression(stream.str().c_str());
  } else {
    rule_edit->setRuleKind(VariableRuleData::NO_RULE);
  }
}


void
SpeciesRuleDelegate::setModelData(
  QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  VariableRuleData *data = new VariableRuleData();
  VariableRuleEditor *rule_edit = static_cast<VariableRuleEditor *>(editor);
  data->setRuleKind(rule_edit->ruleKind());
  data->setRuleExpression(rule_edit->ruleExpression());
  model->setData(index, qVariantFromValue((void *)(data)), Qt::EditRole);
}


void
SpeciesRuleDelegate::updateEditorGeometry(
  QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  editor->setGeometry(option.rect);
}


void
SpeciesRuleDelegate::paint(
  QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyleOptionViewItemV4 myOption = option;
  myOption.text = index.model()->data(index, Qt::DisplayRole).toString();
  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);
}
