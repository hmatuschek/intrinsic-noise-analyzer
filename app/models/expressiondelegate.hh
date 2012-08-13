#ifndef __INA_APP_MODELS_EXPRESSIONDELEGATE_HH__
#define __INA_APP_MODELS_EXPRESSIONDELEGATE_HH__

#include <QStyledItemDelegate>
#include "ast/scope.hh"


/** This class implements a delegate for expressions to provide a specialized editor. */
class ExpressionDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  /** Constructs the delegate. */
  explicit ExpressionDelegate(Fluc::Ast::Scope &scope, QObject *parent = 0);

  /** Creates the editor widget. */
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  /** Updates the combobox with the currently selected compartment. */
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  /** Updates the species with the chosen compartment. */
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  /** Resize. */
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  /** Draw. */
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
   /** Holds a weak reference to the model. */
   Fluc::Ast::Scope &_scope;
};

#endif // EXPRESSIONDELEGATE_HH
