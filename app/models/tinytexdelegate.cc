#include "tinytexdelegate.hh"
#include "../tinytex/tinytex.hh"
#include <QLineEdit>
#include <QPainter>

TinyTexDelegate::TinyTexDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
  // Pass...
}


QWidget *
TinyTexDelegate::createEditor(
  QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  return new QLineEdit(parent);
}

void
TinyTexDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QLineEdit *line_edit = static_cast<QLineEdit *>(editor);
  line_edit->setText(index.model()->data(index, Qt::EditRole).toString());
}

void
TinyTexDelegate::setModelData(
  QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QLineEdit *line_edit = static_cast<QLineEdit *>(editor);
  QString code = line_edit->text();
  model->setData(index, code, Qt::EditRole);
}

void
TinyTexDelegate::updateEditorGeometry(
  QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  editor->setGeometry(option.rect);
}

QSize
TinyTexDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QVariant pixmap = TinyTex::toPixmap(
        index.model()->data(index, Qt::DisplayRole).toString().toStdString());
  return pixmap.value<QPixmap>().size();
}

void
TinyTexDelegate::paint(
  QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  // Get TeX image:
  QPixmap image = TinyTex::toPixmap(
        index.model()->data(index, Qt::DisplayRole).toString().toStdString()).value<QPixmap>();

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
