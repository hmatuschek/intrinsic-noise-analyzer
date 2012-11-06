#include "expressiondelegate.hh"
#include "scopeitemmodel.hh"
#include "../views/expressioneditor.hh"
#include "../tinytex/ginac2formula.hh"
#include <QLineEdit>
#include <QCompleter>
#include <QApplication>
#include <QPainter>



ExpressionDelegate::ExpressionDelegate(iNA::Ast::Scope &scope, QObject *parent)
  : PixmapDelegate(parent), _scope(scope)
{
  // pass...
}


QWidget *
ExpressionDelegate::createEditor(
  QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QLineEdit *editor = new ExpressionEditor(_scope, parent);
  return editor;
}


void
ExpressionDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QLineEdit *line_edit = static_cast<QLineEdit *>(editor);
  QVariant data = index.model()->data(index, Qt::EditRole);
  if (data.isValid()) { line_edit->setText(data.toString()); }
}


void
ExpressionDelegate::setModelData(
  QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QLineEdit *line_edit = static_cast<QLineEdit *>(editor);
  model->setData(index, line_edit->text(), Qt::EditRole);
}

void
ExpressionDelegate::updateEditorGeometry(
  QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  editor->setGeometry(option.rect);
}
