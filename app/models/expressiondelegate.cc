#include "expressiondelegate.hh"
#include "scopeitemmodel.hh"
#include "../views/expressioneditor.hh"
#include "../tinytex/ginac2formula.hh"
#include <QLineEdit>
#include <QCompleter>
#include <QApplication>
#include <QPainter>



ExpressionDelegate::ExpressionDelegate(Fluc::Ast::Scope &scope, QObject *parent)
  : QStyledItemDelegate(parent), _scope(scope)
{
  // pass...
}


QWidget *
ExpressionDelegate::createEditor(
  QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QLineEdit *editor = new ExpressionEditor(parent);
  /*QCompleter *completer = new QCompleter(editor);
  completer->setCompletionMode(QCompleter::InlineCompletion);
  completer->setModel(new ScopeItemModel(_scope, completer));
  editor->setCompleter(completer);*/

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
  // Get TeX image
  QPixmap image = index.model()->data(index, Qt::DisplayRole).value<QPixmap>();

  // Just get default style options:
  QStyleOptionViewItemV4 myoptions(option); initStyleOption(&myoptions, index);
  // Save painter state...
  painter->save();
  // Clip to myoptions.rect:
  painter->setClipRect(myoptions.rect);
  // Fill background:
  painter->fillRect(myoptions.rect, myoptions.backgroundBrush);
  // Center horizontally
  int yoff = (myoptions.rect.height() - image.height())/2;
  // Just some small offset
  int xoff = 3;
  // Render text into given painter:
  painter->drawPixmap(QPoint(myoptions.rect.left()+xoff,myoptions.rect.top()+yoff), image);
  // restore painter
  painter->restore();
}

