#ifndef __INA_APP_MODELS_COMPARTMENTDELEGATE_HH__
#define __INA_APP_MODELS_COMPARTMENTDELEGATE_HH__

#include <QItemDelegate>
#include "ast/model.hh"

/** This class implements a simple delegate to enable a QComboBox for compartment selection. */
class CompartmentDelegate : public QItemDelegate
{
  Q_OBJECT

public:
  /** Constructor.
   * The available compartments are taken from the given model. */
  explicit CompartmentDelegate(Fluc::Ast::Model &model, QObject *parent = 0);

  /** Creates the editor widget. */
  QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  /** */
  void setEditorData(QWidget *editor, const QModelIndex &index) const;
  void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
  void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
   /** Holds a weak reference to the model. */
   Fluc::Ast::Model &_model;
};

#endif // COMPARTMENTDELEGATE_HH
