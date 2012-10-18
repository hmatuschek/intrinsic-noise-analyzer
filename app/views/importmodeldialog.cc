#include "importmodeldialog.hh"
#include <QFormLayout>
#include <QFileInfo>


ModelFormatQuestion::ModelFormatQuestion(const QString &filename, QWidget *parent) :
  QDialog(parent)
{
  setWindowTitle("Import model");

  _format_selector = new QComboBox();
  _format_selector->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
  _format_selector->addItem(tr("SBML"), unsigned(SBML_MODEL));
  _format_selector->addItem(tr("SBML shorthand"), unsigned(SBMLSH_MODEL));
  _format_selector->setCurrentIndex(0);

  QLabel *text = new QLabel(tr("iNA can not determine the format of the model description from"
                               " the file extension. Please select the format below."));

  _buttons = new QDialogButtonBox(QDialogButtonBox::Open|QDialogButtonBox::Cancel);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(text);
  layout->addWidget(_format_selector);
  layout->addWidget(_buttons);
  setLayout(layout);

  QObject::connect(_buttons, SIGNAL(accepted()), this, SLOT(accept()));
  QObject::connect(_buttons, SIGNAL(rejected()), this, SLOT(reject()));
}


ModelFormatQuestion::Format
ModelFormatQuestion::getFormat() const
{
  return Format(_format_selector->itemData(_format_selector->currentIndex()).toUInt());
}
