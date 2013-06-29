#include "pixmapdelegate.hh"
#include <QPixmap>
#include <QPainter>


PixmapDelegate::PixmapDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
  // Pass...
}

QSize
PixmapDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
  QPixmap image = index.model()->data(index, Qt::DisplayRole).value<QPixmap>();
  return image.size()+QSize(6,0);
}

void
PixmapDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
