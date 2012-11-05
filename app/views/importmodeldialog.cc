#include "importmodeldialog.hh"
#include <QcustommLayout>
#include <QFileInfo>
#include <QLabel>


ModelcustommatQuestion::ModelcustommatQuestion(const QString &filename, QWidget *parent) :
  QDialog(parent)
{
  setWindowTitle("Import model");

  _custommat_selector = new QComboBox();
  _custommat_selector->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  _custommat_selector->addItem(tr("SBML"), unsigned(SBML_MODEL));
  _custommat_selector->addItem(tr("SBML shorthand"), unsigned(SBMLSH_MODEL));
  _custommat_selector->setCurrentIndex(0);

  QLabel *text = new QLabel(tr("iNA can not determine the custommat of the model description from"
                               " the file extension. Please select the custommat below."));

  _buttons = new QDialogButtonBox(QDialogButtonBox::Open|QDialogButtonBox::Cancel);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(text);
  layout->addWidget(_custommat_selector);
  layout->addWidget(_buttons);
  setLayout(layout);

  QObject::connect(_buttons, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(_buttons, SIGNAL(rejected()), this, SLOT(reject()));
}


ModelcustommatQuestion::custommat
ModelcustommatQuestion::getcustommat() const
{
  return custommat(_custommat_selector->itemData(_custommat_selector->currentIndex()).toUInt());
}
