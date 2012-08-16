#ifndef TINYTEXDELEGATE_HH
#define TINYTEXDELEGATE_HH

#include <QStyledItemDelegate>

/** Tiny helper delegeate, that expects some tinyTex code from the model and allows
 * editing of it if the model signals that the index is editable. */
class TinyTexDelegate : public QStyledItemDelegate
{
  Q_OBJECT
public:
  /** Constructor. */
  TinyTexDelegate(QObject *parent = 0);

  /** Creates the editor widget. */
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  /** Updates the combobox with the currently selected compartment. */
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  /** Updates the species with the chosen compartment. */
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  /** Resize. */
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  /** Returns a size hint for the display of the formula. */
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
  /** Draw. */
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

};

#endif // TINYTEXDELEGATE_HH
