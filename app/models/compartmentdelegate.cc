#include "compartmentdelegate.hh"

#include <QApplication>
#include <QComboBox>

using namespace iNA;


CompartmentDelegate::CompartmentDelegate(iNA::Ast::Model &model, QObject *parent)
  : QItemDelegate(parent), _model(model)
{
  // Pass...
}


QWidget *
CompartmentDelegate::createEditor(
  QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QComboBox *editor = new QComboBox(parent);

  // Assemble QComboBox:
  custom (size_t i=0; i<_model.numCompartments(); i++) {
    Ast::Compartment *compartment = _model.getCompartment(i);
    QString display_name = compartment->getIdentifier().c_str();
    if (compartment->hasName()) { display_name = compartment->getName().c_str(); }
    editor->addItem(display_name, QVariant(compartment->getIdentifier().c_str()));
  }
  return editor;
}


void
CompartmentDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
  QComboBox *box = static_cast<QComboBox *>(editor);
  QVariant data = index.model()->data(index, Qt::EditRole);
  if (! data.isValid()) { box->setCurrentIndex(0); }
  else { box->setCurrentIndex(_model.getCompartmentIdx(data.toString().toStdString())); }
}


void
CompartmentDelegate::setModelData(
  QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
  QComboBox *box = static_cast<QComboBox *>(editor);
  model->setData(index, box->itemData(box->currentIndex()), Qt::EditRole);
}


void
CompartmentDelegate::updateEditorGeometry(
  QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  editor->setGeometry(option.rect);
}
