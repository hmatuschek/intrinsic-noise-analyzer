#ifndef __INA_APP_MODELS_EXPRESSIONDELEGATE_HH__
#define __INA_APP_MODELS_EXPRESSIONDELEGATE_HH__

#include "pixmapdelegate.hh"
#include "ast/scope.hh"


/** This class implements a delegate for expressions to provide a specialized editor. */
class ExpressionDelegate : public PixmapDelegate
{
  Q_OBJECT

public:
  /** Constructs the delegate. */
  explicit ExpressionDelegate(iNA::Ast::Scope &scope, QObject *parent = 0);

  /** Creates the editor widget. */
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  /** Resize. */
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
   /** Holds a weak reference to the model. */
   iNA::Ast::Scope &_scope;
};

#endif // EXPRESSIONDELEGATE_HH
