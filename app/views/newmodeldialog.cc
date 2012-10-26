#include "newmodeldialog.hh"
#include <QFormLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>


NewModelDialog::NewModelDialog(QWidget *parent) :
  QDialog(parent)
{
  // set title:
  setWindowTitle(tr("New model"));

  _modelIdentifier = new QLineEdit(tr("model"));
  _modelName = new QLineEdit(tr("Model"));

  QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

  QFormLayout *dialog_layout = new QFormLayout();
  dialog_layout->addRow(tr("Name"), _modelName);
  dialog_layout->addRow(tr("Identifier"), _modelIdentifier);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(dialog_layout);
  layout->addWidget(buttons);
  setLayout(layout);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
}


QString
NewModelDialog::getModelIdentifier() const {
  return _modelIdentifier->text();
}

QString
NewModelDialog::getModelName() const {
  return _modelName->text();
}
