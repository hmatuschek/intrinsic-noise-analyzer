#ifndef __INA_APP_MODELS_PIXMAPDELEGATE_HH__
#define __INA_APP_MODELS_PIXMAPDELEGATE_HH__

#include <QStyledItemDelegate>


/** Tiny helper class to display pixmaps returned from models as @c Qt::DisplayRole. */
class PixmapDelegate
    : public QStyledItemDelegate
{
  Q_OBJECT

public:
  /** Trivial constructor. */
  explicit PixmapDelegate(QObject *parent = 0);

  /** Returns the pixmap size. */
  virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

  /** Implements the rendering of the pixmap. */
  virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // PIXMAPDELEGATE_HH
