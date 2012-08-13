#include "expressiondelegate.hh"
#include "scopeitemmodel.hh"
#include "../views/expressioneditor.hh"
#include <QLineEdit>
#include <QCompleter>
#include <QApplication>



ExpressionDelegate::ExpressionDelegate(Fluc::Ast::Scope &scope, QObject *parent)
  : QStyledItemDelegate(parent), _scope(scope)
{
  // pass...
}


QWidget *
ExpressionDelegate::createEditor(
  QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QLineEdit *editor=new ExpressionEditor(parent);
  QCompleter *completer = new QCompleter(editor);
  completer->setCompletionMode(QCompleter::InlineCompletion);
  completer->setModel(new ScopeItemModel(_scope, completer));
  editor->setCompleter(completer);

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

void
ExpressionDelegate::paint(
  QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  /*QStyleOptionViewItemV4 myOption = option;
  QVariant icon = index.model()->data(index, Qt::DecorationRole);
  if (icon.isValid()) {
    myOption.icon = icon.value<QPixmap>();
  }
  myOption.text = index.model()->data(index, Qt::DisplayRole).toString();

  QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &myOption, painter);*/
  QStyledItemDelegate::paint(painter, option, index);
}

