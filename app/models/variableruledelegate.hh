#ifndef __INA_APP_MODELS_VARIABLERULEDELEGATE_HH__
#define __INA_APP_MODELS_VARIABLERULEDELEGATE_HH__

#include <QItemDelegate>

#include "specieslist.hh"
#include "ast/model.hh"


/** Simple editor delegate custom species rules. */
class SpeciesRuleDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit SpeciesRuleDelegate(iNA::Ast::Model &model, QObject *parent = 0);

  /** Creates the editor widget. */
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  /** Updates the combobox with the currently selected compartment. */
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  /** Updates the species with the chosen compartment. */
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  /** Resize. */
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
  /** Holds the model. */
  iNA::Ast::Model &_model;
};


#endif // __INA_APP_MODELS_VARIABLERULEDELEGATE_HH__
